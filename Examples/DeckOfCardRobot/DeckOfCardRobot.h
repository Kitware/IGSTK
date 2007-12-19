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

#include "igstkView2D.h"
#include "igstkView3D.h"

class DeckOfCardRobot : public DeckOfCardRobotGUI
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( DeckOfCardRobot, DeckOfCardRobotGUI );
  
  igstkLoggerMacro();

  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput                       LogOutputType;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for ImageReaderType */
  typedef igstk::CTImageReader                         ImageReaderType;
  typedef igstk::CTImageSpatialObject                  ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType            IndexType;

  typedef ImageSpatialObjectType::ImageType             ITKImageType;

  /** typedef for ImageRepresentationType */
  typedef igstk::CTImageSpatialObjectRepresentation     ImageRepresentationType;
  typedef igstk::ImageSpatialObjectVolumeRepresentation
    < igstk::CTImageSpatialObject>                    VolumeRepresentationType;
  typedef igstk::ObliqueImageSpatialObjectRepresentation
    < igstk::CTImageSpatialObject >                  ObliqueRepresentationType;

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
  igstkLoadedEventTransductionMacro( AxialSliceBounds,
                                     AxialBounds );
  igstkLoadedEventTransductionMacro( SagittalSliceBounds, 
                                     SagittalBounds );
  igstkLoadedEventTransductionMacro( CoronalSliceBounds,
                                     CoronalBounds );

  igstkObserverObjectMacro( ImageSpatialObject,
                    ImageReaderType::ImageModifiedEvent,ImageSpatialObjectType)
  igstkObserverConstObjectMacro( ITKImage,
                   ImageSpatialObjectType::ITKImageModifiedEvent, ITKImageType)

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
  igstk::FLTKTextLogOutput::Pointer   m_LogFLTKOutput;  // log output to FLTK

  /** DICOM image reader */
  ImageReaderType::Pointer            m_ImageReader;

  /** Robot */
  igstk::RobotCommunication *                m_Robot;
  
  /** Pointer to the CTImageSpatialObject */
  ImageSpatialObjectType::Pointer     m_ImageSpatialObject;

  /** Slice representations of the image in View2D and View3D */
  ImageRepresentationType::Pointer    m_ImageRepresentationAxial;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal;
  
  VolumeRepresentationType::Pointer   m_ImageRepresentation3D;
  ObliqueRepresentationType::Pointer  m_ImageRepresentationOblique;

  
  /** To store the landmark registration result transform */
  itk::VersorRigid3DTransform< double >::Pointer    m_ImageToRobotTransform;  
  itk::VersorRigid3DTransform< double >::Pointer    m_RobotToImageTransform;  
  
  /** To store the transform of the image and tracker landmark points */
  igstk::Transform                              m_ImageLandmarkTransformToBeSet;
  igstk::Transform                              m_RobotTransform;
  igstk::Transform                              m_RobotCurrentTransform;
  igstk::Transform                              m_RobotTransformToBeSet;
  igstk::Transform                              m_NeedleTransformToBeSet;

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
  igstk::Transform                                m_TargetTransform;

  EllipsoidType::Pointer                          m_EntryPoint;
  EllipsoidRepresentationType::Pointer            m_EntryRepresentation;  
  igstk::Transform                                m_EntryTransform;
  
  typedef igstk::TubeObject                       PathType;
  typedef igstk::TubeObjectRepresentation         PathRepresentationType;
  typedef igstk::TubeObject::PointType            TubePointType;
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

  igstk::Annotation2D::Pointer                     m_Annotation2D;
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
  void AnimateRobotMove( igstk::Transform TCurrent, 
                                         igstk::Transform TToBeSet, int steps);


  typedef igstk::View2D ViewType2D;
  typedef igstk::View3D ViewType3D;

  ViewType2D::Pointer DisplayAxial;
  ViewType2D::Pointer DisplayCoronal;
  ViewType2D::Pointer DisplaySagittal;
  ViewType3D::Pointer Display3D;
  ViewType2D::Pointer DisplayOblique;

};

#endif
