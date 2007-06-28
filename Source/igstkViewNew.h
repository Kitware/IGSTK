/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkViewNew_h
#define __igstkViewNew_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// VTK headers
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkIndent.h"
#include "vtkWorldPointPicker.h"
#include "vtkCommand.h"
#include "vtkInteractorStyle.h"

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkPulseGenerator.h"
#include "igstkObjectRepresentation.h"   
#include "igstkEvents.h"   
#include "igstkAnnotation2D.h"   
#include "igstkRenderWindowInteractor.h"   

namespace igstk {

/** \class ViewNew
 * 
 *
 * \ingroup Object
 */
class ViewNew : public Object 
{

public:

  /** Macro with standard traits declarations. */    
   igstkStandardClassTraitsMacro( ViewNew, Object );
 
  /** Set the desired frequency for refreshing the view. It is not worth to
   * attempt to go faster than your monitor, nor more than double than your
   * trackers */
  void RequestSetRefreshRate( double frequency );
  
  /** Add an observer to this ViewNew class */
  unsigned long AddObserver( const ::itk::EventObject & event, 
                                                   ::itk::Command * observer );

  /** Remove an observer to this ViewNew class */
  void  RemoveObserver( unsigned long tag );
  
  /** Object representation types */
  typedef ObjectRepresentation::Pointer     ObjectPointer;
  typedef std::list< ObjectPointer >        ObjectListType; 
  typedef ObjectListType::iterator          ObjectListIterator;
  typedef ObjectListType::const_iterator    ObjectListConstIterator;

  /** Add an object representation to the list of children and associate it
   * with a specific view. */ 
  void RequestAddObject( ObjectRepresentation* object ); 

  /** Add annotation to the view */
  void RequestAddAnnotation2D( Annotation2D *  annotation ); 

  /** Remove the object passed as arguments from the list of children, only if
   * it is associated to a particular view. */ 
  void RequestRemoveObject( ObjectRepresentation* object ); 

  /** Request to save a screen shot into a file. The file format MUST be PNG
   * in order to have lossless compression. This method will trigger an extra
   * rendering of the scene in order to ensure that the image is fresh.
   * */
  void RequestSaveScreenShot( const std::string & filename );

  /** Print the object information in a stream. */
  void Print( std::ostream& os, ::itk::Indent indent=0) const;

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Initialize the render window interactor */
  void Initialize();

  /** Enable the render window interactor */
  void Enable();

  /** Enable the render window interactor */
  void Render();

  /** Get render window */
  vtkRenderWindow * GetRenderWindow()
    {
    return this->m_RenderWindow; 
    } 

  /** Get render window interactor */
  RenderWindowInteractor *  GetRenderWindowInteractor()
    {
    return this->m_RenderWindowInteractor; 
    }

protected:

  ViewNew( );
  virtual ~ViewNew( void );

public:
  
  /** Update the display in order to render the new content of the scene */
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

  /** Change the window size */
  void UpdateSize(int x, int y);

protected:
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
 
  /** Default Camera */
  vtkCamera             * m_Camera;
 
  /** Set the interactor style in the derived classes */
  void SetInteractorStyle( vtkInteractorStyle * style );

private:
  
  vtkRenderWindow       * m_RenderWindow;
  vtkRenderer           * m_Renderer;

  typedef vtkWorldPointPicker  PickerType;
  PickerType            * m_PointPicker;
  
  bool                    m_InteractionHandling;

  /** Render Window Interactor */
  RenderWindowInteractor  * m_RenderWindowInteractor;

  /** Member variables for holding temptative arguments of functions.
   *  This is needed for implementing a layer of security that decouples
   *  user invokations from the actual state of this class */
  vtkProp            * m_ActorToBeAdded;
  vtkProp            * m_ActorToBeRemoved;
  Annotation2D       * m_Annotation2DToBeAdded; 
  
  typedef itk::SimpleMemberCommand< Self >   ObserverType;

  PulseGenerator::Pointer   m_PulseGenerator;
  ObserverType::Pointer     m_PulseObserver;
  ::itk::Object::Pointer    m_Reporter;

  /** List of the children object plug to the spatial object. */
  ObjectListType m_Objects; 

private:

  /** Methods that will only be invoked by the State Machine */

  /** Add and remove vtk Actors. Intended to be called only by the state
   * machine */
  void AddActorProcessing();
  void RemoveActorProcessing();

  /** Add annotation */
  void AddAnnotation2DProcessing(  );

  /** Add and remove RepresentationObject classes */
  void AddObjectProcessing();
  void RemoveObjectProcessing();

  /** Method that will refresh the view.. and the GUI */
  void RefreshRender();

  void RequestAddActor( vtkProp * actor );
  void RequestRemoveActor( vtkProp * actor );
  
  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Save a screenshot of the current rendered scene while in idle state */
  void SaveScreenShotWhileIdleProcessing();
  
  /** Save a screenshot of the current rendered scene while in refreshing
   * state */
  void SaveScreenShotWhileRefreshingProcessing();

  /** Save a screenshot. This method is the actual implementation of the
   * process of saving the screenshot. This method is invoked from
   * SaveScreenShotWhileIdleProcessing and
   * SaveScreenShotWhileRefreshingProcessing */
  void SaveScreenShot();
  
  /** Reports when a filename for the screen shot is not valid */
  void ReportInvalidScreenShotFileNameProcessing();

  /** This should be called by the state machine */
  void StartProcessing();
  void StopProcessing();
  
  /** Reset the settings of the camera */  
  void ResetCameraProcessing();
  
  /** Disable keyboard and mouse interactions */
  void DisableInteractionsProcessing();

  /** Enable keyboard and mouse interactions */
  void EnableInteractionsProcessing();

private:
  
  // Arguments for methods to be invoked by the state machine.
  //
  ObjectRepresentation::Pointer m_ObjectToBeAdded;
  ObjectRepresentation::Pointer m_ObjectToBeRemoved;
  ObjectListType::iterator      m_IteratorToObjectToBeRemoved;
  std::string                   m_ScreenShotFileName;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidAddActor );
  igstkDeclareInputMacro( NullAddActor );
  igstkDeclareInputMacro( ValidRemoveActor );
  igstkDeclareInputMacro( NullRemoveActor );
  igstkDeclareInputMacro( ValidAddObject );
  igstkDeclareInputMacro( NullAddObject );
  igstkDeclareInputMacro( ValidAddAnnotation2D );
  igstkDeclareInputMacro( NullAddAnnotation2D );
  igstkDeclareInputMacro( ExistingAddObject );
  igstkDeclareInputMacro( ValidRemoveObject );
  igstkDeclareInputMacro( InexistingRemoveObject );
  igstkDeclareInputMacro( NullRemoveObject );
  igstkDeclareInputMacro( ResetCamera );
  igstkDeclareInputMacro( EnableInteractions );
  igstkDeclareInputMacro( DisableInteractions );
  igstkDeclareInputMacro( StartRefreshing );
  igstkDeclareInputMacro( StopRefreshing );
  igstkDeclareInputMacro( ValidScreenShotFileName );
  igstkDeclareInputMacro( InvalidScreenShotFileName );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( Refreshing );

};

std::ostream& operator<<(std::ostream& os, const ViewNew& o);

} // end namespace igstk

#endif
