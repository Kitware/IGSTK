/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkView_h
#define __igstkView_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// VTK declarations
class vtkRenderWindow;
class vtkCamera;
class vtkProp;
class vtkInteractorStyle;
class vtkRenderer;
class vtkWorldPointPicker;


// ITK headers
#include "itkCommand.h"

// IGSTK headers
#include "igstkLogger.h"
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkPulseGenerator.h"
#include "igstkObjectRepresentation.h"   
#include "igstkEvents.h"   
#include "igstkAnnotation2D.h"   
#include "igstkRenderWindowInteractor.h"   

#include "igstkCoordinateSystemInterfaceMacros.h"

namespace igstk {

/** \class View
 *  
 *  \brief Display graphical representations of surgical scenes.
 * 
 *  The functionality of the View class is to aggregate all the graphical
 *  representations of spatial objects into one scene. igstk::View2D and
 *  igstk::View3D classes are derived from this class for 2D and 3D viewing
 *  capabilities. The view class encapsulate VTK classes into a restrictive
 *  API subjected to control of a state machine.
 *
 * \image html igstkView.png  "State Machine Diagram"
 * \image latex igstkView.eps "State Machine Diagram" 
 *
 * \sa View2D
 * \sa View3D
 *
 * \ingroup Object
 * \ingroup View
 */
class View : public Object 
{

public:

  /** Macro with standard traits declarations. */
   igstkStandardClassTraitsMacro( View, Object );
 
  /** Set the desired frequency for refreshing the view. It is not worth to
   * attempt to go faster than your monitor, nor more than double than your
   * trackers */
  void SetRefreshRate( double frequency );
 
  /** Add an object representation to the list of children and associate it
   * with a specific view. */ 
  void RequestAddObject( ObjectRepresentation* object ); 

  /** Add annotation to the view */
  void RequestAddAnnotation2D( Annotation2D::Pointer  annotation ); 

  /** Remove the object passed as arguments from the list of children, only 
   * if it is associated to a particular view. */ 
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

  /** Request to return the camera to a known position */
  void RequestResetCamera();
  
  /** Request Start the periodic refreshing of the view */
  void RequestStart();

  /** Request Stopping the periodic refreshing of the view */
  void RequestStop();

  /** Set Camera position */
  void SetCameraPosition( double x, double y, double z);
  
  /** Set camera focal point */
  void SetCameraFocalPoint( double x, double y, double z);

  /** Set Camera View Up vector */
  void SetCameraViewUp( double vx, double vy, double vz);

  /** Set Clipping range */
  void SetCameraClippingRange( double dNear, double dFar );

  /** Turn on/off parallel projection */
  void SetCameraParallelProjection( bool flag );

  /** Set renderer background color */
  void SetRendererBackgroundColor(
                    double red, double green, double blue );

  /** Set camera zoom factor */
  void SetCameraZoomFactor( double rate );

  friend class ViewProxyBase;

protected:

