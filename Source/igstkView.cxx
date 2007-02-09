/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView.cxx
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

#include "igstkView.h"
#include <FL/x.H>
#include <vtkVersion.h>
#include <vtkCommand.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <igstkEvents.h>
#include "itksys/SystemTools.hxx"
#include "vtkViewport.h"

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

namespace igstk
{

/** Constructor */
View::View( int x, int y, int w, int h, const char *l ) : 
Fl_Gl_Window( x, y, w, h, l ), m_StateMachine(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  // Create a default render window
  m_RenderWindow = vtkRenderWindow::New();
  m_Renderer = vtkRenderer::New();
  m_PointPicker = PickerType::New();
  m_RenderWindowInteractor = RenderWindowInteractor::New();

  m_RenderWindow->AddRenderer( m_Renderer );
  m_Camera = m_Renderer->GetActiveCamera();
  m_RenderWindow->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);

  m_RenderWindow->SetSize( this->w(), this->h() );

  int * size = m_RenderWindow->GetSize();
    
  m_RenderWindowInteractor->SetSize( size );

  m_RenderWindowInteractor->SetPicker( m_PointPicker );

  m_RenderWindowInteractor->SetRenderWindow( m_RenderWindow );

  // Set the default view to be axial
  m_Camera->SetPosition ( 0, 0, -1 );
  m_Camera->SetViewUp( 0, -1, 0 );
  m_Renderer->ResetCamera();
  
  this->end();

  m_InteractionHandling = true;

  igstkAddInputMacro( ValidAddObject );
  igstkAddInputMacro( NullAddObject  );
  igstkAddInputMacro( ValidAddAnnotation2D );
  igstkAddInputMacro( NullAddAnnotation2D  );
  igstkAddInputMacro( ExistingAddObject );
  igstkAddInputMacro( ValidRemoveObject );
  igstkAddInputMacro( InexistingRemoveObject );
  igstkAddInputMacro( NullRemoveObject  );
  igstkAddInputMacro( ValidAddActor );
  igstkAddInputMacro( NullAddActor  );
  igstkAddInputMacro( ValidRemoveActor );
  igstkAddInputMacro( NullRemoveActor  );
  igstkAddInputMacro( ResetCamera  );
  igstkAddInputMacro( EnableInteractions  );
  igstkAddInputMacro( DisableInteractions  );
  igstkAddInputMacro( StartRefreshing  );
  igstkAddInputMacro( StopRefreshing  );
  igstkAddInputMacro( ValidScreenShotFileName  );
  igstkAddInputMacro( InvalidScreenShotFileName  );


  igstkAddStateMacro( Idle       );
  igstkAddStateMacro( Refreshing );


  igstkAddTransitionMacro( Idle, ValidAddObject, 
                           Idle,  AddObject );
  igstkAddTransitionMacro( Idle, NullAddObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidAddAnnotation2D,
                           Idle,  AddAnnotation2D );
  igstkAddTransitionMacro( Idle, NullAddAnnotation2D,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ExistingAddObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidRemoveObject,
                           Idle,  RemoveObject );
  igstkAddTransitionMacro( Idle, NullRemoveObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, InexistingRemoveObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidAddActor,
                           Idle,  AddActor );
  igstkAddTransitionMacro( Idle, NullAddActor,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidRemoveActor,
                           Idle,  RemoveActor );
  igstkAddTransitionMacro( Idle, NullRemoveActor,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ResetCamera,
                           Idle,  ResetCamera );
  igstkAddTransitionMacro( Idle, EnableInteractions,
                           Idle,  EnableInteractions );
  igstkAddTransitionMacro( Idle, DisableInteractions,
                           Idle,  DisableInteractions );
  igstkAddTransitionMacro( Idle, StartRefreshing,
                           Refreshing,  Start );
  igstkAddTransitionMacro( Idle, StopRefreshing,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidScreenShotFileName,
                           Idle, SaveScreenShotWhileIdle )
  igstkAddTransitionMacro( Idle, InvalidScreenShotFileName,
                           Idle, ReportInvalidScreenShotFileName );
  igstkAddTransitionMacro( Refreshing, ValidAddObject,
                           Refreshing,  AddObject );
  igstkAddTransitionMacro( Refreshing, NullAddObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidAddAnnotation2D,
                           Refreshing,  AddAnnotation2D );
  igstkAddTransitionMacro( Refreshing, NullAddAnnotation2D,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ExistingAddObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidRemoveObject,
                           Refreshing,  RemoveObject );
  igstkAddTransitionMacro( Refreshing, NullRemoveObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, InexistingRemoveObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidAddActor,
                           Refreshing,  AddActor );
  igstkAddTransitionMacro( Refreshing, NullAddActor,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidRemoveActor,
                           Refreshing,  RemoveActor );
  igstkAddTransitionMacro( Refreshing, NullRemoveActor,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ResetCamera,
                           Refreshing,  ResetCamera );
  igstkAddTransitionMacro( Refreshing, EnableInteractions,
                           Refreshing,  EnableInteractions );
  igstkAddTransitionMacro( Refreshing, DisableInteractions,
                           Refreshing,  DisableInteractions );
  igstkAddTransitionMacro( Refreshing, StartRefreshing,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, StopRefreshing,
                           Idle,  Stop );
  igstkAddTransitionMacro( Refreshing, ValidScreenShotFileName,
                           Refreshing, SaveScreenShotWhileRefreshing )
  igstkAddTransitionMacro( Refreshing, InvalidScreenShotFileName,
                           Refreshing, ReportInvalidScreenShotFileName );

  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();

