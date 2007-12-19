/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    NeedleBiopsy.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __NeedleBiopsy_h
#define __NeedleBiopsy_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "NeedleBiopsyGUI.h"

#include "igstkStateMachine.h"
#include "itkStdStreamLogOutput.h"
#include "igstkFLTKTextLogOutput.h"
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkLandmark3DRegistration.h"

#include "igstkSandboxConfigure.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkPolarisTracker.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkTubeObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAnnotation2D.h"

#include "igstkView2D.h"
#include "igstkView3D.h"

class NeedleBiopsy : public NeedleBiopsyGUI
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( NeedleBiopsy, NeedleBiopsyGUI );
  
  igstkLoggerMacro();

  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput        LogOutputType;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for ImageReaderType */
  typedef igstk::CTImageReader                          ImageReaderType;
  typedef ImageReaderType::ImageSpatialObjectType       ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType             IndexType;

  /** typedef for ImageRepresentationType */
  typedef igstk::CTImageSpatialObjectRepresentation     ImageRepresentationType;

  /** typedef for RegistrationType */
  typedef igstk::Landmark3DRegistration                 RegistrationType;
  typedef RegistrationType::LandmarkImagePointType      LandmarkPointType;

  typedef RegistrationType::LandmarkPointContainerType  
                                                    LandmarkPointContainerType;
  
  /** typedef for TransformType */
  typedef RegistrationType::TransformType               TransformType;

  /** typedefs for the communication */
#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows               CommunicationType;
#else
  typedef igstk::SerialCommunicationForPosix                 CommunicationType;
#endif

  /** typedefs for the tracker */
  typedef igstk::PolarisTracker                                TrackerType;

  /** Public request methods from the GUI. */
  virtual void RequestSetPatientName();
  virtual void RequestLoadImage();
  virtual void RequestInitializeTracker();
  virtual void RequestAddImageLandmark();
  virtual void RequestAddTrackerLandmark();
  virtual void RequestClearImageLandmarks();
  virtual void RequestClearTrackerLandmarks();
  virtual void RequestRegistration();
  virtual void RequestStartTracking();
  virtual void RequestStopTracking();

  virtual void RequestSetTargetPoint();
  virtual void RequestSetEntryPoint();
  
  virtual void RequestReset();         // Do we need to offer this method?
  virtual void RequestResliceImage();  // Do we need state machine for this one?


  /** Methods for Converting Events into State Machine Inputs */

  igstkLoadedEventTransductionMacro( AxialSliceBounds,
                                     AxialBounds );
  igstkLoadedEventTransductionMacro( SagittalSliceBounds,
                                     SagittalBounds );
  igstkLoadedEventTransductionMacro( CoronalSliceBounds,
                                     CoronalBounds );
  
  igstkObserverObjectMacro(CTImage,igstk::CTImageReader::ImageModifiedEvent,
                                                  igstk::CTImageSpatialObject)

  NeedleBiopsy();
  virtual ~NeedleBiopsy();

