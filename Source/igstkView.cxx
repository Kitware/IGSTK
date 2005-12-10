/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
#include <igstkEvents.h>


namespace igstk{

/** Constructor */
View::View( int x, int y, int w, int h, const char *l ) : 
Fl_Gl_Window( x, y, w, h, l ), vtkRenderWindowInteractor(),
  m_StateMachine(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  // Create a default render window
  m_RenderWindow = vtkRenderWindow::New();
  m_Renderer = vtkRenderer::New();
  m_PointPicker = PickerType::New();

  m_RenderWindow->AddRenderer( m_Renderer );
  m_Camera = m_Renderer->GetActiveCamera();
  m_RenderWindow->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);
  this->Initialize();
  m_InteractionHandling = true;
  this->end();

  igstkAddInputMacro( ValidAddObject );
  igstkAddInputMacro( NullAddObject  );
  igstkAddInputMacro( ExistingAddObject );
  igstkAddInputMacro( ValidRemoveObject );
  igstkAddInputMacro( InexistingRemoveObject );
  igstkAddInputMacro( NullRemoveObject  );
  igstkAddInputMacro( ValidAddActor );
  igstkAddInputMacro( NullAddActor  );
  igstkAddInputMacro( ValidRemoveActor );
  igstkAddInputMacro( NullRemoveActor  );
  igstkAddInputMacro( ResetCameraInput  );
  igstkAddInputMacro( EnableInteractionsInput  );
  igstkAddInputMacro( DisableInteractionsInput  );
  igstkAddInputMacro( StartRefreshingInput  );
  igstkAddInputMacro( StopRefreshingInput  );

  igstkAddStateMacro( IdleState       );
  igstkAddStateMacro( RefreshingState );


  igstkAddTransitionMacro( IdleState, ValidAddObject, IdleState,  AddObject );
  igstkAddTransitionMacro( IdleState, NullAddObject,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ExistingAddObject,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ValidRemoveObject, IdleState,  RemoveObject );
  igstkAddTransitionMacro( IdleState, NullRemoveObject,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, InexistingRemoveObject,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ValidAddActor, IdleState,  AddActor );
  igstkAddTransitionMacro( IdleState, NullAddActor,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ValidRemoveActor, IdleState,  RemoveActor );
  igstkAddTransitionMacro( IdleState, NullRemoveActor,  IdleState,  ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ResetCameraInput,  IdleState,  ResetCamera );
  igstkAddTransitionMacro( IdleState, EnableInteractionsInput,  IdleState,  EnableInteractions );
  igstkAddTransitionMacro( IdleState, DisableInteractionsInput,  IdleState,  DisableInteractions );
  igstkAddTransitionMacro( IdleState, StartRefreshingInput,  RefreshingState,  Start );
  igstkAddTransitionMacro( IdleState, StopRefreshingInput,  IdleState,  ReportInvalidRequest );

  igstkAddTransitionMacro( RefreshingState, ValidAddObject, RefreshingState,  AddObject );
  igstkAddTransitionMacro( RefreshingState, NullAddObject,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, ExistingAddObject,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, ValidRemoveObject, RefreshingState,  RemoveObject );
  igstkAddTransitionMacro( RefreshingState, NullRemoveObject,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, InexistingRemoveObject,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, ValidAddActor, RefreshingState,  AddActor );
  igstkAddTransitionMacro( RefreshingState, NullAddActor,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, ValidRemoveActor, RefreshingState,  RemoveActor );
  igstkAddTransitionMacro( RefreshingState, NullRemoveActor,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, ResetCameraInput,  RefreshingState,  ResetCamera );
  igstkAddTransitionMacro( RefreshingState, EnableInteractionsInput,  RefreshingState,  EnableInteractions );
  igstkAddTransitionMacro( RefreshingState, DisableInteractionsInput,  RefreshingState,  DisableInteractions );
  igstkAddTransitionMacro( RefreshingState, StartRefreshingInput,  RefreshingState,  ReportInvalidRequest );
  igstkAddTransitionMacro( RefreshingState, StopRefreshingInput,  IdleState,  Stop );


  m_StateMachine.SelectInitialState( m_IdleState );

  m_StateMachine.SetReadyToRun();

  m_ActorToBeAdded = 0;
  m_ActorToBeRemoved = 0;

  m_PulseGenerator = PulseGenerator::New();
  m_Reporter = ::itk::Object::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & View::RefreshRender );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  this->RequestSetRefreshRate( 30 ); // 30 Hz is rather low frequency for video.
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

  vtkRenderWindowInteractor::SetPicker( NULL );

  m_RenderWindow->Delete();

  m_Renderer->Delete();

  m_PointPicker->Delete();

  // This must be invoked to prevent Memory Leaks because we call
  // SetRenderWindow() in the Initialize() method.
  this->SetRenderWindow(NULL); 
}

