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

#ifndef _FourViewsTrackingWithCT_h
#define _FourViewsTrackingWithCT_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
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
#define TRACKER_TOOL_SROM_FILE "C:/Patrick/Vicra/Tookit/Tool Definition Files/8700340.rom"

namespace igstk
{


class FourViewsTrackingWithCT : public FourViewsTrackingWithCTGUI, public itk::Object
{
public:
  /** Typedefs */
  typedef FourViewsTrackingWithCT        Self;
  typedef FourViewsTrackingWithCTGUI     Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  igstkTypeMacro( FourViewsTrackingWithCT, FourViewsTrackingWithCTGUI );
  igstkNewMacro( Self );

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;

  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput        LogOutputType;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Declarations needed for the Logging the application */
  igstkLoggerMacro();

  /** typedef for ImageReaderType */
  typedef CTImageReader                                 ImageReaderType;

  typedef ImageReaderType::ImageSpatialObjectType       ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType             IndexType;

  /** typedef for ImageRepresentationType */
  typedef CTImageSpatialObjectRepresentation            ImageRepresentationType;

  /** typedef for RegistrationType */
  typedef Landmark3DRegistration                        RegistrationType;
  typedef RegistrationType::LandmarkPointContainerType  LandmarkPointContainerType;
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

protected:

  FourViewsTrackingWithCT();
  virtual ~FourViewsTrackingWithCT();

private:

  FourViewsTrackingWithCT(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  StateType            m_InitialState;
  StateType            m_WaitingForPatientNameState;
  StateType            m_PatientNameReadyState;
  StateType            m_WaitingForDICOMDirectoryState;
  StateType            m_ImageReadyState;
  StateType            m_PatientNameVerifiedState;
  StateType            m_AddingImageLandmarkState;
  StateType            m_ImageLandmarksReadyState;
  StateType            m_InitializingTrackerState;
  StateType            m_TrackerReadyState;
  StateType            m_AddingTrackerLandmarkState;
  StateType            m_TrackerLandmarksReadyState;
  StateType            m_LandmarkRegistrationReadyState;
  StateType            m_TrackingState;

  /** Inputs to the state machine and it's designed transitions */
  InputType            m_RequestSetPatientNameInput;      //->m_WaitingForPatientNameState
  InputType            m_PatientNameInput;                //->m_PatientNameReadyState
  InputType            m_PatientNameEmptyInput;           //->m_InitialState

  InputType            m_RequestLoadImageInput;           //->m_WaitingForDICOMDirectoryState
  InputType            m_LoadImageSuccessInput;           //->m_ImageReady
  InputType            m_LoadImageFailureInput;           //->m_PatientNameReadeyState
  InputType            m_PatientNameMatchInput;           //->m_PatientNameVerifiedState
  InputType            m_OverwritePatientNameInput;       //->m_PatientNameVerifiedState
  InputType            m_ReloadImageInput;                //->m_PatientNameReadyState
  
  InputType            m_RequestAddImageLandmarkInput;    //->m_AddingImageLandmarkState
  InputType            m_NeedMoreLandmarkPointsInput;     //->m_AddingImageLandmarkState
  InputType            m_EnoughLandmarkPointsInput;       //->m_ImageLandmarksReadyState
  InputType            m_RequestClearImageLandmarksInput; //->m_PatientNameVerifiedState  

  InputType            m_RequestInitializeTrackerInput;   //
  InputType            m_InitializeTrackerSuccessInput;   //-> TrackerReadyState
  InputType            m_InitializeTrackerFailureInput;   //-> ImageLandmarksReadyState

  InputType            m_RequestAddTrackerLandmarkInput;    //->m_AddingTrackerLandmarkState
  //InputType          m_NeedMoreLandmarkPointsInput;       //->m_AddingTrackerLandmarkState
  //InputType          m_EnoughLandmarkPointsInput;         //->m_TrackerLandmarksReadyState
  InputType            m_RequestClearTrackerLandmarksInput; //->m_TrackerReadyState

  InputType            m_RequestRegistrationInput;
  InputType            m_RegistrationSuccessInput;        //->m_LandmarkRegistrationReadyState
  InputType            m_RegistrationFailureInput;        //->m_TrackerLandmarksReadyState

  InputType            m_RequestStartTrackingInput;
  InputType            m_StartTrackingSuccessInput;       //->m_TrackingState                   //FIXME, how to check if it succeed
  InputType            m_StartTrackingFailureInput;       //->m_LandmarkRegistrationReadyState

  InputType            m_RequestStopTrackingInput;
  InputType            m_StopTrackingSuccessInput;        //->m_LandmarkRegistrationReadyState  //FIXME, how to check if it succeed
  InputType            m_StopTrackingFailureInput;        //->m_TrackingState

  /** Logger */
  LogOutputType::Pointer              m_LogFileOutput;  // log output to file
  LogOutputType::Pointer              m_LogCoutOutput;  // log output to console
  std::ofstream                       m_LogFile;        // file stream
  FLTKTextLogOutput::Pointer          m_LogFLTKOutput;  // log output to FLTK text display

  /** igstkLoggerMacro create a m_Logger,igstkLogMacro(x,y) will write to m_Logger.
      This logger is for the logging of internal igstk components, eg. m_ImageReader->SetLogger( logger ).
      You can also use igstkLogMacro( logger, x, y ) to log your message in to this logger. */
  LoggerType::Pointer                 logger; // Another logger for igstk components

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

  /** Observer type for simple event, the callback can be set to a member function. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer               m_Observer;

  /** Observer type for loaded event, the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > ObserverType2;
  ObserverType2::Pointer               m_LandmarkRegistrationObserver;
  ObserverType2::Pointer               m_ViewPickerObserver;

  /** Ellipsoid spatial object, used to represent the landmark point, tip of the probe. */
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
  void SetPatientName();
  void LoadImage();
  void VerifyPatientName(); 
  void ConnectImageRepresentation();
  void AddImageLandmark();
  void ClearImageLandmarks();
  void InitializeTracker();
  void GetTrackerTransform();
  void AddTrackerLandmark();
  void ClearTrackerLandmarks();  
  void Registration();
  void StartTracking();
  void Tracking();
  void StopTracking();
  void ResliceImage();
  void ResliceImage( IndexType index );  
  void Reset();
  
  /** Callback functions for picking and registration success events. */
  void GetLandmarkRegistrationTransform( const itk::EventObject & event);
  void DrawPickedPoint( const itk::EventObject & event );
  
};

} // end of namespace

#endif

