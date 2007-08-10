/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidget.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "igstkFLTKWidget.h"
#include <FL/x.H>

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

namespace igstk
{

/** Constructor */
FLTKWidget::FLTKWidget( int x, int y, int w, int h, const char *l ) : 
Fl_Gl_Window( x, y, w, h, l ), m_StateMachine(this), m_ProxyView(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  this->end();

  m_PointPicker = PickerType::New();
  m_Reporter    = ::itk::Object::New();

  //Turn on interaction handling
  m_InteractionHandling = true;


  m_RenderWindowIDSet = false;

  m_Renderer = NULL;
  m_RenderWindowInteractor = NULL;

  // instantiate the view object 
  m_View = ViewType::New(); 

  igstkAddInputMacro( ValidView );
  igstkAddInputMacro( InValidView );
  igstkAddInputMacro( EnableInteractions );
  igstkAddInputMacro( DisableInteractions );

  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ViewConnected );

  igstkAddTransitionMacro( Idle, ValidView, ViewConnected, ConnectView );   

  igstkAddTransitionMacro( Idle, EnableInteractions,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, DisableInteractions,
                           Idle,  ReportInvalidRequest );

  igstkAddTransitionMacro( ViewConnected, EnableInteractions,
                           ViewConnected,  EnableInteractions );
  igstkAddTransitionMacro( ViewConnected, DisableInteractions,
                           ViewConnected,  DisableInteractions );

  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();


}

/** Destructor */
FLTKWidget::~FLTKWidget()
{
  igstkLogMacro( DEBUG, "FLTKWidget Destructor() called ...\n");

  if ( ! m_View.IsNull() ) 
    {
    this->m_View->RequestStop();


    if ( m_Renderer != NULL )
      {
      vtkRenderWindow * renderWindow = m_Renderer->GetRenderWindow();

      if ( renderWindow != NULL )
        {
        #if defined(WIN32)
          renderWindow->SetWindowId( NULL );
        #endif
        }
      }
    }

  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    {
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
    }

  m_PointPicker->Delete();
}

/** Set VTK renderer */
void FLTKWidget::SetRenderer( vtkRenderer * renderer )
{
  this->m_Renderer = renderer;
}

/** Set VTK renderw window interactor */
void FLTKWidget::SetRenderWindowInteractor( vtkRenderWindowInteractor * interactor )
{
  this->m_RenderWindowInteractor = interactor;
}

/** Request set View */
void FLTKWidget::RequestSetView( ViewType::Pointer view )
{
  igstkLogMacro( DEBUG, "RequestSetView called ...\n");
  //FIXME: insert a logic to check if the specified view is 
  //valid or not
  m_View = view;
  igstkPushInputMacro( ValidView );
  m_StateMachine.ProcessInputs();
}

/** Connect view  */
void FLTKWidget::ConnectViewProcessing( )
{
  igstkLogMacro( DEBUG, "ConnectViewProcessing called ...\n");

  this->m_ProxyView.Connect( m_View );
  
  m_Renderer->GetRenderWindow()->GetInteractor()->SetPicker( m_PointPicker );

  //Add actors to the point picker list
  vtkPropCollection * propList = this->m_Renderer->GetViewProps();
  vtkProp * prop;

  while(prop = propList->GetNextProp())
    {
    this->m_PointPicker->AddPickList(prop);
    }
}

/** Set render window */
void FLTKWidget::SetRenderWindowID(void)
{

  if ( m_Renderer == NULL ) 
    {
    return;
    }

  vtkRenderWindow * renderWindow = m_Renderer->GetRenderWindow();

  if ( renderWindow == NULL ) 
    {
    return;
    }

  m_RenderWindowIDSet = true;

  #if defined(__APPLE__) && defined(VTK_USE_CARBON)
    // FLTK 1.x does not support HiView
    ((vtkCarbonRenderWindow *)renderWindow)->SetRootWindow( fl_xid( this ) );
  #else
    renderWindow->SetWindowId( (void *)fl_xid( this ) );
  #endif
  #if !defined(WIN32) && !defined(__APPLE__)
    renderWindow->SetDisplayId( fl_display );
  #endif

}
 
/** Request enable interactions */
void FLTKWidget::RequestEnableInteractions()
{
  igstkLogMacro( DEBUG, "RequestEnableInteractions() called ...\n");
  igstkPushInputMacro( EnableInteractions );
  m_StateMachine.ProcessInputs();
}


/** Request disable interactions */
void FLTKWidget::RequestDisableInteractions()
{
  igstkLogMacro( DEBUG, "RequestDisableInteractions() called ...\n");
  igstkPushInputMacro( DisableInteractions );
  m_StateMachine.ProcessInputs();
}

/** */
void FLTKWidget::EnableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "EnableInteractionsProcessing() called ...\n");
  m_InteractionHandling = true;
}

/** */
void FLTKWidget::DisableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "DisableInteractionsProcessing() called ...\n");
  m_InteractionHandling = false;
}