  m_ActorToBeAdded = 0;
  m_ActorToBeRemoved = 0;

  m_PulseGenerator = PulseGenerator::New();
  m_Reporter = ::itk::Object::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & View::RefreshRender );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  this->RequestSetRefreshRate( 30 ); // 30 Hz is rather low frequency for video.

  m_RenderWindowInteractor->Initialize();
  
}

/** Destructor */
View::~View()
{
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    {
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
    }

  m_RenderWindowInteractor->SetRenderWindow( NULL ); 

  m_RenderWindowInteractor->SetPicker( NULL );

  m_RenderWindowInteractor->Delete();
  
  m_PointPicker->Delete();

  m_Renderer->Delete();

  m_RenderWindow->Delete();

}

/** Update the display */
void View::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  if( !m_RenderWindowInteractor->GetInitialized() )
    {
    m_RenderWindowInteractor->Initialize();
    }
  this->redraw();
}


void View::AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer )
{
  igstkLogMacro( DEBUG, "AddObserver() called ...\n");
  m_Reporter->AddObserver( event, observer );
}


/** */
void View::RequestAddActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "RequestAddActor() called ...\n");
  m_ActorToBeAdded = actor;
  if( !actor )
    {
    igstkPushInputMacro( NullAddActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidAddActor );
    m_StateMachine.ProcessInputs();
    }
}


/** */
void View::AddActorProcessing()
{
  igstkLogMacro( DEBUG, "AddActorProcessing() called ...\n");
  m_Renderer->AddActor( m_ActorToBeAdded );
  m_PointPicker->AddPickList( m_ActorToBeAdded );
}


/** */
void View::RequestRemoveActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "RequestRemoveActor() called ...\n");
  m_ActorToBeRemoved = actor;
  if( !actor )
    {
    igstkPushInputMacro( NullRemoveActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidRemoveActor );
    m_StateMachine.ProcessInputs();
    }
}


/** */
void View::RemoveActorProcessing()
{
  igstkLogMacro( DEBUG, "RemoveActorProcessing() called ...\n");
  m_Renderer->RemoveActor( m_ActorToBeRemoved );
  m_PointPicker->DeletePickList( m_ActorToBeRemoved );
}


/** */
void View::RequestEnableInteractions()
{
  igstkLogMacro( DEBUG, "RequestEnableInteractions() called ...\n");
  igstkPushInputMacro( EnableInteractions );
  m_StateMachine.ProcessInputs();
}


/** */
void View::RequestDisableInteractions()
{
  igstkLogMacro( DEBUG, "RequestDisableInteractions() called ...\n");
  igstkPushInputMacro( DisableInteractions );
  m_StateMachine.ProcessInputs();
}

/** */
void View::Initialize()
{
  m_RenderWindowInteractor->Initialize();
}

/** */
void View::Enable()
{
  m_RenderWindowInteractor->Enable();
}

/** */
void View::Render()
{
  m_RenderWindowInteractor->Render();
}

/** */
void View::EnableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "EnableInteractionsProcessing() called ...\n");
  m_InteractionHandling = true;
}

/** */
void View::DisableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "DisableInteractionsProcessing() called ...\n");
  m_InteractionHandling = false;
}

