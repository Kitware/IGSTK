/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _igstkView_h
#define _igstkView_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <vtkRenderWindowInteractor.h>

// VTK headers
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkScene.h"

namespace igstk{

class View : public Fl_Gl_Window, public vtkRenderWindowInteractor 
{

private:
    
  typedef igstk::StateMachine< View >                StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType;
  typedef StateMachineType::StateType                StateType;
  typedef StateMachineType::InputType                InputType;

  FriendClassMacro( StateMachineType );

protected:

  View( int x, int y, int w, int h, const char *l="");
  ~View( void );

public:
  
  typedef View                               Self;
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  
  // vtkRenderWindowInteractor overrides
  void Initialize();
  void Enable();
  void Disable();
  void Start();
  void SetRenderWindow(vtkRenderWindow *aren);
  void UpdateSize(int x, int y);
  int CreateTimer(int timertype);
  int DestroyTimer();
  void OnTimer(void);
  void ResetCamera();

  /** void Update the display */
  void Update();
 
  /** Disable the interactions */
  void DisableInteractions();
  void EnableInteractions();

  /** Add a vtk Actor */
  void SetScene(igstk::Scene* scene);

protected:
  
  // Fl_Gl_Window overrides
  void flush(void);
  void draw( void );
  void resize( int x, int y, int w, int h );
  virtual int  handle( int event );
   
  /** Get the vtk RenderWindow */
  vtkRenderWindow* GetRenderWindow() {return m_RenderWindow;}
    
  /** Get the vtk Renderer */
  vtkRenderer* GetRenderer() {return m_Renderer;}
    
 
private:
  
  vtkRenderWindow       * m_RenderWindow;
  vtkRenderer           * m_Renderer;
  vtkCamera             * m_Camera;
  bool                    m_InteractionHandling;
  igstk::Scene::Pointer   m_Scene;          

  /** Member variables for holding temptative arguments of functions.
   *  This is needed for implementing a layer of security that decouples
   *  user invokations from the actual state of this class */
  vtkProp3D            * m_NewActor;
  
  ObserverType::Pointer     m_SceneAddObjectObserver;
  ObserverType::Pointer     m_SceneRemoveObjectObserver;

private:

  /** Methods that will only be invoked by the State Machine */

  /** Add a vtk Actor */
  void AddActor();

  void RequestAddActor( vtkProp3D * actor );
  void UpdateViewFromAddedObject();
  void UpdateViewFromRemovedObject();


private:

  StateMachineType     m_StateMachine;
  
  /** Inputs to the State Machine */
  InputType            m_ValidAddActor;
  InputType            m_NullAddActor;
  

  /** States for the State Machine */
  StateType            m_IdleState;

};

} // end namespace igstk

#endif
