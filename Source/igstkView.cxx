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
#include "igstkView.h"
#include <FL/x.H>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyle.h>
#include <vtkVersion.h>
#include <vtkCommand.h>
#include <igstkEvents.h>

namespace igstk{

/** Constructor */
View::View( int x, int y, int w, int h, const char *l ) : 
Fl_Gl_Window( x, y, w, h, l ), vtkRenderWindowInteractor()
{  
  // Create a default render window
  m_RenderWindow = vtkRenderWindow::New();
  m_Renderer = vtkRenderer::New();
  m_RenderWindow->AddRenderer(m_Renderer);
  m_Camera = m_Renderer->GetActiveCamera();
  m_RenderWindow->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);
  this->Initialize();
  m_InteractionHandling = true;
  this->end();

  // Preparing the State Machine 
  m_StateMachine.SetOwnerClass( this );

  m_StateMachine.AddInput( m_ValidAddActor,  "ValidAddActor" );
  m_StateMachine.AddInput( m_NullAddActor,   "NullAddActor"  );

  m_StateMachine.AddState( m_IdleState,      "IdleState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_IdleState, m_ValidAddActor, m_IdleState,  & View::AddActor );
  m_StateMachine.AddTransition( m_IdleState, m_NullAddActor,  m_IdleState,          NoAction );

  m_StateMachine.SelectInitialState( m_IdleState );

  m_StateMachine.SetReadyToRun();

  m_SceneAddObjectObserver =  ObserverType::New();
  m_SceneAddObjectObserver->SetCallbackFunction(this, & View::UpdateViewFromAddedObject);
  m_SceneRemoveObjectObserver =  ObserverType::New();
  m_SceneRemoveObjectObserver->SetCallbackFunction(this, & View::UpdateViewFromRemovedObject);

  m_Scene = 0;

}

/** Destructor */
View::~View()
{
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    {
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
    }
  m_RenderWindow->Delete();
  m_Renderer->Delete();
}

/** */
void View::Initialize()
{
  this->SetRenderWindow(m_RenderWindow);
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
  this->redraw();
}

/** Callback function, if the scene has been modified, i.e. an object has been added */
void View::UpdateViewFromAddedObject()
{
  if(!m_Scene)
    {
    return;
    }

  ObjectRepresentation* object = m_Scene->GetLastAddedObject();

  if(!object)
    {
    return;
    }

  object->CreateActors();

  ObjectRepresentation::ActorsListType actors = object->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    }

}

/** Callback function, if the scene has been modified, i.e. an object has been removed */
void View::UpdateViewFromRemovedObject()
{
  if(!m_Scene)
    {
    return;
    }

  ObjectRepresentation* object = m_Scene->GetLastRemovedObject();

  if(!object)
    {
    return;
    }

  // First we need to remove the actor from the renderer
  ObjectRepresentation::ActorsListType actors = object->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    // FIXME: This needs to use the state machine concept
    m_Renderer->RemoveActor( *actorIt );
    actorIt++;
    }

  // Second we delete the actors created by the object
  object->DeleteActors();

}

/** Set the scene */
void View::SetScene(igstk::Scene* scene)
{
  m_Scene = scene;
  m_Scene->AddObserver( SceneAddObjectEvent(),   m_SceneAddObjectObserver);
  m_Scene->AddObserver( SceneRemoveObjectEvent(),   m_SceneRemoveObjectObserver);

  Scene::ObjectListType objects = m_Scene->GetObjects();
  Scene::ObjectListConstIterator it        = objects.begin();
  Scene::ObjectListConstIterator objectEnd = objects.end();

  while( it != objectEnd )
    {
    (*it)->CreateActors();

    ObjectRepresentation::ActorsListType actors = (*it)->GetActors();
    ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
    while(actorIt != actors.end())
      {
      this->RequestAddActor(*actorIt);
      actorIt++;
      }
    it++;
    }
}


/** */
void View::RequestAddActor( vtkProp3D * actor )
{
  m_NewActor = actor;
  if( !actor )
    {
    m_StateMachine.ProcessInput( m_NullAddActor );
    }
  else
    {
    m_StateMachine.ProcessInput( m_ValidAddActor );
    }
}


/** */
void View::AddActor()
{
  m_Renderer->AddActor( m_NewActor );
}


/** */
void View::EnableInteractions()
{
  m_InteractionHandling = true;
}

/** */
void View::DisableInteractions()
{
  m_InteractionHandling = false;
}



/** */
void View::ResetCamera()
{
  m_Renderer->ResetCamera();
}

/** */
void View::Enable()
{
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
  // the interactor cannot control the event loop
  vtkErrorMacro(<<"View::Start() interactor cannot control event loop.");
}

/** */
void View::SetRenderWindow(vtkRenderWindow *aren)
{
  vtkRenderWindowInteractor::SetRenderWindow(aren);
  // if a View has been shown already, and one
  // re-sets the RenderWindow, neither UpdateSize nor draw is called,
  // so we have to force the dimensions of the NEW RenderWindow to match
  // the our (vtkFlRWI) dimensions
  if (RenderWindow)
    {
    RenderWindow->SetSize(this->w(), this->h());
    }
}

/** this gets called during FLTK window draw()s and resize()s */
void View::UpdateSize(int W, int H)
{
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
/** FLTK needs global timer callbacks, but we set it up so that this global
 *  callback knows which instance OnTimer() to call */
void OnTimerGlobal(void *p)
{
  if (p)
    {
    ((View *)p)->OnTimer();
    }
}

/** */
int View::CreateTimer(int timertype)
{
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
  // do nothing
  return 1;
}

/** */
void View::OnTimer(void)
{
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
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  draw();
}

/** Draw function */
void View::draw(void)
{
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
  // make sure VTK knows about the new situation
  UpdateSize( w, h );
  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}


/** main FLTK event handler */
int View::handle( int event ) 
{
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
          this->InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL);
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

/** */
static char const rcsid[] =
  "Id";

const char *View_rcsid(void)
{
    return rcsid;
}

} // end namespace igstk
