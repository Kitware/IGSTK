/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQView.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkQView_h
#define __igstkQView_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// QT dared to define macro called DEBUG!!
#define QT_NO_DEBUG 1

// VTK headers
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
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkPulseGenerator.h"
#include "igstkObjectRepresentation.h"   
#include "igstkEvents.h"   
#include "igstkAnnotation2D.h"   

#include "QVTKWidget.h"

namespace igstk {

/** \class QView
 * 
 * \brief Base class for all QView2D and QView3D classes that display scenes in
 * a VTK controlled window attached to the Qt GUI.
 *
 * \ingroup Object
 */
class QView : public QVTKWidget
{

public:
    
  typedef QView          Self;
  typedef QVTKWidget     Superclass;

  igstkTypeMacro( QView, QVTKWidget );
  
  /** Set the desired frequency for refreshing the view. It is not worth to
   * attempt to go faster than your monitor, nor more than double than your
   * trackers */
  void RequestSetRefreshRate( double frequency );
  
  /** Add an observer to this View class */
  void AddObserver( const ::itk::EventObject & event, 
                    ::itk::Command * observer );
  
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

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

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

  /** Constructor */
#if QT_VERSION < 0x040000
    //! constructor for Qt 3
    QView(QWidget* parent = NULL, const char* name = NULL, Qt::WFlags f = 0);
#else
    //! constructor for Qt 4
    QView(QWidget* parent = NULL, Qt::WFlags f = 0);
#endif

  /** Destructor */
  virtual ~QView( void );
  
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
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
 
  /** Default Camera */
  vtkCamera             * m_Camera;
 
  /** Set the interactor style in the derived classes */
  void SetInteractorStyle( vtkInteractorStyle * style );

private:
  
  vtkRenderer           * m_Renderer;

  typedef vtkWorldPointPicker  PickerType;
  PickerType            * m_PointPicker;
  
  bool                    m_InteractionHandling;

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
   * state*/
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

std::ostream& operator<<(std::ostream& os, const QView& o);

} // end namespace igstk

#endif