/** */
void View::Initialize()
{
  igstkLogMacro( DEBUG, "Initialize() called ...\n");

  this->SetRenderWindow( m_RenderWindow );
  // if don't have render window then we can't do anything yet
  if (!RenderWindow)
    {
    vtkErrorMacro(<< "View::Initialize has no render window");
    return;
    }

  int *size = RenderWindow->GetSize();
  // enable everything and start rendering
  this->Enable();
    
  // set the size in the render window interactor
  Size[0] = size[0];
  Size[1] = size[1];

  // this is initialized
  Initialized = 1;
  m_Renderer->ResetCamera();

}



/** Update the display */
void View::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  this->redraw();
}


void View::AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer )
{
  igstkLogMacro( DEBUG, "AddObserver() called ...\n");
  m_Reporter->AddObserver( event, observer );
}


/** */
void View::RequestAddActor( vtkProp3D * actor )
{
  igstkLogMacro( DEBUG, "RequestAddActor() called ...\n");
  m_ActorToBeAdded = actor;
  if( !actor )
    {
    m_StateMachine.PushInput( m_NullAddActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidAddActor );
    m_StateMachine.ProcessInputs();
    }
}


/** */
void View::AddActor()
{
  igstkLogMacro( DEBUG, "AddActor() called ...\n");
  m_Renderer->AddActor( m_ActorToBeAdded );
  m_PointPicker->AddPickList( m_ActorToBeAdded );
}


/** */
void View::RequestRemoveActor( vtkProp3D * actor )
{
  igstkLogMacro( DEBUG, "RequestRemoveActor() called ...\n");
  m_ActorToBeRemoved = actor;
  if( !actor )
    {
    m_StateMachine.PushInput( m_NullRemoveActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidRemoveActor );
    m_StateMachine.ProcessInputs();
    }
}


/** */
void View::RemoveActor()
{
  igstkLogMacro( DEBUG, "RemoveActor() called ...\n");
  m_Renderer->RemoveActor( m_ActorToBeRemoved );
}


/** */
void View::RequestEnableInteractions()
{
  igstkLogMacro( DEBUG, "RequestEnableInteractions() called ...\n");
  m_StateMachine.PushInput( m_EnableInteractionsInput );
  m_StateMachine.ProcessInputs();
}


/** */
void View::RequestDisableInteractions()
{
  igstkLogMacro( DEBUG, "RequestDisableInteractions() called ...\n");
  m_StateMachine.PushInput( m_DisableInteractionsInput );
  m_StateMachine.ProcessInputs();
}


/** */
void View::EnableInteractions()
{
  igstkLogMacro( DEBUG, "EnableInteractions() called ...\n");
  m_InteractionHandling = true;
}

/** */
void View::DisableInteractions()
{
  igstkLogMacro( DEBUG, "DisableInteractions() called ...\n");
  m_InteractionHandling = false;
}



/** */
void View::RequestResetCamera()
{
  igstkLogMacro( DEBUG, "RequestResetCamera() called ...\n");
  m_StateMachine.PushInput( m_ResetCameraInput );
  m_StateMachine.ProcessInputs();
}


/** */
void View::ResetCamera()
{
  igstkLogMacro( DEBUG, "ResetCamera() called ...\n");
  m_Renderer->ResetCamera();
}

/** */
void View::Enable()
{
  igstkLogMacro( DEBUG, "Enable() called ...\n");
  // if already enabled then done
  if (Enabled)
    {
    return;
    }
  // that's it
  Enabled = 1;
  m_Renderer->ResetCamera();
  this->Modified();
}

/** */
void View::Disable()
{
  igstkLogMacro( DEBUG, "Disable() called ...\n");
  // if already disabled then done
  if (!Enabled)
    {
    return;
    }
  
  // that's it (we can't remove the event handler like it should be...)
  Enabled = 0;
  this->Modified();
}

