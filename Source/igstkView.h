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
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// VTK headers
#include <vtkRenderWindowInteractor.h>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkPulseGenerator.h"
#include "igstkObjectRepresentation.h"   

namespace igstk{

class Scene;

/** \class View
 * 
 * \brief Base class for all View2D and View3D classes that display scenes in a
 * VTK controlled window attached to the GUI.
 *
 * \ingroup Object
 */

class View : public Fl_Gl_Window, public vtkRenderWindowInteractor 
{

public:
    
  typedef View      Self;

  igstkFriendClassMacro( Scene );

  igstkTypeMacro( View, vtkRenderWindowInteractor );
  
  /** Set the desired frequency for refreshing the view. It is not worth to
   * attempt to go faster than your monitor, nor more than double than your
   * trackers */
  void RequestSetRefreshRate( double frequency );
  
  /** Add an observer to this View class */
  void AddObserver( const ::itk::EventObject & event, ::itk::Command * observer );
  
  /** Object representation types */
  typedef ObjectRepresentation::Pointer     ObjectPointer;
  typedef std::list< ObjectPointer >        ObjectListType; 
  typedef ObjectListType::iterator          ObjectListIterator;
  typedef ObjectListType::const_iterator    ObjectListConstIterator;

  /** Add an object representation to the list of children and associate it
   * with a specific view. */ 
  void RequestAddObject( ObjectRepresentation* object ); 

  /** Remove the object passed as arguments from the list of children, only if
   * it is associated to a particular view. */ 
  void RequestRemoveObject( ObjectRepresentation* object ); 

  /** Logger class */
  typedef itk::Logger                  LoggerType;

  /** The SetLogger method is used to attach a logger to this object for
   * debugging and retrospective analysis purposes. */
  void SetLogger( LoggerType * logger );

   /** Declarations needed for the State Machine */
  igstkStateMachineMacro();
 
protected:

  View( int x, int y, int w, int h, const char *l="");
  virtual ~View( void );

  // vtkRenderWindowInteractor overrides made protected in order to prevent
  // users from using these methods.
  void Initialize();
  void Enable();
  void Disable();
  void SetRenderWindow(vtkRenderWindow *aren);
  void UpdateSize(int x, int y);
  int CreateTimer(int timertype);
  int DestroyTimer();
  void OnTimer(void);
  void ResetCamera();
  void DisableInteractions();
  void EnableInteractions();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


  static void OnTimerGlobal(void *p);

public:
  
  /** Update the display in order to render the new content of the scene*/
  void Update();
 
  /** Disable user interactions with the window via mouse and keyboard */
  void RequestDisableInteractions();

  /** Enable user interactions with the window via mouse and keyboard */
  void RequestEnableInteractions();

  /** Request to return the camera to a known position */
  void RequestResetCamera();
  
  /** Request Start the periodic refreshing of the view */
  void RequestStart();

  /** Request Stopping the periodic refreshing of the view */
  void RequestStop();

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

  /** Member variables for holding temptative arguments of functions.
   *  This is needed for implementing a layer of security that decouples
   *  user invokations from the actual state of this class */
  vtkProp3D            * m_ActorToBeAdded;
  vtkProp3D            * m_ActorToBeRemoved;
  
  typedef itk::SimpleMemberCommand< Self >   ObserverType;

  PulseGenerator::Pointer   m_PulseGenerator;
  ObserverType::Pointer     m_PulseObserver;
  ::itk::Object::Pointer    m_Reporter;

  /** List of the children object plug to the Scene spatial object. */
  ObjectListType m_Objects; 

private:

  /** Methods that will only be invoked by the State Machine */

  /** Add and remove vtk Actors. Intended to be called only by the state
   * machine */
  void AddActor();
  void RemoveActor();

  /** Add and remove RepresentationObject classes */
  void AddObject();
  void RemoveObject();

  /** Method that will refresh the view.. and the GUI */
  void RefreshRender();

  void RequestAddActor( vtkProp3D * actor );
  void RequestRemoveActor( vtkProp3D * actor );
  
  /** Report any invalid request to the logger */
  void ReportInvalidRequest();

  /** This should be called by the state machine */
  void Start();
  void Stop();
  
private:
  
  // Arguments for methods to be invoked by the state machine.
  //
  ObjectRepresentation::Pointer m_ObjectToBeAdded;
  ObjectRepresentation::Pointer m_ObjectToBeRemoved;
  ObjectListType::iterator      m_IteratorToObjectToBeRemoved;

  /** Inputs to the State Machine */
  InputType            m_ValidAddActor;
  InputType            m_NullAddActor;
  InputType            m_ValidRemoveActor;
  InputType            m_NullRemoveActor;
  InputType            m_ValidAddObject;
  InputType            m_NullAddObject;
  InputType            m_ExistingAddObject;
  InputType            m_ValidRemoveObject;
  InputType            m_InexistingRemoveObject;
  InputType            m_NullRemoveObject;
  InputType            m_ResetCameraInput;
  InputType            m_EnableInteractionsInput;
  InputType            m_DisableInteractionsInput;
  InputType            m_StartRefreshingInput;
  InputType            m_StopRefreshingInput;

  /** States for the State Machine */
  StateType            m_IdleState;
  StateType            m_RefreshingState;

  /** The Logger instance */
  mutable LoggerType::Pointer      m_Logger;

  /** Get pointer to the Logger */
  LoggerType * GetLogger() const;

};

} // end namespace igstk

#endif
