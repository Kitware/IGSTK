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
TrackerInitializer::TrackerInitializer()
{
    m_ErrorMessage  = "";
    m_HasReferenceTool = 0;
}

std::string TrackerInitializer::GetTrackerTypeAsString()
{
  switch( m_TrackerType )
  {
  case TrackerConfiguration::Polaris:
    return "Polaris"; break;
  case TrackerConfiguration::Aurora:
    return "Aurora"; break;
  case TrackerConfiguration::Micron:
    return "Micron"; break;
  default:
    return "Not Defined";
  }
}

int TrackerInitializer::RequestInitializeTracker()
{
  if( m_TrackerConfiguration == NULL )
    {
    m_ErrorMessage = "Invalid configuration file";
    return 0;
    }

  m_TrackerType = m_TrackerConfiguration->GetTrackerType();

  if( m_TrackerType == TrackerConfiguration::Polaris )
    {
    return InitializePolarisTracker();
    }
  else if (m_TrackerType == TrackerConfiguration::Aurora)
    {
    return InitializeAuroraTracker();
    }
  else if (m_TrackerType == TrackerConfiguration::Micron)
    {
    #ifdef IGSTK_USE_MicronTracker
    return InitializeMicronTracker();
    #else
    m_ErrorMessage = 
      "Please configure IGSTK to use MicronTrakcer first\n";
    return 0;
    #endif /* IGSTK_USE_MicronTracker */
    
    }
  else
    {
    m_ErrorMessage = "Invalid tracker type";
    return 0;
    }

}

// FIXME: Add error events listener
int TrackerInitializer::InitializePolarisTracker()
{
  NDITrackerConfiguration * trackerConfig = 
    m_TrackerConfiguration->GetNDITrackerConfiguration();
  
  m_Communication = SerialCommunication::New();

  m_Communication->SetPortNumber( trackerConfig->m_COMPort );
  m_Communication->SetParity( SerialCommunication::NoParity );
  m_Communication->SetBaudRate(SerialCommunication::BaudRate115200);
  m_Communication->SetDataBits( SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake( SerialCommunication::HandshakeOff);
  if ( !m_Communication->OpenCommunication())
    {
    std::cout << "Serial port open failure\n";
    return 0;
    }

  m_Tracker = m_PolarisTracker = PolarisTracker::New();
  m_PolarisTracker->SetCommunication( m_Communication );
  m_PolarisTracker->RequestOpen();

  m_TrackerToolList.clear();
  for( unsigned int i=0; i< trackerConfig->m_TrackerToolList.size(); i++ )
    {
    PolarisTrackerTool::Pointer tool = PolarisTrackerTool::New();
    NDITrackerToolConfiguration * toolConfig =
      trackerConfig->m_TrackerToolList[i];

    if( toolConfig->m_WiredTool)
      {
      tool->RequestSelectWiredTrackerTool();
      tool->RequestSetPortNumber( toolConfig->m_PortNumber );
      if ( toolConfig->m_HasSROM )
        {
        tool->RequestSetSROMFileName( toolConfig->m_SROMFile );
        }
      tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform);
      tool->RequestConfigure();
      }
    else
      {
      tool->RequestSelectWirelessTrackerTool();
      tool->RequestSetSROMFileName( toolConfig->m_SROMFile );
      tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform);
      tool->RequestConfigure();
      }
    
    tool->RequestAttachToTracker( m_PolarisTracker );

    if( toolConfig->m_IsReference )
      {
      m_PolarisTracker->RequestSetReferenceTool( tool );
      m_ReferenceTool = tool;
      m_HasReferenceTool = 1;
      }
    else
      {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
      }
    }

  m_Tracker->RequestSetFrequency( trackerConfig->m_Frequency );
  m_Tracker->RequestStartTracking();

  return 1;

}