private:

  NeedleBiopsy(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( WaitingForPatientName );
  igstkDeclareStateMacro( PatientNameReady );
  igstkDeclareStateMacro( WaitingForDICOMDirectory );
  igstkDeclareStateMacro( ImageReady );
  igstkDeclareStateMacro( PatientNameVerified );
  igstkDeclareStateMacro( AddingImageLandmark );
  igstkDeclareStateMacro( ImageLandmarksReady );
  igstkDeclareStateMacro( AttemptingInitializeTracker );
  igstkDeclareStateMacro( TrackerReady );
  igstkDeclareStateMacro( AddingTrackerLandmark );
  igstkDeclareStateMacro( TrackerLandmarksReady );
  igstkDeclareStateMacro( AttemptingRegistration );
  igstkDeclareStateMacro( EvaluatingRegistrationError );
  igstkDeclareStateMacro( LandmarkRegistrationReady );
  igstkDeclareStateMacro( TargetPointReady );
  igstkDeclareStateMacro( PathReady );
  igstkDeclareStateMacro( AttemptingStartTracking );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( AttemptingStopTracking );

  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( RequestSetPatientName );
  igstkDeclareInputMacro( PatientName );
  igstkDeclareInputMacro( PatientNameEmpty );

  igstkDeclareInputMacro( RequestLoadImage );
  igstkDeclareInputMacro( LoadImageSuccess );
  igstkDeclareInputMacro( LoadImageFailure );
  igstkDeclareInputMacro( PatientNameMatch );
  igstkDeclareInputMacro( OverwritePatientName );
  igstkDeclareInputMacro( ReloadImage );
  
  igstkDeclareInputMacro( RequestAddImageLandmark );
  igstkDeclareInputMacro( NeedMoreLandmarkPoints );
  igstkDeclareInputMacro( EnoughLandmarkPoints );
  igstkDeclareInputMacro( RequestClearImageLandmarks );

  igstkDeclareInputMacro( RequestInitializeTracker );
  igstkDeclareInputMacro( InitializeTrackerSuccess );
  igstkDeclareInputMacro( InitializeTrackerFailure );

  igstkDeclareInputMacro( RequestAddTrackerLandmark );
  igstkDeclareInputMacro( RequestClearTrackerLandmarks );

  igstkDeclareInputMacro( RequestRegistration );
  igstkDeclareInputMacro( RegistrationSuccess );
  igstkDeclareInputMacro( RegistrationFailure );
  igstkDeclareInputMacro( RegistrationErrorAccepted );
  igstkDeclareInputMacro( RegistrationErrorRejected );

  igstkDeclareInputMacro( RequestSetTargetPoint );
  igstkDeclareInputMacro( RequestSetEntryPoint  );

  igstkDeclareInputMacro( RequestStartTracking );
  igstkDeclareInputMacro( StartTrackingSuccess ); 
  igstkDeclareInputMacro( StartTrackingFailure );

  igstkDeclareInputMacro( RequestStopTracking );
  igstkDeclareInputMacro( StopTrackingSuccess );
  igstkDeclareInputMacro( StopTrackingFailure );

  igstkDeclareInputMacro( AxialBounds );
  igstkDeclareInputMacro( SagittalBounds );
  igstkDeclareInputMacro( CoronalBounds );


  /** Logger */
  LogOutputType::Pointer              m_LogFileOutput;  // log output to file
  LogOutputType::Pointer              m_LogCoutOutput;  // log output to console
  std::ofstream                       m_LogFile;        // file stream
  igstk::FLTKTextLogOutput::Pointer   m_LogFLTKOutput;  // log output to FLTK

  /** Registered patient name */
  std::string                         m_PatientName;
  
  /** DICOM image reader */
  ImageReaderType::Pointer            m_ImageReader;
  
  /** Observer for the image reader */
  CTImageObserver::Pointer            m_CTImageObserver;

  /** Pointer to the CTImageSpatialObject */
  ImageSpatialObjectType::Pointer     m_ImageSpatialObject;

  /** Slice representations of the image in View2D and View3D */
  ImageRepresentationType::Pointer    m_ImageRepresentationAxial;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal;
  ImageRepresentationType::Pointer    m_ImageRepresentationAxial3D;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal3D;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal3D;

  /** Landmark registration and its landmark points container */
  RegistrationType::Pointer           m_LandmarkRegistration;
  LandmarkPointContainerType          m_ImageLandmarksContainer;
  LandmarkPointContainerType          m_TrackerLandmarksContainer;
  igstk::Transform::ErrorType         m_LandmarkRegistrationRMSError;

  /** To store the landmark registration result transform */
  igstk::Transform                           m_ImageToTrackerTransform;  
  
  /** To store the transform of the image and tracker landmark points */
  igstk::Transform                           m_TrackerLandmarkTransform; 
  igstk::Transform                           m_ImageLandmarkTransform;
  igstk::Transform                           m_TrackerLandmarkTransformToBeSet; 
  igstk::Transform                           m_ImageLandmarkTransformToBeSet;  

  /** Serial communication and tracker */
  CommunicationType::Pointer          m_SerialCommunication;
  TrackerType::Pointer                m_Tracker;

  /** A pulse generator which can generate PulseEvent. */
  igstk::PulseGenerator::Pointer             m_PulseGenerator;

  /** Observer type for simple event, 
   *  the callback can be set to a member function. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer               m_Observer;

  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > ObserverType2;
  ObserverType2::Pointer               m_LandmarkRegistrationObserver;
  ObserverType2::Pointer               m_LandmarkRegistrationRMSErrorObserver;
  ObserverType2::Pointer               m_ViewPickerObserver;

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

  igstk::Annotation2D::Pointer                            m_Annotation2D;

  /** Action methods to be invoked only by the state machine */
  void NoProcessing();
  void SetPatientNameProcessing();
  void LoadImageProcessing();
  void VerifyPatientNameProcessing(); 
  void ConnectImageRepresentationProcessing();
  void AddImageLandmarkProcessing();
  void ClearImageLandmarksProcessing();
  void InitializeTrackerProcessing();
  void AddTrackerLandmarkProcessing();
  void ClearTrackerLandmarksProcessing();  
  void RegistrationProcessing();
  void StartTrackingProcessing();
  void StopTrackingProcessing();
  void SetAxialSliderBoundsProcessing();
  void SetSagittalSliderBoundsProcessing();
  void SetCoronalSliderBoundsProcessing();

  void EvaluatingRegistrationErrorProcessing();
  void ResetRegistrationProcessing();
  
  void DrawTargetPointProcessing();
  void DrawPathProcessing();

  /** Callback functions for picking and registration success 
   and rms error events. */
  void GetLandmarkRegistrationTransform( const itk::EventObject & event);
  void GetLandmarkRegistrationRMSError( const itk::EventObject & event);
  void DrawPickedPoint( const itk::EventObject & event );

  /** Callback function for Events */
  void Tracking();
  
  /** Obtain the transform from the tracker */
  void GetTrackerTransform();


  /** Methods for reslicing the image */
  void ResliceImage();
  void ResliceImage( IndexType index );  

  /** Resetting the state of the application */
  void Reset();

  typedef igstk::View2D ViewType2D;
  typedef igstk::View3D ViewType3D;

  ViewType2D::Pointer DisplayAxial;
  ViewType2D::Pointer DisplayCoronal;
  ViewType2D::Pointer DisplaySagittal;
  ViewType3D::Pointer Display3D;

};

#endif