/** FLTK event handlers */
void FLTKWidget::flush(void)
{
  igstkLogMacro( DEBUG, "flush() called ...\n");
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  this->draw();
}

/** Draw function */
void FLTKWidget::draw(void)
{
  igstkLogMacro( DEBUG, "draw() called ...\n");

  // make sure the vtk part knows where and how large we are
  //
  if ( ! m_View.IsNull() )
    {
    m_View->RequestSetRenderWindowSize( this->w(), this->h() );
    }

  // make sure the GL context exists and is current:
  // after a hide() and show() sequence e.g. there is no context yet
  // and the Render() will fail due to an invalid context.
  // see Fl_Gl_Window::show()
  this->make_current();

  if ( ! m_RenderWindowIDSet )
    {
    this->SetRenderWindowID();
    }

  vtkRenderWindowInteractor * interactor = m_RenderWindowInteractor; 

  if ( interactor != NULL )
    {
    interactor->Render();
    }
}

/** Resize function */
void FLTKWidget::resize( int x, int y, int w, int h ) 
{
  igstkLogMacro( DEBUG, "resize() called ...\n");

  // make sure VTK knows about the new situation
  if ( ! m_View.IsNull() )
    {
    m_View->RequestSetRenderWindowSize( w, h );
    }

  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}


/** main FLTK event handler */
int FLTKWidget::handle( int event ) 
{
  igstkLogMacro( DEBUG, "handle() called ...\n");

  vtkRenderWindowInteractor * renderWindowInteractor = 
                                                   m_RenderWindowInteractor;

  if ( renderWindowInteractor == NULL )
    {
    return 0;
    }

  if( !renderWindowInteractor->GetEnabled() || !m_InteractionHandling) 
    {
    return 0;
    }
  // SEI(x, y, ctrl, shift, keycode, repeatcount, keysym)
  renderWindowInteractor->SetEventInformation(
                            Fl::event_x(), this->h()-Fl::event_y()-1, 
                            Fl::event_state( FL_CTRL ), 
                            Fl::event_state( FL_SHIFT ),
                            Fl::event_key(), 1, NULL);   
    
  switch( event ) 
    {
    case FL_FOCUS:
    case FL_UNFOCUS:
      // Return 1 if you want keyboard events, 0 otherwise. Yes we do
      break;

    case FL_KEYBOARD:   // keypress
      renderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
      
      // Disabling VTK keyboard interaction
      //this->InvokeEvent(vtkCommand::KeyPressEvent, NULL);
      //this->InvokeEvent(vtkCommand::CharEvent, NULL);
     
      // now for possible controversy: there
      // is no way to find out if the
      // InteractorStyle actually did
      // something with this event.  To play
      // it safe (and have working hotkeys),
      // we return "0", which indicates to
      // FLTK that we did NOTHING with this
      // event.  FLTK will send this keyboard
      // event to other children in our group,
      // meaning it should reach any FLTK
      // keyboard callbacks (including
      // hotkeys)
      return 0;
      break;
     
    case FL_PUSH: // mouse down
      this->take_focus();  // this allows key events to work
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          renderWindowInteractor->InvokeEvent(
                                      vtkCommand::LeftButtonPressEvent,NULL);
          break;
        case FL_MIDDLE_MOUSE:
          renderWindowInteractor->InvokeEvent(
                                    vtkCommand::MiddleButtonPressEvent,NULL);
          break;
        case FL_RIGHT_MOUSE:
          renderWindowInteractor->InvokeEvent(
                                     vtkCommand::RightButtonPressEvent,NULL);
          break;
        }
      break; // this break should be here, at least according to 
             // vtkXRenderWindowInteractor

      // we test for both of these, as fltk classifies mouse moves as 
      // with or without button press whereas vtk wants all mouse movement 
      // (this bug took a while to find :)
    case FL_DRAG:
    case FL_MOVE:
      renderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
    break;

    case FL_RELEASE:    // mouse up
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          {
          renderWindowInteractor->InvokeEvent(
                                     vtkCommand::LeftButtonReleaseEvent,NULL);
          }
          break;
        case FL_MIDDLE_MOUSE:
          renderWindowInteractor->InvokeEvent(
                                   vtkCommand::MiddleButtonReleaseEvent,NULL);
          break;
        case FL_RIGHT_MOUSE:
          renderWindowInteractor->InvokeEvent(
                                    vtkCommand::RightButtonReleaseEvent,NULL);
          break;
        }
      break;

    default:    // let the base class handle everything else 
    return Fl_Gl_Window::handle( event );
    } // switch(event)...


  return 1; // we handled the event if we didn't return earlier
}

/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this class. */
void FLTKWidget::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** Report that an invalid view component is specified */
void FLTKWidget::ReportInvalidViewConnectedProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidViewConnectedProcessing() called ...\n");
}


} // end namespace igstk