// FIXME: Add error events listener
int TrackerInitializer::InitializeAuroraTracker()
{
  NDITrackerConfiguration * trackerConfig = 
    m_TrackerConfiguration->GetNDITrackerConfiguration();

  m_Communication = SerialCommunication::New();

  m_Communication->SetPortNumber( trackerConfig->m_COMPort );
  m_Communication->SetParity( SerialCommunication::NoParity );
  m_Communication->SetBaudRate(SerialCommunication::BaudRate115200);
  m_Communication->SetDataBits( SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake( SerialCommunication::HandshakeOff);
  
  if( !m_Communication->OpenCommunication())
    {
    std::cout << "Serial port open failure\n";
    return 0;
    }

  m_Tracker = m_AuroraTracker = AuroraTracker::New();
  m_AuroraTracker->SetCommunication( m_Communication );
  m_AuroraTracker->RequestOpen();

  m_TrackerToolList.clear();
  for ( unsigned int i=0; i< trackerConfig->m_TrackerToolList.size(); i++)
    {
    AuroraTrackerTool::Pointer tool = AuroraTrackerTool::New();
    NDITrackerToolConfiguration * toolConfig =
      trackerConfig->m_TrackerToolList[i];

    if( toolConfig->m_Is5DOF )
      {
      tool->RequestSelect5DOFTrackerTool();
      tool->RequestSetPortNumber( toolConfig->m_PortNumber );
      tool->RequestSetChannelNumber( toolConfig->m_ChannelNumber );
      if ( toolConfig->m_HasSROM )
        {
        tool->RequestSetSROMFileName( toolConfig->m_SROMFile );
        }
      tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform);
      tool->RequestConfigure();
      }
    else // 6 DOF tool
      {
      tool->RequestSelect6DOFTrackerTool();
      tool->RequestSetPortNumber( toolConfig->m_PortNumber);
      if ( toolConfig->m_HasSROM )
        {
        tool->RequestSetSROMFileName( toolConfig->m_SROMFile );
        }
      tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform);
      tool->RequestConfigure();
      }

    tool->RequestAttachToTracker( m_AuroraTracker );

    if ( toolConfig->m_IsReference )
      {
      m_AuroraTracker->RequestSetReferenceTool( tool );
      m_ReferenceTool = tool;
      m_HasReferenceTool = 1;
      }
    else
      {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
      }
    }

  m_Tracker->RequestSetFrequency( trackerConfig->m_Frequency );
  m_Tracker->RequestStartTracking();

  return 1;
}

#ifdef IGSTK_USE_MicronTracker
// FIXME: Add error events listener
int TrackerInitializer::InitializeMicronTracker()
{
  MicronTrackerConfiguration * trackerConfig = 
    m_TrackerConfiguration->GetMicronTrackerConfiguration();


  m_Tracker = m_MicronTracker = MicronTracker::New();
  
  m_MicronTracker->SetCameraCalibrationFilesDirectory( 
    trackerConfig->m_CameraCalibrationFileDirectory );

  m_MicronTracker->SetInitializationFile( 
    trackerConfig->m_InitializationFile );

  m_MicronTracker->SetMarkerTemplatesDirectory( 
    trackerConfig->m_TemplatesDirectory );

  m_MicronTracker->RequestOpen();

  m_TrackerToolList.clear();
  for ( unsigned int i=0; i< trackerConfig->m_TrackerToolList.size(); i++)
    {
    MicronTrackerTool::Pointer tool = MicronTrackerTool::New();
    MicronTrackerToolConfiguration * toolConfig = 
      trackerConfig->m_TrackerToolList[i];

    tool->RequestSetMarkerName( toolConfig->m_MarkerName );
    tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform );
    tool->RequestConfigure();

    tool->RequestAttachToTracker( m_MicronTracker );

    if( toolConfig->m_IsReference )
      {
      m_MicronTracker->RequestSetReferenceTool( tool );
      m_ReferenceTool = tool;
      m_HasReferenceTool = 1;
      }
    else
      {
      TrackerTool::Pointer t = tool.GetPointer();
      m_TrackerToolList.push_back( t );
      }
    }

  m_Tracker->RequestSetFrequency( trackerConfig->m_Frequency );
  m_Tracker->RequestStartTracking();

  return 1;

}

#endif /* IGSTK_USE_MicronTracker */

/** Destructor */
TrackerInitializer::~TrackerInitializer()
{
  if (m_TrackerConfiguration)
    {
    delete m_TrackerConfiguration;
    }

}

} // end of namespace
