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
Fl_Gl_Window( x, y, w, h, l ), m_StateMachine(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  this->end();

  //Turn on interaction handling
  m_InteractionHandling = true;

  // instantiate the view object 
  m_View = ViewType::New();
}

/** Destructor */
FLTKWidget::~FLTKWidget()
{
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    {
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
    }

}

/** Set View */
void FLTKWidget::SetView( ViewType::Pointer view )
{
  m_View = view;
}

/** Update the display */
void FLTKWidget::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  this->m_View->Update();
}

/** */
void FLTKWidget::EnableInteractions()
{
  m_InteractionHandling = true;
}

/** */
void FLTKWidget::DisableInteractions()
{
  m_InteractionHandling = false;
}

/** this gets called during FLTK window draw()s and resize()s */
void FLTKWidget::UpdateSize(int W, int H)
{
  igstkLogMacro( DEBUG, "UpdateSize() called ...\n");
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void FLTKWidget::Refresh()
{
  igstkLogMacro( DEBUG, "Refresh() called ...\n");
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
  m_View->UpdateSize( this->w(), this->h() );

  // make sure the GL context exists and is current:
  // after a hide() and show() sequence e.g. there is no context yet
  // and the Render() will fail due to an invalid context.
  // see Fl_Gl_Window::show()
  this->make_current();

  vtkRenderWindow * renderWindow = m_View->GetRenderWindow();
  vtkRenderWindowInteractor * interactor = m_View->GetRenderWindowInteractor();

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
  // FLTK 1.x does not support HiView
  ((vtkCarbonRenderWindow *)renderWindow)->SetRootWindow( fl_xid( this ) );
#else
  renderWindow->SetWindowId( (void *)fl_xid( this ) );
#endif
#if !defined(WIN32) && !defined(__APPLE__)
  renderWindow->SetDisplayId( fl_display );
#endif
  // get vtk to render to the Fl_Gl_Window
  interactor->Render();
}

/** Resize function */
void FLTKWidget::resize( int x, int y, int w, int h ) 
{
  igstkLogMacro( DEBUG, "resize() called ...\n");

  // make sure VTK knows about the new situation
  UpdateSize( w, h );
  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}


/** main FLTK event handler */
int FLTKWidget::handle( int event ) 
{
  igstkLogMacro( DEBUG, "handle() called ...\n");
  
  switch( event ) 
    {
    case FL_FOCUS:
    case FL_UNFOCUS:
      break;

    case FL_KEYBOARD:   // keypress
      return 0;
      break;
     
    case FL_PUSH: // mouse down
      this->take_focus();  // this allows key events to work
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          break;
        case FL_MIDDLE_MOUSE:
          break;
        case FL_RIGHT_MOUSE:
          break;
        }
      break; // this break should be here, at least according to 
    case FL_DRAG:
    case FL_MOVE:
    break;

    case FL_RELEASE:    // mouse up
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          {
          }
          break;
        case FL_MIDDLE_MOUSE:
          break;
        case FL_RIGHT_MOUSE:
          break;
        }
      break;

    default:    // let the base class handle everything else 
    return Fl_Gl_Window::handle( event );
    } // switch(event)...

  return 1; // we handled the event if we didn't return earlier
}

} // end namespace igstk
