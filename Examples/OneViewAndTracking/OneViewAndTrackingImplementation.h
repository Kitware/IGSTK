/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __OneViewAndTrackingImplementation_h
#define __OneViewAndTrackingImplementation_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "OneViewAndTrackingGUI.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{


class OneViewAndTrackingImplementation : public OneViewAndTrackingGUI
{
public:

  typedef itk::Logger              LoggerType; 
  typedef itk::StdStreamLogOutput  LogOutputType;

  typedef igstk::AuroraTracker     TrackerType;

#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif

public:

  OneViewAndTrackingImplementation()
    {
    m_Tracker = TrackerType::New();

    m_Logger = LoggerType::New();
    m_LogOutput = LogOutputType::New();
    m_LogFile.open("logOneViewAndTracking.txt");
    if( !m_LogFile.fail() )
      {
      m_LogOutput->SetStream( m_LogFile );
      }
    else
      {
      std::cerr << "Problem opening Log file, using cerr instead " 
        << std::endl;
      m_LogOutput->SetStream( std::cerr );
      }
    m_Logger->AddLogOutput( m_LogOutput );

    // add stdout for debug purposes
    LogOutputType::Pointer coutLogOutput = LogOutputType::New();
    coutLogOutput->SetStream( std::cout );
    m_Logger->AddLogOutput( coutLogOutput );

    m_Logger->SetPriorityLevel( LoggerType::DEBUG );
    m_Tracker->SetLogger( m_Logger );

    m_Communication = CommunicationType::New();
    m_Communication->SetLogger( m_Logger );
    m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
    m_Communication->SetParity( igstk::SerialCommunication::NoParity );
    m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
    m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
    m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
    m_Communication->SetHardwareHandshake( 
      igstk::SerialCommunication::HandshakeOff );
    m_Tracker->SetCommunication(m_Communication);

    m_Communication->OpenCommunication();

    m_Tracker->RequestOpen();
    m_Tracker->RequestInitialize();

    // Set up the four quadrant views
    this->Display3D->RequestResetCamera();
    this->Display3D->Update();
    this->Display3D->RequestEnableInteractions();
    this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
    this->Display3D->RequestStart();
   
    m_Tracking = false;
    }

  ~OneViewAndTrackingImplementation()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracker->RequestClose();
    }
  
  void EnableTracking()
    {
    m_Tracking = true;
    m_Tracker->RequestStartTracking();
    }
  
  void DisableTracking()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    }
  
  void AddCylinder( igstk::CylinderObjectRepresentation 
                                                    * cylinderRepresentation )
    {
    this->Display3D->RequestAddObject( cylinderRepresentation->Copy() );
    }
  
  void AddEllipsoid( igstk::EllipsoidObjectRepresentation * 
      ellipsoidRepresentation )
    {
    this->Display3D->RequestAddObject( ellipsoidRepresentation->Copy() );
    }

  void AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
    {
    const unsigned int toolPort = 0;
    const unsigned int toolNumber = 0;
    m_Tracker->AttachObjectToTrackerTool( 
        toolPort, toolNumber, objectToTrack );
    }

private:

  LoggerType::Pointer     m_Logger;
  LogOutputType::Pointer  m_LogOutput;
  TrackerType::Pointer    m_Tracker;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;
};


} // end of namespace

#endif
