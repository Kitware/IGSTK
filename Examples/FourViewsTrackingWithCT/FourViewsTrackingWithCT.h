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

#include "igstkLandmarkRegistration.h"

#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"

#include "igstkSandboxConfigure.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

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

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

  /** typedefs for the tracker */
  typedef igstk::PolarisTracker          TrackerType;

  /** typedefs for the communication */
  #ifdef WIN32
    typedef igstk::SerialCommunicationForWindows  CommunicationType;
  #else
    typedef igstk::SerialCommunicationForPosix    CommunicationType;
  #endif

  /** typedef for ImageReaderType */
  typedef CTImageReader                           ImageReaderType;

  /** typedef for ImageSpatialObjectType */
  typedef CTImageSpatialObject                    ImageSpatialObjectType;

  /** typedef for ImageRepresentationType */
  typedef CTImageSpatialObjectRepresentation      ImageRepresentationType;

  virtual void RequestRegisterPatientInfo();
  virtual void RequestLoadImage();
  virtual void RequestInitializeTracker();
  virtual void RequestStartTracking();
  virtual void RequestStopTracking();
  
protected:

  FourViewsTrackingWithCT( void );
  virtual ~FourViewsTrackingWithCT( void );

private:

  /** Inputs to the State Machine */
  InputType            m_PatientNameValidInput;

  /** States for the State Machine */
  StateType            m_InitialState;
  StateType            m_PatientInfoRegisteredState;
  StateType            m_ImageLoadedState;
  StateType            m_PatientInfoValidState;
  StateType            m_TrackerReadyState;
  StateType            m_TrackingState;
  StateType            m_TrackerLostState;

  /** Logger */
  LogOutputType::Pointer  m_LogFileOutput;
  LogOutputType::Pointer  m_LogCoutOutput;
  std::ofstream           m_LogFile;

  /** Internal variables */
  ImageReaderType::Pointer            m_ImageReader;
  ImageRepresentationType::Pointer    m_ImageRepresentation;

  std::string             m_PatientName;
  std::string             m_PatientNameToBeSet;

  /** Action methods to be invoked only by the state machine */
  void RegisterPatientInfo();
  void LoadImage( const char *);
  void InitializeTracker();
  void StartTracking();
  void StopTracking();

  
};

} // end of namespace

#endif