/** */
void View::Start()
{
  igstkLogMacro( DEBUG, "Start() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStart();
}

/** */
void View::Stop()
{
  igstkLogMacro( DEBUG, "Stop() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStop();
}


/** */
void View::SetRenderWindow(vtkRenderWindow *aren)
{
  igstkLogMacro( DEBUG, "SetRenderWindow() called ...\n");
  vtkRenderWindowInteractor::SetRenderWindow(aren);
  // if a View has been shown already, and one
  // re-sets the RenderWindow, neither UpdateSize nor draw is called,
  // so we have to force the dimensions of the NEW RenderWindow to match
  // the our (vtkFlRWI) dimensions
  if ( RenderWindow )
    {
    RenderWindow->SetSize(this->w(), this->h());
    vtkRenderWindowInteractor::SetPicker( m_PointPicker );
    }
}

/** this gets called during FLTK window draw()s and resize()s */
void View::UpdateSize(int W, int H)
{
  igstkLogMacro( DEBUG, "UpdateSize() called ...\n");
  if (RenderWindow != NULL)
    {
    // if the size changed tell render window
    if ( (W != Size[0]) || (H != Size[1]) )
      {
      // adjust our (vtkRenderWindowInteractor size)
      Size[0] = W;
      Size[1] = H;
      // and our RenderWindow's size
      RenderWindow->SetSize(W, H);
     
      // FLTK can move widgets on resize; if that happened, make
      // sure the RenderWindow position agrees with that of the
      // Fl_Gl_Window
      int *pos = RenderWindow->GetPosition();
      if( pos[0] != x() || pos[1] != y() ) 
        {
        RenderWindow->SetPosition( x(), y() );
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

  // First, compute the time at which we estimate that the scene will be rendered
  TimeStamp renderTime;
  double frequency = m_PulseGenerator->GetFrequency();
  renderTime.SetStartTimeNowAndExpireAfter( 1.0 / frequency ); // milliseconds

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
    m_StateMachine.PushInput( m_NullAddObject );
    m_StateMachine.ProcessInputs();
    return;
    }

  ObjectListType::iterator it =    
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( it != m_Objects.end() )
    {
    m_StateMachine.PushInput( m_ExistingAddObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidAddObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Add an object to the View. This method should only be called by the state
 * machine. The state machine makes sure that this method is called with a valid
 * value in the ObjectToBeAdded. */
void View::AddObject()
{
  igstkLogMacro( DEBUG, "AddObject() called ...\n");
  
  m_Objects.push_back( m_ObjectToBeAdded );
  this->Modified();
  
  m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = m_ObjectToBeAdded->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
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
    m_StateMachine.PushInput( m_NullRemoveObject );
    m_StateMachine.ProcessInputs();
    return;
    }
  
  m_IteratorToObjectToBeRemoved =
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( m_IteratorToObjectToBeRemoved == m_Objects.end() )
    {
    m_StateMachine.PushInput( m_InexistingRemoveObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidRemoveObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Remove a spatial object from the View. This method can only be invoked by
 * the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved is valid. */
void View::RemoveObject()
{
  igstkLogMacro( DEBUG, "RemoveObject() called ...\n");

  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  this->Modified();
  
  ObjectRepresentation::ActorsListType actors = m_ObjectToBeRemoved->GetActors();
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

  m_StateMachine.PushInput( m_StartRefreshingInput );
  m_StateMachine.ProcessInputs();
}


/** Request to Stop the Pulse Generator for periodically refreshing the View
 * */
void View::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");

  m_StateMachine.PushInput( m_StopRefreshingInput );
  m_StateMachine.ProcessInputs();
}


/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this class. */
void View::ReportInvalidRequest()
{
  igstkLogMacro( WARNING, "ReportInvalidRequest() called ...\n");
}


/** FLTK needs global timer callbacks, but we set it up so that this global
 *  callback knows which instance OnTimer() to call */
void View::OnTimerGlobal(void *p)
{
  if (p)
    {
    ((View *)p)->OnTimer();
    }
}

/** */
int View::CreateTimer(int timertype)
{
  igstkLogMacro( DEBUG, "CreateTimer() called ...\n");
  
  // to be called every 10 milliseconds, one shot timer
  // we pass "this" so that the correct OnTimer instance will be called
  if (timertype == VTKI_TIMER_FIRST)
    {
    Fl::add_timeout(0.01, OnTimerGlobal, (void *)this);
    }
  else
    {
    Fl::repeat_timeout(0.01, OnTimerGlobal, (void *)this);
    }
  return 1;
  // Fl::repeat_timer() is more correct, it doesn't measure the timeout
  // from now, but from when the system call that caused this timeout
  // elapsed.
}

/** */
int View::DestroyTimer()
{
  igstkLogMacro( DEBUG, "DestroyTimer() called ...\n");

  // do nothing
  return 1;
}

/** */
void View::OnTimer(void)
{
  igstkLogMacro( DEBUG, "OnTimer() called ...\n");

  if (!Enabled)
    {
    return;
    }
  // this is all we need to do, InteractorStyle is stateful and will
  // continue with whatever it's busy
  this->InvokeEvent(vtkCommand::TimerEvent, NULL);    
}

/** FLTK event handlers */
void View::flush(void)
{
  igstkLogMacro( DEBUG, "flush() called ...\n");
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  draw();
}

/** Draw function */
void View::draw(void)
{
  igstkLogMacro( DEBUG, "draw() called ...\n");

  if (RenderWindow!=NULL)
    {
    // make sure the vtk part knows where and how large we are
    UpdateSize( this->w(), this->h() );

    // make sure the GL context exists and is current:
    // after a hide() and show() sequence e.g. there is no context yet
    // and the Render() will fail due to an invalid context.
    // see Fl_Gl_Window::show()
    make_current();

    RenderWindow->SetWindowId( (void *)fl_xid( this ) );
#if !defined(WIN32) && !defined(__APPLE__)
    RenderWindow->SetDisplayId( fl_display );
#endif
    // get vtk to render to the Fl_Gl_Window
    Render();
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
  
  if( !Enabled || !m_InteractionHandling) 
    {
    return 0;
    }
  // SEI(x, y, ctrl, shift, keycode, repeatcount, keysym)
  this->SetEventInformation(Fl::event_x(), this->h()-Fl::event_y()-1, 
                            Fl::event_state( FL_CTRL ), Fl::event_state( FL_SHIFT ),
                            Fl::event_key(), 1, NULL);   
    
  switch( event ) 
    {
    case FL_FOCUS:
    case FL_UNFOCUS:
    ;   // Return 1 if you want keyboard events, 0 otherwise. Yes we do
    break;
      
    case FL_KEYBOARD:   // keypress
      this->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);        
      
      // Disabling VTK keyboard interaction
      //this->InvokeEvent(vtkCommand::KeyPressEvent, NULL);
      //this->InvokeEvent(vtkCommand::CharEvent, NULL);
     
      // now for possible controversy: there is no way to find out if the InteractorStyle actually did
      // something with this event.  To play it safe (and have working hotkeys), we return "0", which indicates
      // to FLTK that we did NOTHING with this event.  FLTK will send this keyboard event to other children
      // in our group, meaning it should reach any FLTK keyboard callbacks (including hotkeys)
      return 0;
    break;
      
    case FL_PUSH: // mouse down
    this->take_focus();  // this allows key events to work
    switch( Fl::event_button() ) 
      {
      case FL_LEFT_MOUSE:
            this->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
        break;
      case FL_MIDDLE_MOUSE:
            this->InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
        break;
      case FL_RIGHT_MOUSE:
            this->InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
            break;
      }
    break; // this break should be here, at least according to vtkXRenderWindowInteractor

    // we test for both of these, as fltk classifies mouse moves as with or
    // without button press whereas vtk wants all mouse movement (this bug took
    // a while to find :)
    case FL_DRAG:
    case FL_MOVE:
      this->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
    break;

    case FL_RELEASE:    // mouse up
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          {
          this->InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL);
          
          m_PointPicker->Pick( Fl::event_x(), 
                               this->h()-Fl::event_y()-1, 
                               0, m_Renderer );
          double data[3];
          m_PointPicker->GetPickPosition( data );
          Transform::VectorType pickedPoint;
          pickedPoint[0] = data[0];
          pickedPoint[1] = data[1];
          pickedPoint[2] = data[2];
          
          double validityTime = 100000.0; // 100 seconds
          double errorValue = 1.0; // this should be obtained from the picked object.

          igstk::Transform transform;
          transform.SetTranslation( pickedPoint, errorValue, validityTime );

          igstk::TransformModifiedEvent transformEvent;
          transformEvent.Set( transform );

          m_Reporter->InvokeEvent( transformEvent );
          }
        break;
        case FL_MIDDLE_MOUSE:
          this->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent,NULL);
        break;
        case FL_RIGHT_MOUSE:
          this->InvokeEvent(vtkCommand::RightButtonReleaseEvent,NULL);
        break;
        }
     break;

    default:    // let the base class handle everything else 
    return Fl_Gl_Window::handle( event );
    } // switch(event)...

  return 1; // we handled the event if we didn't return earlier
}


void 
View::SetLogger( LoggerType * logger )
{
  m_Logger = logger;
  m_PulseGenerator->SetLogger( logger );
}


View::LoggerType* 
View::GetLogger() const
{
  return m_Logger;
}


void 
View
::Print(std::ostream& os)
{
  os << "  " << "Transform" << " (" << this << ")\n";
  itk::Indent indent;
  this->PrintSelf(os, indent);
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, View& o)
{
  o.Print(os);
  return os;
}


/** Print object information */
void View::PrintSelf( std::ostream& os, itk::Indent indent )
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "RenderWindow Pointer: " << this->m_RenderWindow << std::endl;
  os << indent << "Renderer Pointer: " << this->m_Renderer << std::endl;
  os << indent << "Camera Pointer: " << this->m_Camera << std::endl;
  os << indent << "InteractionHandling: " << this->m_InteractionHandling << std::endl;

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



/** */
static char const rcsid[] =
  "Id";

const char *View_rcsid(void)
{
    return rcsid;
}


} // end namespace igstk