/** */
void View::RequestResetCamera()
{
  igstkLogMacro( DEBUG, "RequestResetCamera() called ...\n");
  igstkPushInputMacro( ResetCamera );
  m_StateMachine.ProcessInputs();
}

/** */
void View::ResetCameraProcessing()
{
  igstkLogMacro( DEBUG, "ResetCameraProcessing() called ...\n");
  m_Renderer->ResetCamera();
}

/** */
void View::StartProcessing()
{
  igstkLogMacro( DEBUG, "StartProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStart();
}

/** */
void View::StopProcessing()
{
  igstkLogMacro( DEBUG, "StopProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStop();
}

/** this gets called during FLTK window draw()s and resize()s */
void View::UpdateSize(int W, int H)
{
  igstkLogMacro( DEBUG, "UpdateSize() called ...\n");
  if ( m_RenderWindow != NULL)
    {
    const int * size = m_RenderWindowInteractor->GetSize();
    
    // if the size changed tell render window
    if ( (W != size[0]) || (H != size[1]) )
      {
      // adjust our (m_RenderWindowInteractor size)
      m_RenderWindowInteractor->UpdateSize( W, H );
      // and our RenderWindow's size
      m_RenderWindow->SetSize(W, H);
     
      // FLTK can move widgets on resize; if that happened, make
      // sure the RenderWindow position agrees with that of the
      // Fl_Gl_Window
      int *pos = m_RenderWindow->GetPosition();
      if( pos[0] != x() || pos[1] != y() ) 
        {
        m_RenderWindow->SetPosition( x(), y() );
        }
      }
    }
}

/** Define the refresh rate by programming the internal pulse generator */
void View::RequestSetRefreshRate( double frequencyHz )
{
  igstkLogMacro( DEBUG, "RequestSetRefreshRate() called ...\n");
  // Let the state machine of the pulse generator manage this request
  m_PulseGenerator->RequestSetFrequency( frequencyHz );
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void View::RefreshRender()
{
  igstkLogMacro( DEBUG, "RefreshRender() called ...\n");

  // First, compute the time at which we
  // estimate that the scene will be rendered
  TimeStamp renderTime;
  double frequency = m_PulseGenerator->GetFrequency();
  // Frequency is in hertz but period is expected to be in milliseconds
  renderTime.SetStartTimeNowAndExpireAfter( 1000.0 / frequency );

  // Second, notify all the representation object of the time at which this
  // scene will be rendered.
  ObjectListType::iterator itr    = m_Objects.begin();
  ObjectListType::iterator endItr = m_Objects.end();
  while( itr != endItr )
    {
    (*itr)->RequestUpdateRepresentation( renderTime );
    (*itr)->RequestUpdatePosition( renderTime );
    ++itr;
    }

  // Third, trigger VTK rendering by invoking a refresh of the GUI.
  this->redraw();

  // Last, report to observers that a refresh event took place.
  m_Reporter->InvokeEvent( RefreshEvent() );
}

/** Request for Adding an object to the View */
void View::RequestAddObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "RequestAddObject() called ...\n");

  m_ObjectToBeAdded = pointer;

  if( !pointer )
    {
    igstkPushInputMacro( NullAddObject );
    m_StateMachine.ProcessInputs();
    return;
    }

  ObjectListType::iterator it =
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( it != m_Objects.end() )
    {
    igstkPushInputMacro( ExistingAddObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidAddObject );
    m_StateMachine.ProcessInputs();
    }
}

/** Request for adding annotation */
void View::RequestAddAnnotation2D ( Annotation2D * annotation )
{
  igstkLogMacro( DEBUG, "RequestAddAnnotation2D() called ...\n");

  m_Annotation2DToBeAdded = annotation;

  if( !annotation )
    {
    igstkPushInputMacro( NullAddAnnotation2D );
    m_StateMachine.ProcessInputs();
    return;
    }

  igstkPushInputMacro( ValidAddAnnotation2D );
  m_StateMachine.ProcessInputs();
}


/** Add an object to the View. This method should only be called by the state
 * machine. The state machine makes sure that this method is called with a valid
 * value in the ObjectToBeAdded. */
void View::AddObjectProcessing()
{
  igstkLogMacro( DEBUG, "AddObjectProcessing() called ...\n");
  
  m_Objects.push_back( m_ObjectToBeAdded );
  m_RenderWindowInteractor->Modified();
  
  m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = m_ObjectToBeAdded->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}

void View::AddAnnotation2DProcessing( )
{
  igstkLogMacro( DEBUG, "AddAnnotation2DProcessing called ...\n");
  
  const int * size = m_RenderWindowInteractor->GetSize();
  m_Annotation2DToBeAdded->RequestSetAnnotationsViewPort( size[0], size[1] );
  m_Annotation2DToBeAdded->RequestAddAnnotations( );
  Annotation2D::ActorsListType actors = m_Annotation2DToBeAdded->GetActors();
  Annotation2D::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}


/** Request for removing a spatial object from the View */
void View::RequestRemoveObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "RequestRemoveObject() called ...\n");

  m_ObjectToBeRemoved = pointer;
  
  m_IteratorToObjectToBeRemoved = m_Objects.end(); 
  
  if( !pointer )
    {
    igstkPushInputMacro( NullRemoveObject );
    m_StateMachine.ProcessInputs();
    return;
    }
  
  m_IteratorToObjectToBeRemoved =
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( m_IteratorToObjectToBeRemoved == m_Objects.end() )
    {
    igstkPushInputMacro( InexistingRemoveObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidRemoveObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Remove a spatial object from the View. This method can only be invoked by
 * the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved is valid. */
void View::RemoveObjectProcessing()
{
  igstkLogMacro( DEBUG, "RemoveObjectProcessing() called ...\n");

  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  m_RenderWindowInteractor->Modified();
  
  ObjectRepresentation::ActorsListType actors =
                                         m_ObjectToBeRemoved->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestRemoveActor(*actorIt);
    actorIt++;
    } 
}


/** Request to Start the Pulse Generator for periodically refreshing the View
 * */
void View::RequestStart()
{
  igstkLogMacro( DEBUG, "RequestStart() called ...\n");

  igstkPushInputMacro( StartRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to Stop the Pulse Generator for periodically refreshing the View
 * */
void View::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");

  igstkPushInputMacro( StopRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to save the screenshot of the current scene into a file.
 *  The fileformat must be PNG. */
void View::RequestSaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG, "RequestSaveScreenShot() called ...\n");

  std::string fileNameExtension =
        ::itksys::SystemTools::GetFilenameLastExtension( filename );

  if( fileNameExtension == ".png" )
    {
    m_ScreenShotFileName = filename;
    igstkPushInputMacro( ValidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( InvalidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
}


/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this class. */
void View::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}


/** Report that an invalid filename for saving the screen shot */
void View::ReportInvalidScreenShotFileNameProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidScreenShotFileNameProcessing() called ...\n");
}

/** Save current screenshot while in refreshing state */
void View::SaveScreenShotWhileRefreshingProcessing()
{
  igstkLogMacro( DEBUG, 
                 "SaveScreenShotWhileRefreshingProcessing() called ...\n");

  m_PulseGenerator->RequestStop();
  
  this->SaveScreenShot();

  m_PulseGenerator->RequestStart();
}

/** Save current screenshot in idle state*/
void View::SaveScreenShotWhileIdleProcessing()
{
  igstkLogMacro( DEBUG, "SaveScreenShotWhileIdleProcessing() called ...\n");
  
  this->SaveScreenShot();
}

/** Save current screenshot */
void View::SaveScreenShot()
{
  igstkLogMacro( DEBUG, "SaveScreenShot() called ...\n");

  vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

  vtkPNGWriter * writer = vtkPNGWriter::New();

  windowToImageFilter->SetInput( m_RenderWindow );

  windowToImageFilter->Update();

  writer->SetInput( windowToImageFilter->GetOutput() );
  
  writer->SetFileName( m_ScreenShotFileName.c_str() );
  
  m_RenderWindow->Render();
  
  writer->Write();

  writer->SetInput( NULL );
  windowToImageFilter->SetInput( NULL );

  windowToImageFilter->Delete();
  writer->Delete();
}


/** FLTK event handlers */
void View::flush(void)
{
  igstkLogMacro( DEBUG, "flush() called ...\n");
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  this->draw();
}

/** Draw function */
void View::draw(void)
{
  igstkLogMacro( DEBUG, "draw() called ...\n");

  if( m_RenderWindow != NULL )
    {
    // make sure the vtk part knows where and how large we are
    this->UpdateSize( this->w(), this->h() );

    // make sure the GL context exists and is current:
    // after a hide() and show() sequence e.g. there is no context yet
    // and the Render() will fail due to an invalid context.
    // see Fl_Gl_Window::show()
    this->make_current();

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
    // FLTK 1.x does not support HiView
    ((vtkCarbonRenderWindow *)m_RenderWindow)->SetRootWindow( fl_xid( this ) );
#else
    m_RenderWindow->SetWindowId( (void *)fl_xid( this ) );
#endif
#if !defined(WIN32) && !defined(__APPLE__)
    m_RenderWindow->SetDisplayId( fl_display );
#endif
    // get vtk to render to the Fl_Gl_Window
    m_RenderWindowInteractor->Render();
    }
}

/** Resize function */
void View::resize( int x, int y, int w, int h ) 
{
  igstkLogMacro( DEBUG, "resize() called ...\n");

  // make sure VTK knows about the new situation
  UpdateSize( w, h );
  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}


/** main FLTK event handler */
int View::handle( int event ) 
{
  igstkLogMacro( DEBUG, "handle() called ...\n");
  
  if( !m_RenderWindowInteractor->GetEnabled() || !m_InteractionHandling) 
    {
    return 0;
    }
  // SEI(x, y, ctrl, shift, keycode, repeatcount, keysym)
  m_RenderWindowInteractor->SetEventInformation(
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
      m_RenderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
      
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
          m_RenderWindowInteractor->InvokeEvent(
                                      vtkCommand::LeftButtonPressEvent,NULL);
          break;
        case FL_MIDDLE_MOUSE:
          m_RenderWindowInteractor->InvokeEvent(
                                    vtkCommand::MiddleButtonPressEvent,NULL);
          break;
        case FL_RIGHT_MOUSE:
          m_RenderWindowInteractor->InvokeEvent(
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
      m_RenderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
    break;

    case FL_RELEASE:    // mouse up
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          {
          m_RenderWindowInteractor->InvokeEvent(
                                     vtkCommand::LeftButtonReleaseEvent,NULL);
          
          m_PointPicker->Pick( Fl::event_x(), 
                               this->h()-Fl::event_y()-1, 
                               0, m_Renderer );
          double data[3];
          m_PointPicker->GetPickPosition( data );
          Transform::VectorType pickedPoint;
          pickedPoint[0] = data[0];
          pickedPoint[1] = data[1];
          pickedPoint[2] = data[2];
          
          double validityTime = itk::NumericTraits<double>::max();
          double errorValue = 1.0; // this should be obtained from 
                                   // the picked object.

          igstk::Transform transform;
          transform.SetTranslation( pickedPoint, errorValue, validityTime );

          igstk::TransformModifiedEvent transformEvent;
          transformEvent.Set( transform );

          m_Reporter->InvokeEvent( transformEvent );
          }
          break;
        case FL_MIDDLE_MOUSE:
          m_RenderWindowInteractor->InvokeEvent(
                                   vtkCommand::MiddleButtonReleaseEvent,NULL);
          break;
        case FL_RIGHT_MOUSE:
          m_RenderWindowInteractor->InvokeEvent(
                                    vtkCommand::RightButtonReleaseEvent,NULL);
          break;
        }
      break;

    default:    // let the base class handle everything else 
    return Fl_Gl_Window::handle( event );
    } // switch(event)...

  return 1; // we handled the event if we didn't return earlier
}


void
View
::SetInteractorStyle( vtkInteractorStyle * interactorStyle )
{
  m_RenderWindowInteractor->SetInteractorStyle( interactorStyle );
}


void 
View
::Print( std::ostream& os, ::itk::Indent indent ) const
{
  this->PrintSelf(os, indent);
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const View& o)
{
  o.Print(os);
  return os;
}


/** Print object information */
void View::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "RenderWindow Pointer: " << this->m_RenderWindow << std::endl;
  os << indent << "RenderWindowInteractor Pointer: " 
               << *(this->m_RenderWindowInteractor) << std::endl;
  os << indent << "Renderer Pointer: " << this->m_Renderer << std::endl;
  os << indent << "Camera Pointer: " << this->m_Camera << std::endl;
  os << indent << "InteractionHandling: ";
  os << this->m_InteractionHandling << std::endl;

  if( this->m_PulseGenerator )
    {
    os << indent;
    this->m_PulseGenerator->Print(os);
    }

  if( this->m_PulseObserver )
    {
    os << indent;
    this->m_PulseObserver->Print(os);
    }

  ObjectListConstIterator itr;

  for( itr = this->m_Objects.begin(); itr != this->m_Objects.end(); ++itr )
    {
    os << indent << *itr << std::endl;
    }
}

} // end namespace igstk
