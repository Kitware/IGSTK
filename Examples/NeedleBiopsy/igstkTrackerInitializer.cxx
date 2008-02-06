/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerInitializer.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerInitializer.h"

namespace igstk
{


/** Constructor: Initializes all internal variables. */
TrackerInitializer::TrackerInitializer( TrackerConfiguration * config)
{ 
    m_TrackerConfiguration = config;
    m_ErrorMessage  = "";
}

int TrackerInitializer::RequestInitializeTracker()
{
  if ( m_TrackerConfiguration == NULL )
  {
    m_ErrorMessage = "Invalid configuration file";
    return 0;
  }

  m_TrackerType = m_TrackerConfiguration->GetTrackerType();

  if ( m_TrackerType == TrackerConfiguration::Polaris )
  {
    return InitializePolarisTracker();
  }
  else if (m_TrackerType == TrackerConfiguration::Aurora)
  {
    return InitializeAuroraTracker();
  }
  else if (m_TrackerType == TrackerConfiguration::Micron)
  {
    #ifdef IGSTKSandbox_USE_MicronTracker
    return InitializeMicronTracker();
    #else
    m_ErrorMessage = "Please configure IGSTKSandbox to use MicronTrakcer first\n";
    return 0;
    #endif /* IGSTKSandbox_USE_MicronTracker */
    
  }
  else
    {
    m_ErrorMessage = "Invalid tracker type";
    return 0;
    }

}

int TrackerInitializer::InitializePolarisTracker()
{
  NDITrackerConfiguration * trackerConfig = m_TrackerConfiguration->GetNDITrackerConfiguration();
  
  SerialCommunication::Pointer serialCom = SerialCommunication::New();

  serialCom->SetPortNumber( trackerConfig->COMPort );
  serialCom->SetParity( SerialCommunication::NoParity );
  serialCom->SetBaudRate(SerialCommunication::BaudRate115200);
  serialCom->SetDataBits( SerialCommunication::DataBits8 );
  serialCom->SetStopBits( SerialCommunication::StopBits1 );
  serialCom->SetHardwareHandshake( SerialCommunication::HandshakeOff);
  serialCom->OpenCommunication();

  m_Tracker = m_PolarisTracker = PolarisTracker::New();
  m_PolarisTracker->SetCommunication( serialCom );
  m_PolarisTracker->RequestOpen();

  for ( int i=0; i< trackerConfig->TrackerToolList.size(); i++)
  {
    PolarisTrackerTool::Pointer tool = PolarisTrackerTool::New();
    NDITrackerToolConfiguration * toolConfig = trackerConfig->TrackerToolList[i];

    if ( toolConfig->WiredTool)
    {
      tool->RequestSelectWiredTrackerTool();
      tool->RequestSetPortNumber( toolConfig->PortNumber );
      if ( toolConfig->HasSROM )
      {
        tool->RequestSetSROMFileName( toolConfig->SROMFile );
      }
      tool->SetCalibrationTransform( toolConfig->CalibrationTransform);
      tool->RequestConfigure();
    }
    else
    {
      tool->RequestSelectWirelessTrackerTool();
      tool->RequestSetSROMFileName( toolConfig->SROMFile );
      tool->SetCalibrationTransform( toolConfig->CalibrationTransform);
      tool->RequestConfigure();
    }
    
    tool->RequestAttachToTracker( m_PolarisTracker );

    if ( toolConfig->IsReference )
    {
      m_PolarisTracker->RequestSetReferenceTool( tool );
    }
    else
    {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
    }

    m_Tracker->RequestSetFrequency( trackerConfig->Frequency );
    m_Tracker->RequestStartTracking();
    
  }

  return 1;

}

int TrackerInitializer::InitializeAuroraTracker()
{
  NDITrackerConfiguration * trackerConfig = m_TrackerConfiguration->GetNDITrackerConfiguration();

  SerialCommunication::Pointer serialCom = SerialCommunication::New();

  serialCom->SetPortNumber( trackerConfig->COMPort );
  serialCom->SetParity( SerialCommunication::NoParity );
  serialCom->SetBaudRate(SerialCommunication::BaudRate115200);
  serialCom->SetDataBits( SerialCommunication::DataBits8 );
  serialCom->SetStopBits( SerialCommunication::StopBits1 );
  serialCom->SetHardwareHandshake( SerialCommunication::HandshakeOff);
  serialCom->OpenCommunication();

  m_Tracker = m_AuroraTracker = AuroraTracker::New();
  m_AuroraTracker->SetCommunication( serialCom );
  m_AuroraTracker->RequestOpen();

  for ( int i=0; i< trackerConfig->TrackerToolList.size(); i++)
  {
    AuroraTrackerTool::Pointer tool = AuroraTrackerTool::New();
    NDITrackerToolConfiguration * toolConfig = trackerConfig->TrackerToolList[i];

    if ( toolConfig->Is5DOF)
    {
      tool->RequestSelect5DOFTrackerTool();
      tool->RequestSetPortNumber( toolConfig->PortNumber );
      tool->RequestSetChannelNumber( toolConfig->ChannelNumber );
      if ( toolConfig->HasSROM )
      {
        tool->RequestSetSROMFileName( toolConfig->SROMFile );
      }
      tool->SetCalibrationTransform( toolConfig->CalibrationTransform);
      tool->RequestConfigure();
    }
    else // 6 DOF tool
    {
      tool->RequestSelect6DOFTrackerTool();
      tool->RequestSetPortNumber( toolConfig->PortNumber);
      if ( toolConfig->HasSROM )
      {
        tool->RequestSetSROMFileName( toolConfig->SROMFile );
      }
      tool->SetCalibrationTransform( toolConfig->CalibrationTransform);
      tool->RequestConfigure();
    }

    tool->RequestAttachToTracker( m_AuroraTracker );

    if ( toolConfig->IsReference )
    {
      m_AuroraTracker->RequestSetReferenceTool( tool );
    }
    else
    {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
    }

    m_Tracker->RequestSetFrequency( trackerConfig->Frequency );
    m_Tracker->RequestStartTracking();

  }

  return 1;

}

#ifdef IGSTKSandbox_USE_MicronTracker
int TrackerInitializer::InitializeMicronTracker()
{
  MicronTrackerConfiguration * trackerConfig = m_TrackerConfiguration->GetMicronTrackerConfiguration();


  m_Tracker = m_MicronTracker = MicronTracker::New();
  
  m_MicronTracker->SetCameraCalibrationFilesDirectory( trackerConfig->CameraCalibrationFileDirectory );
  m_MicronTracker->SetInitializationFile( trackerConfig->InitializationFile );
  m_MicronTracker->SetMarkerTemplatesDirectory( trackerConfig->TemplatesDirectory );

  m_MicronTracker->RequestOpen();

  for ( int i=0; i< trackerConfig->TrackerToolList.size(); i++)
  {
    MicronTrackerTool::Pointer tool = MicronTrackerTool::New();
    MicronTrackerToolConfiguration * toolConfig = trackerConfig->TrackerToolList[i];

    tool->RequestSetMarkerName( toolConfig->MarkerName );
    tool->SetCalibrationTransform( toolConfig->CalibrationTransform );
    tool->RequestConfigure();

    tool->RequestAttachToTracker( m_MicronTracker );

    if ( toolConfig->IsReference )
    {
      m_MicronTracker->RequestSetReferenceTool( tool );
    }
    else
    {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
    }

    m_Tracker->RequestSetFrequency( trackerConfig->Frequency );
    m_Tracker->RequestStartTracking();

  }

  return 1;

}

#endif /* IGSTKSandbox_USE_MicronTracker */

/** Destructor */
TrackerInitializer::~TrackerInitializer()
{
  if (m_TrackerConfiguration)
  {
    delete m_TrackerConfiguration;
  }

}

} // end of namespace
