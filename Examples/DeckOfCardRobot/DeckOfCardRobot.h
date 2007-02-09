/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    DeckOfCardRobot.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DeckOfCardRobot_h
#define __DeckOfCardRobot_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "DeckOfCardRobotGUI.h"
#include "RobotCommunication.h"

#include "igstkStateMachine.h"
#include "itkStdStreamLogOutput.h"
#include "igstkFLTKTextLogOutput.h"
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObjectVolumeRepresentation.h"
#include "igstkObliqueImageSpatialObjectRepresentation.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkTubeObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAnnotation2D.h"
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"

#include "itkVersorRigid3DTransform.h"

namespace igstk
{

class DeckOfCardRobot : public DeckOfCardRobotGUI, public igstk::Object
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( DeckOfCardRobot, DeckOfCardRobotGUI );
  igstkNewMacro( Self );

  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput                       LogOutputType;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for ImageReaderType */
  typedef CTImageReader                                 ImageReaderType;

  typedef CTImageSpatialObject                          ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType             IndexType;

  typedef ImageSpatialObjectType::ImageType             ITKImageType;

  /** typedef for ImageRepresentationType */
  typedef CTImageSpatialObjectRepresentation            ImageRepresentationType;
  typedef ImageSpatialObjectVolumeRepresentation< CTImageSpatialObject>
                                                       VolumeRepresentationType;
  typedef ObliqueImageSpatialObjectRepresentation< CTImageSpatialObject >
                                                      ObliqueRepresentationType;

  /** Public request methods from the GUI. */
  virtual void RequestLoadImage();
  virtual void RequestSetROI();
  virtual void RequestRegistration();
  virtual void RequestSetTargetPoint();
  virtual void RequestSetEntryPoint();  
  virtual void RequestConnectToRobot();
  virtual void RequestHomeRobot();
  virtual void RequestTargetingRobot();
  virtual void RequestResliceImage();
  virtual void RequestInsertNeedle();


  /** Methods for Converting Events into State Machine Inputs */
  igstkLoadedEventTransductionMacro( AxialSliceBoundsEvent,
                                     AxialBoundsInput, AxialBounds    );
  igstkLoadedEventTransductionMacro( SagittalSliceBoundsEvent, 
                                     SagittalBoundsInput, SagittalBounds );
  igstkLoadedEventTransductionMacro( CoronalSliceBoundsEvent,  
                                     CoronalBoundsInput, CoronalBounds  );

  igstkObserverObjectMacro( ImageSpatialObject,
                    ImageReaderType::ImageModifiedEvent,ImageSpatialObjectType)
  igstkObserverConstObjectMacro( ITKImage,
                   ImageSpatialObjectType::ITKImageModifiedEvent, ITKImageType)

protected:

  DeckOfCardRobot();
  virtual ~DeckOfCardRobot();

private:

  DeckOfCardRobot(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );

  igstkDeclareStateMacro( WaitingForDICOMDirectory );
  igstkDeclareStateMacro( ImageReady );

  igstkDeclareStateMacro( AttemptingRegistration );
  igstkDeclareStateMacro( LandmarkRegistrationReady );

  igstkDeclareStateMacro( TargetPointReady );
  igstkDeclareStateMacro( PathReady );

  igstkDeclareStateMacro( AttemptingConnectToRobot );
  igstkDeclareStateMacro( RobotConnected );

  igstkDeclareStateMacro( AttemptingHomeRobot );
  igstkDeclareStateMacro( RobotHomed );

  igstkDeclareStateMacro( AttemptingTargetingRobot );
  igstkDeclareStateMacro( RobotReady );

  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( RequestLoadImage );
  igstkDeclareInputMacro( LoadImageSuccess );
  igstkDeclareInputMacro( LoadImageFailure );
  
  igstkDeclareInputMacro( RequestRegistration );
  igstkDeclareInputMacro( RegistrationSuccess );
  igstkDeclareInputMacro( RegistrationFailure );

  igstkDeclareInputMacro( RequestSetTargetPoint );
  igstkDeclareInputMacro( RequestSetEntryPoint  );

  igstkDeclareInputMacro( RequestConnectToRobot );
  igstkDeclareInputMacro( ConnectToRobotSuccess ); 
  igstkDeclareInputMacro( ConnectToRobotFailure );

  igstkDeclareInputMacro( RequestHomeRobot );
  igstkDeclareInputMacro( HomeRobotSuccess );
  igstkDeclareInputMacro( HomeRobotFailure );

  igstkDeclareInputMacro( RequestTargetingRobot );
  igstkDeclareInputMacro( TargetingRobotSuccess );
  igstkDeclareInputMacro( TargetingRobotFailure );

  igstkDeclareInputMacro( AxialBounds );
  igstkDeclareInputMacro( SagittalBounds );
  igstkDeclareInputMacro( CoronalBounds );


  /** Logger */
  LogOutputType::Pointer              m_LogFileOutput;  // log output to file
  LogOutputType::Pointer              m_LogCoutOutput;  // log output to console
  std::ofstream                       m_LogFile;        // file stream
  FLTKTextLogOutput::Pointer          m_LogFLTKOutput;  // log output to FLTK

