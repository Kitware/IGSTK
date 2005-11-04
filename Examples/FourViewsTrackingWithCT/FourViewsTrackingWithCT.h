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
  virtual void RequestSetImageLandmarks();
  virtual void RequestInitializeTracker();
  virtual void RequestSetTrackerLandmarks();
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
  StateType            m_ImageLandmarksReadyState;
  StateType            m_TrackerReadyState;
  StateType            m_TrackerLandmarksReadyState;
  StateType            m_LandmarkRegistrationReadyState;
  StateType            m_TrackingState;
  //StateType            m_TrackerLostState;

  /** Inputs to the state machine and it's designed transitions */
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
  // PatientNameVerifiedState
  InputType            m_AddImageLandmarksSuccessInput;   //-> ImageLandmarksReadyState
  InputType            m_AddImageLandmarksFailureInput;   //-> PatientNameVerifiedState
  // ImageLandmarksReadyState
  InputType            m_InitializeTrackerSuccessInput;   //-> TrackerReadyState
  InputType            m_InitializeTrackerFailureInput;   //-> ImageLandmarksReadyState
  // TrackerReadyState
  InputType            m_AddTrackerLandmarksSuccessInput; //-> TrackerLandmarksReadyState
  InputType            m_AddTrackerLandmarksFailureInput; //-> TrackerReadyState
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
  LoggerType::Pointer                 logger; // Another logger for igstk components

  /** Internal variables. */
  std::string                         m_PatientName;
  std::string                         m_PatientNameToBeSet;

  //std::string                         m_DICOMDirectory;
  
  ImageReaderType::Pointer            m_ImageReader;

  ImageRepresentationType::Pointer    m_ImageRepresentationAxial;
  ImageRepresentationType::Pointer    m_ImageRepresentationCoronal;
  ImageRepresentationType::Pointer    m_ImageRepresentationSagittal;

  RegistrationType::Pointer           m_LandmarkRegistrtion;
  LandmarkPointContainerType          m_ImageLandmarks;
  LandmarkPointContainerType          m_ImageLandmarksToBeSet;
  LandmarkPointContainerType          m_TrackerLandmarks;
  LandmarkPointContainerType          m_TrackerLandmarksToBeSet;

  CommunicationType::Pointer          m_SerialCommunication;
  TrackerType::Pointer                m_Tracker;


  /** Action methods to be invoked only by the state machine */
  void SetPatientName();
  void VerifyPatientName(); 
  void ConnectImageRepresentation();
};

} // end of namespace

#endif

