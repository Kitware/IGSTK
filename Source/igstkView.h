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
#endif

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <vtkRenderWindowInteractor.h>
#include <igstkScene.h>

// VTK headers
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

#include "igstkMacros.h"
#include "igstkStateMachine.h"

namespace igstk{

class View : public Fl_Gl_Window, public vtkRenderWindowInteractor 
{

private:
    
  typedef igstk::StateMachine< View > StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer ActionType;
  typedef StateMachineType::StateType              StateType;
  typedef StateMachineType::InputType              InputType;
  typedef StateMachineType::StateIdentifierType    StateIdentifierType;

  FriendClassMacro( StateMachineType );

protected:

  View( int x, int y, int w, int h, const char *l="");
  ~View( void );

public:
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

  /** Set the scene */
  void SetScene(const Scene * scene);

  /** void Update the display */
  void Update();

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
    
  /** Get the vtk Camera */
  vtkCamera* GetCamera() {return m_Camera;}

private:
  
  vtkRenderWindow    * m_RenderWindow;
  vtkRenderer        * m_Renderer;
  vtkCamera          * m_Camera;
  Scene::ConstPointer m_Scene;
};

} // end namespace igstk

#endif
