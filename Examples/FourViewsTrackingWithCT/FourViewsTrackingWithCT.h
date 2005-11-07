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

  /** Public request methods for the GUI. */
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
  virtual void RequestResliceImage();   // TEMP

protected:

  FourViewsTrackingWithCT();
  virtual ~FourViewsTrackingWithCT();

private:

  FourViewsTrackingWithCT(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  StateType            m_InitialState;
  StateType            m_PatientNameReadyState;
  StateType            m_ImageReadyState;
  StateType            m_PatientNameVerifiedState;
  StateType            m_TrackerReadyState;
  StateType            m_AddingImageLandmarkState;
  StateType            m_ImageLandmarksReadyState;
  StateType            m_AddingTrackerLandmarkState;
  StateType            m_TrackerLandmarksReadyState;
  StateType            m_LandmarkRegistrationReadyState;
  StateType            m_TrackingState;
  //StateType            m_TrackerLostState;

  /** Inputs to the state machine and it's designed transitions */
  InputType            m_RequestSetPatientNameInput;
  InputType            m_RequestLoadImageInput;
  InputType            m_RequestInitializeTrackerInput;
  InputType            m_RequestAddImageLandmarkInput;
  InputType            m_RequestClearImageLandmarksInput;
  InputType            m_RequestAddTrackerLandmarkInput;
  InputType            m_RequestClearTrackerLandmarksInput;
  InputType            m_RequestRegistrationInput;
  InputType            m_RequestStartTrackingInput;
  InputType            m_RequestStopTrackingInput;
  InputType            m_RequestResliceImageInput;

  // FROM                                                 //-> TO
  // InitialState
  InputType            m_PatientNameInput;                //-> PatientNameReadyState
  InputType            m_PatientNameEmptyInput;           //-> InitialState
  // PatientNameReadyState
  InputType            m_LoadImageSuccessInput;           //-> ImageReadyState
  InputType            m_LoadImageFailureInput;           //-> PatientNameReadeyState
  // ImageReadyState
  InputType            m_PatientNameMatchInput;           //-> PatientNameVerifiedState
  InputType            m_OverwritePatientNameInput;       //-> PatientNameVerifiedState
  InputType            m_ReloadImageInput;                //-> PatientNameReadyState
  // ImageLandmarksReadyState
  InputType            m_InitializeTrackerSuccessInput;   //-> TrackerReadyState
  InputType            m_InitializeTrackerFailureInput;   //-> ImageLandmarksReadyState
  // PatientNameVerifiedState
  InputType            m_NeedMoreLandmarkPointsInput;      //-> ImageLandmarksReadyState
  InputType            m_EnoughLandmarkPointsInput;        //-> PatientNameVerifiedState
  // TrackerReadyState
  //InputType            m_AddTrackerLandmarksSuccessInput; //-> TrackerLandmarksReadyState
  //InputType            m_AddTrackerLandmarksFailureInput; //-> TrackerReadyState
  // TrackerLandmarksReadyState
  InputType            m_RegistrationSuccessInput;        //-> LandmarkRegistrationReadyState
  InputType            m_RegistrationFailureInput;        //-> TrackerLandmarksReadyState
  // LandmarkRegistrationReadyState
  InputType            m_StartTrackingSuccessInput;       //-> TrackingState
  InputType            m_StartTrackingFailureInput;       //-> LandmarkRegistrationReadyState
  // TrackingState
  InputType            m_StopTrackingSuccessInput;        //-> LandmarkRegistrationReadyState
  InputType            m_StopTrackingFailureInput;        //-> ??????

  
  /** Logger */
  LogOutputType::Pointer              m_LogFileOutput;
  LogOutputType::Pointer              m_LogCoutOutput;
  std::ofstream                       m_LogFile;
  FLTKTextLogOutput::Pointer          m_LogFLTKOutput;
  LoggerType::Pointer                 logger; // Another logger for igstk components

  /** Internal variables. */
  std::string                         m_PatientName;
  
  ImageReaderType::Pointer            m_ImageReader;

  ImageRepresentationType::Pointer    m_ImageRepresentationAxial;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal;

  unsigned int                        m_SliceNumber[3];
  unsigned int                        m_SliceNumberToBeSet[3];

  RegistrationType::Pointer           m_LandmarkRegistration;
  LandmarkPointContainerType          m_ImageLandmarksContainer;
  LandmarkPointContainerType          m_TrackerLandmarksContainer;

  Transform                           m_Transform;  //WHY igstk::Transform has no smart pointer
  
  CommunicationType::Pointer          m_SerialCommunication;
  TrackerType::Pointer                m_Tracker;

  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  PulseGenerator::Pointer             m_PulseGenerator;
  ObserverType::Pointer               m_PulseObserver;

  typedef itk::ReceptorMemberCommand < Self > ObserverType2;
  ObserverType2::Pointer               m_LandmarkRegistrationObserver;

  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef EllipsoidType::Pointer                  EllipsoidPointer;
  
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;
  typedef EllipsoidRepresentationType::Pointer    EllipsoidRepresentationPointer;


  EllipsoidPointer                                m_Ellipsoid;
  EllipsoidRepresentationPointer                  m_EllipsoidRepresentationAxial;
  EllipsoidRepresentationPointer                  m_EllipsoidRepresentationSagittal;
  EllipsoidRepresentationPointer                  m_EllipsoidRepresentationCoronal;

  typedef igstk::CylinderObject                   CylinderType;
  typedef CylinderType::Pointer                   CylinderPointer;
  
  typedef igstk::CylinderObjectRepresentation     CylinderRepresentationType;
  typedef CylinderRepresentationType::Pointer     CylinderRepresentationPointer;


  CylinderPointer                                 m_Cylinder;
  CylinderRepresentationPointer                   m_CylinderRepresentationAxial;
  CylinderRepresentationPointer                   m_CylinderRepresentationSagittal;
  CylinderRepresentationPointer                   m_CylinderRepresentationCoronal;


  /** Action methods to be invoked only by the state machine */
  void SetPatientName();
  void LoadImage();
  void InitializeTracker();
  void AddImageLandmark();
  void AddTrackerLandmark();
  void Registration();
  void StartTracking();
  void StopTracking();
  void ResliceImage();
  void VerifyPatientName(); 
  void ConnectImageRepresentation();
  void GetTrackerTransform();
  void GetLandmarkRegistrationTransform( const itk::EventObject & event);
};

} // end of namespace

#endif