  /** igstkLoggerMacro create a m_Logger,igstkLogMacro(x,y) will write 
   *  to m_Logger.
   *  This logger is for the logging of internal igstk components, 
   *  eg. m_ImageReader->SetLogger( logger ).
   * You can also use igstkLogMacro( logger, x, y ) to log your 
   * message in to this logger. */
  LoggerType::Pointer                 m_Logger; // Another logger for igstk

  /** DICOM image reader */
  ImageReaderType::Pointer            m_ImageReader;

  /** Robot */
  RobotCommunication *                m_Robot;
  
  /** Pointer to the CTImageSpatialObject */
  ImageSpatialObjectType::Pointer     m_ImageSpatialObject;

  /** Slice representations of the image in View2D and View3D */
  ImageRepresentationType::Pointer    m_ImageRepresentationAxial;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal;
  
  VolumeRepresentationType::Pointer   m_ImageRepresentation3D;
  ObliqueRepresentationType::Pointer  m_ImageRepresentationOblique;

  
  /** To store the landmark registration result transform*/
  itk::VersorRigid3DTransform< double >::Pointer    m_ImageToRobotTransform;  
  itk::VersorRigid3DTransform< double >::Pointer    m_RobotToImageTransform;  
  
  /** To store the transform of the image and tracker landmark points */
  Transform                                    m_ImageLandmarkTransformToBeSet;
  Transform                                    m_RobotTransform;
  Transform                                    m_RobotCurrentTransform;
  Transform                                    m_RobotTransformToBeSet;
  Transform                                    m_NeedleTransformToBeSet;

  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > ObserverType;
  ObserverType::Pointer               m_ViewPickerObserver;

  /** Ellipsoid spatial object, used to represent 
   *  the landmark point, tip of the probe. */
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  EllipsoidType::Pointer                          m_NeedleTip;
  EllipsoidRepresentationType::Pointer            m_NeedleTipRepresentation;
  EllipsoidType::Pointer                          m_PickedPoint;
  EllipsoidRepresentationType::Pointer            m_PickedPointRepresentation;

  /** Objects for path planning */
  EllipsoidType::Pointer                          m_TargetPoint;
  EllipsoidRepresentationType::Pointer            m_TargetRepresentation;
  Transform                                       m_TargetTransform;

  EllipsoidType::Pointer                          m_EntryPoint;
  EllipsoidRepresentationType::Pointer            m_EntryRepresentation;  
  Transform                                       m_EntryTransform;
  
  typedef igstk::TubeObject                       PathType;
  typedef igstk::TubeObjectRepresentation         PathRepresentationType;
  typedef TubeObject::PointType                   TubePointType;
  PathType::Pointer                               m_Path;
  PathRepresentationType::Pointer                 m_PathRepresentationAxial;
  PathRepresentationType::Pointer                 m_PathRepresentationSagittal;
  PathRepresentationType::Pointer                 m_PathRepresentationCoronal;
  PathRepresentationType::Pointer                 m_PathRepresentation3D;


  /** Cylinder spatial object, used to represent the probe */
  typedef igstk::CylinderObject                   CylinderType;
  typedef igstk::CylinderObjectRepresentation     CylinderRepresentationType;  
  CylinderType::Pointer                            m_Needle;
  CylinderRepresentationType::Pointer              m_NeedleRepresentation;
  CylinderType::Pointer                            m_NeedleHolder;
  CylinderRepresentationType::Pointer              m_NeedleHolderRepresentation;

  /** Box spatial object, used to represent the movement region */
  typedef igstk::BoxObject                        BoxType;
  typedef igstk::BoxObjectRepresentation          BoxRepresentationType;
  BoxType::Pointer                                m_Box;
  BoxRepresentationType::Pointer                  m_BoxRepresentation;

  Annotation2D::Pointer                            m_Annotation2D;
  ITKImageType::IndexType                          m_P1;
  ITKImageType::IndexType                          m_P2;
  float                                            m_Translation[3];
  float                                            m_Rotation[3];
  bool                                             m_Reachable;

  /** Action methods to be invoked only by the state machine */
  void NoProcessing();

  void LoadImageProcessing();

  void ConnectImageRepresentationProcessing();
  void SetAxialSliderBoundsProcessing();
  void SetSagittalSliderBoundsProcessing();
  void SetCoronalSliderBoundsProcessing();

  void SetRegionProcessing();

  void RegistrationProcessing();

  void DrawTargetPointProcessing();
  void DrawPathProcessing();

  void ConnectToRobotProcessing();
  void HomeRobotProcessing();
  void TargetingRobotProcessing();

  /** Callback functions for picking  events. */
  void DrawPickedPoint( const itk::EventObject & event );

  /** Methods for reslicing the image */
  void ResliceImage();
  void ResliceImage( IndexType index );
  bool CalculateRobotMovement();
  void CreateObliqueView();
  void DisableObliqueView();
  void AnimateRobotMove( Transform TCurrent, Transform TToBeSet, int steps);

};

} // end of namespace

#endif
