/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FourViewsTrackingWithCT.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __FourViewsTrackingWithCT_h
#define __FourViewsTrackingWithCT_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "FourViewsTrackingWithCTGUI.h"

#include "igstkStateMachine.h"
#include "itkStdStreamLogOutput.h"
#include "igstkFLTKTextLogOutput.h"

#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"

#include "igstkLandmark3DRegistration.h"

//#include "igstkSystemInformation.h"
#include "igstkSandboxConfigure.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkPolarisTracker.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"

#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"

#define TRACKER_TOOL_PORT 3
#define TRACKER_TOOL_SROM_FILE "C:/Patrick/Vicra/Tookit/Tool\
                                Definition Files/8700340.rom"

#include "igstkPivotCalibration.h"

namespace igstk
{

class FourViewsTrackingWithCT 
                     : public FourViewsTrackingWithCTGUI, public igstk::Object
{
public:
  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( FourViewsTrackingWithCT, 
                                      FourViewsTrackingWithCTGUI );
  igstkNewMacro( Self );

  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput        LogOutputType;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for ImageReaderType */
  typedef CTImageReader                                 ImageReaderType;

  typedef ImageReaderType::ImageSpatialObjectType       ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType             IndexType;

  /** typedef for ImageRepresentationType */
  typedef CTImageSpatialObjectRepresentation            ImageRepresentationType;

  /** typedef for RegistrationType */
  typedef Landmark3DRegistration                        RegistrationType;
  typedef RegistrationType::LandmarkPointContainerType  
                                                     LandmarkPointContainerType;
  typedef RegistrationType::LandmarkImagePointType      LandmarkPointType;
  
  /** typedef for TransformType */
  typedef RegistrationType::TransformType               TransformType;

  /** typedefs for the communication */
#ifdef WIN32
  typedef SerialCommunicationForWindows               CommunicationType;
#else
  typedef SerialCommunicationForPosix                 CommunicationType;
#endif

  /** typedefs for the tracker */
  typedef PolarisTracker                                TrackerType;

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
  
  virtual void RequestReset();         // Do we need to offer this method?
  virtual void RequestResliceImage();  // Do we need state machine for this one?


  /** Methods for Converting Events into State Machine Inputs */
  igstkLoadedEventTransductionMacro( AxialSliceBoundsEvent,
                                     AxialBoundsInput, AxialBounds    );
  igstkLoadedEventTransductionMacro( SagittalSliceBoundsEvent, 
                                     SagittalBoundsInput, SagittalBounds );
  igstkLoadedEventTransductionMacro( CoronalSliceBoundsEvent,  
                                     CoronalBoundsInput, CoronalBounds  );

protected:

  FourViewsTrackingWithCT();
  virtual ~FourViewsTrackingWithCT();

private:

  FourViewsTrackingWithCT(const Self&); // purposely not implemented
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
  igstkDeclareStateMacro( InitializingTracker );
  igstkDeclareStateMacro( TrackerReady );
  igstkDeclareStateMacro( AddingTrackerLandmark );
  igstkDeclareStateMacro( TrackerLandmarksReady );
  igstkDeclareStateMacro( LandmarkRegistrationReady );
  igstkDeclareStateMacro( Tracking );

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
  FLTKTextLogOutput::Pointer          m_LogFLTKOutput;  // log output to FLTK

  /** igstkLoggerMacro create a m_Logger,igstkLogMacro(x,y) will write 
   *  to m_Logger.
   *  This logger is for the logging of internal igstk components, 
   *  eg. m_ImageReader->SetLogger( logger ).
   *  You can also use igstkLogMacro( logger, x, y ) to log your message 
   *  in to this logger. */
  LoggerType::Pointer                 logger; // Another logger

  /** Registered patient name */
  std::string                         m_PatientName;
  
  /** DICOM image reader */
  ImageReaderType::Pointer            m_ImageReader;

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

  /** To store the landmark registration result transform*/
  Transform                           m_ImageToTrackerTransform;  
  
  /** To store the transform of the image and tracker landmark points */
  Transform                           m_TrackerLandmarkTransform; 
  Transform                           m_ImageLandmarkTransform;
  Transform                           m_TrackerLandmarkTransformToBeSet; 
  Transform                           m_ImageLandmarkTransformToBeSet;  

  /** Serial communication and tracker */
  CommunicationType::Pointer          m_SerialCommunication;
  TrackerType::Pointer                m_Tracker;

  /** A pulse generator which can generate PulseEvent. */
  PulseGenerator::Pointer             m_PulseGenerator;

  /** Observer type for simple event, the callback can be set 
   *  to a member function. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer               m_Observer;

  /** Observer type for loaded event, the callback can be set 
   *  to a member function. */
  typedef itk::ReceptorMemberCommand < Self > ObserverType2;
  ObserverType2::Pointer               m_LandmarkRegistrationObserver;
  ObserverType2::Pointer               m_ViewPickerObserver;

  /** Ellipsoid spatial object, used to represent 
   *  the landmark point, tip of the probe. */
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  EllipsoidType::Pointer                          m_Ellipsoid;
  EllipsoidRepresentationType::Pointer            m_EllipsoidRepresentation;

  /** Cylinder spatial object, used to represent the probe */
  typedef igstk::CylinderObject                   CylinderType;
  typedef igstk::CylinderObjectRepresentation     CylinderRepresentationType;  
  CylinderType::Pointer                            m_Cylinder;
  CylinderRepresentationType::Pointer              m_CylinderRepresentation;


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
  
  /** Callback functions for picking and registration success events. */
  void GetLandmarkRegistrationTransform( const itk::EventObject & event);
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

};

} // end of namespace

#endif