  View( );
  virtual ~View( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;
 
  /** Set the interactor style in the derived classes */
  void SetInteractorStyle( vtkInteractorStyle * style );

  /** Set the size of the render window */
  void RequestSetRenderWindowSize( int width, int height );
 
  /** Request initialize the RenderWindow interactor */
  void RequestInitializeRenderWindowInteractor();

  /** Sets the coordinates of the point picked */
  void SetPickedPointCoordinates( double xPosition, double yPosition );

private:

  /** Get renderer */ 
  vtkRenderer *  GetRenderer() const;
 
  /** Get render window */
  vtkRenderWindow * GetRenderWindow() const;

  /** Get render window interactor */
  RenderWindowInteractor *  GetRenderWindowInteractor() const;

  /** Initialize the interactor */
  void InitializeRenderWindowInteractorProcessing();

  /** Add and remove vtk Actors. Intended to be called only by the state
   * machine */
  void AddActorProcessing();
  void RemoveActorProcessing();

  /** Add annotation */
  void AddAnnotation2DProcessing();

  /** Add and remove RepresentationObject classes */
  void AddObjectProcessing();
  void RemoveObjectProcessing();

  /** Method that will refresh the view.. and the GUI */
  void RefreshRender();

  /** Request add actor */
  void RequestAddActor( vtkProp * actor );

  /** Request remove actor */
  void RequestRemoveActor( vtkProp * actor );
  
  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Save a screenshot of the current rendered scene while in Idle state */
  void SaveScreenShotWhileIdleProcessing();
  
  /** Save a screenshot of the current rendered scene while
   *  in InteractorInitialized state */
  void SaveScreenShotWhileInteractorInitializedProcessing();
  
  /** Save a screenshot of the current rendered scene while in Refreshing
   * state */
  void SaveScreenShotWhileRefreshingProcessing();

  /** Save a screenshot. This method is the actual implementation of the
   * process of saving the screenshot. This method is invoked from
   * SaveScreenShotWhileIdleProcessing and
   * SaveScreenShotWhileRefreshingProcessing */
  void SaveScreenShot();
  
  /** Reports when a filename for the screen shot is not valid */
  void ReportInvalidScreenShotFileNameProcessing();

  /** Set render window size */
  void SetRenderWindowSizeProcessing(); 

  /** Report invalid render window size. */
  void ReportInvalidRenderWindowSizeProcessing();
 
  /** This should be called by the state machine */
  void StartProcessing();
  void StopProcessing();
  
  /** Reset the settings of the camera */  
  void ResetCameraProcessing();
  
private:
 
  vtkRenderWindow       * m_RenderWindow;
  vtkRenderer           * m_Renderer;
  vtkCamera             * m_Camera;
  vtkWorldPointPicker   * m_PointPicker;

  /** Render Window Interactor */
  RenderWindowInteractor  * m_RenderWindowInteractor;

  /** Member variables for holding temptative arguments of functions.
   *  This is needed for implementing a layer of security that decouples
   *  user invokations from the actual state of this class */
  vtkProp            * m_ActorToBeAdded;
  vtkProp            * m_ActorToBeRemoved;

  Annotation2D::Pointer       m_Annotation2DToBeAdded; 
  
  typedef itk::SimpleMemberCommand< Self >   ObserverType;

  PulseGenerator::Pointer   m_PulseGenerator;
  ObserverType::Pointer     m_PulseObserver;

  /** Object representation types */
  typedef ObjectRepresentation::Pointer     ObjectPointer;
  typedef std::list< ObjectPointer >        ObjectListType; 
  typedef ObjectListType::const_iterator    ObjectListConstIterator;

  /** List of the children object plug to the spatial object. */
  ObjectListType m_Objects; 
 
  // Arguments for methods to be invoked by the state machine.
  ObjectRepresentation::Pointer m_ObjectToBeAdded;
  ObjectRepresentation::Pointer m_ObjectToBeRemoved;
  ObjectListType::iterator      m_IteratorToObjectToBeRemoved;
  std::string                   m_ScreenShotFileName;
  int                           m_RenderWindowWidthToBeSet;
  int                           m_RenderWindowHeightToBeSet;
 
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
  igstkDeclareInputMacro( StartRefreshing );
  igstkDeclareInputMacro( StopRefreshing );
  igstkDeclareInputMacro( ValidScreenShotFileName );
  igstkDeclareInputMacro( InvalidScreenShotFileName );
  igstkDeclareInputMacro( ValidRenderWindowSize );
  igstkDeclareInputMacro( InValidRenderWindowSize );
  igstkDeclareInputMacro( InitializeInteractor );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( InteractorInitialized );
  igstkDeclareStateMacro( Refreshing );

  View(const View& ); // purposely not implemented
  View& operator=(const View& ); // purposely not implemented

  /** Define the coordinate system interface*/
  igstkCoordinateSystemClassInterfaceMacro();

  /** Additional coordinate system used for defining 
   *  picking transforms with respect to the View's 
   *  coordinate system. */
  CoordinateSystem::Pointer  m_PickerCoordinateSystem;
};

std::ostream& operator<<(std::ostream& os, const View& o);

} // end namespace igstk

#endif
