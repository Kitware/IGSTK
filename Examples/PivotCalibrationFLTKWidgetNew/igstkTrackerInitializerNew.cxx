/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerInitializerNew.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerInitializerNew.h"
#include "itksys/SystemTools.hxx"
namespace igstk
{


/** Constructor: Initializes all internal variables. */
TrackerInitializerNew::TrackerInitializerNew()
{
    m_ErrorMessage  = "";
    m_HasReferenceTool = 0;

    //create error observer
    this->m_errorObserver = TrackingErrorObserver::New();
}

void
TrackerInitializerNew::TrackingErrorObserver::Execute(
    itk::Object *caller,
    const itk::EventObject & event )
{
    std::map<std::string,std::string>::iterator it;
    std::string className = event.GetEventName();
    it = this->m_ErrorEvent2ErrorMessage.find( className );
    if( it != this->m_ErrorEvent2ErrorMessage.end() )
    {
        this->m_ErrorOccured = true;
        fl_alert( (*it).second.c_str() );
        fl_beep( FL_BEEP_ERROR );
    }
}

void
TrackerInitializerNew::TrackingErrorObserver::Execute(
    const itk::Object *caller,
    const itk::EventObject & event )
{
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
}

void
TrackerInitializerNew::TrackingErrorObserver::ClearError()
{
    this->m_ErrorOccured = false;
}

bool
TrackerInitializerNew::TrackingErrorObserver::Error()
{
    return this->m_ErrorOccured;
}

TrackerInitializerNew
::TrackingErrorObserver::TrackingErrorObserver()
: m_ErrorOccured( false )
{
    //serial communication errors
    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::OpenPortErrorEvent().GetEventName(), "Error opening com port." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::ClosePortErrorEvent().GetEventName(), "Error closing com port." ) );

    //tracker errors
    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerOpenErrorEvent().GetEventName(),
        "Error opening tracker communication." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerInitializeErrorEvent().GetEventName(),
        "Error initializing tracker." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerStartTrackingErrorEvent().GetEventName(),
        "Error starting tracking." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerStopTrackingErrorEvent().GetEventName(),
        "Error stopping tracking." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerCloseErrorEvent().GetEventName(),
        "Error closing tracker communication." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::TrackerUpdateStatusErrorEvent().GetEventName(),
        "Error updating transformations from tracker." ) );

    this->m_ErrorEvent2ErrorMessage.insert(
        std::pair<std::string,std::string>(
        igstk::AttachingTrackerToolToTrackerErrorEvent().GetEventName(),
        "Error attaching tool to tracker." ) );
}

std::string TrackerInitializerNew::GetTrackerTypeAsString()
{
  switch( m_TrackerType )
  {
  case TrackerConfigurationNew::Polaris:
    return "Polaris"; break;
  case TrackerConfigurationNew::Aurora:
    return "Aurora"; break;
  case TrackerConfigurationNew::FlockOfBirds:
    return "FlockOfBirds"; break;
  case TrackerConfigurationNew::Micron:
    return "Micron"; break;
  default:
    return "Not Defined";
  }
}

int TrackerInitializerNew::RequestInitializeTracker()
{
  if( m_TrackerConfiguration == NULL )
    {
    m_ErrorMessage = "Invalid configuration file";
    return 0;
    }

  m_TrackerType = m_TrackerConfiguration->GetTrackerType();

  if( m_TrackerType == TrackerConfigurationNew::Polaris )
    {
    return InitializePolarisTracker();
    }
  else if (m_TrackerType == TrackerConfigurationNew::Aurora)
    {
    return InitializeAuroraTracker();
    }
  else if (m_TrackerType == TrackerConfigurationNew::FlockOfBirds)
  {
    return InitializeFlockOfBirdsTracker();
  }
  else if (m_TrackerType == TrackerConfigurationNew::Micron)
    {
    #ifdef IGSTKSandbox_USE_MicronTracker
    return InitializeMicronTracker();
    #else
    m_ErrorMessage = 
      "Please configure IGSTKSandbox to use MicronTrakcer first\n";
    return 0;
    #endif /* IGSTKSandbox_USE_MicronTracker */
    
    }
  else
    {
    m_ErrorMessage = "Invalid tracker type";
    return 0;
    }

}

// FIXME: Add error events listener
int TrackerInitializerNew::InitializePolarisTracker()
{
  NDITrackerConfiguration * trackerConfig = 
    m_TrackerConfiguration->GetNDITrackerConfiguration();
  
  m_Communication = SerialCommunication::New();

  //observe errors generated by the serial communication
  m_Communication->AddObserver( igstk::OpenPortErrorEvent(),
                                              this->m_errorObserver );
  m_Communication->AddObserver( igstk::ClosePortErrorEvent(),
                                          this->m_errorObserver );

  m_Communication->SetPortNumber( trackerConfig->m_COMPort );
  m_Communication->SetParity( SerialCommunication::NoParity );
  m_Communication->SetBaudRate(SerialCommunication::BaudRate115200);
  m_Communication->SetDataBits( SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake( SerialCommunication::HandshakeOff);

  m_Communication->OpenCommunication();
  if( this->m_errorObserver->Error() )
  {
    this->m_errorObserver->ClearError();
    return 0;
  }

  m_Tracker = m_PolarisTracker = PolarisTracker::New();

  //observe errors generated by the tracker
  m_Tracker->AddObserver( igstk::TrackerOpenErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerInitializeErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerStartTrackingErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerStopTrackingErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerCloseErrorEvent(),
                                this->m_errorObserver );
  m_Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(),
                                this->m_errorObserver );

  m_PolarisTracker->SetCommunication( m_Communication );
  m_PolarisTracker->RequestOpen();
  if( this->m_errorObserver->Error() )
    {
    this->m_errorObserver->ClearError();
    return 0;
    }

  m_TrackerToolList.clear();
  for( int i=0; i< trackerConfig->m_TrackerToolList.size(); i++ )
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
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

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

  //start tracking
  m_Tracker->RequestStartTracking();
  if( this->m_errorObserver->Error() )
    {
     this->m_errorObserver->ClearError();
     return 0;
    }

  return 1;

}

// FIXME: Add error events listener
int TrackerInitializerNew::InitializeAuroraTracker()
{
  NDITrackerConfiguration * trackerConfig = 
    m_TrackerConfiguration->GetNDITrackerConfiguration();

  m_Communication = SerialCommunication::New();

  //observe errors generated by the serial communication
  m_Communication->AddObserver( igstk::OpenPortErrorEvent(),
      this->m_errorObserver );
  m_Communication->AddObserver( igstk::ClosePortErrorEvent(),
      this->m_errorObserver );

  m_Communication->SetPortNumber( trackerConfig->m_COMPort );
  m_Communication->SetParity( SerialCommunication::NoParity );
  m_Communication->SetBaudRate(SerialCommunication::BaudRate115200);
  m_Communication->SetDataBits( SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake( SerialCommunication::HandshakeOff);
  
  m_Communication->OpenCommunication();
  if( this->m_errorObserver->Error() )
  {
      this->m_errorObserver->ClearError();
      return 0;
  }

  m_Tracker = m_AuroraTracker = AuroraTracker::New();
  m_AuroraTracker->SetCommunication( m_Communication );
  m_AuroraTracker->RequestOpen();
  if( this->m_errorObserver->Error() )
  {
      this->m_errorObserver->ClearError();
      return 0;
  }

  m_TrackerToolList.clear();
  for ( int i=0; i< trackerConfig->m_TrackerToolList.size(); i++)
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
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

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
  if( this->m_errorObserver->Error() )
  {
      this->m_errorObserver->ClearError();
      return 0;
  }

  return 1;
}

// FIXME: Add error events listener
int TrackerInitializerNew::InitializeFlockOfBirdsTracker()
{
   AscensionTrackerConfiguration * trackerConfig = 
       m_TrackerConfiguration->GetAscensionTrackerConfiguration();

    m_Communication = SerialCommunication::New();

    //observe errors generated by the serial communication
    m_Communication->AddObserver( igstk::OpenPortErrorEvent(),
        this->m_errorObserver );
    m_Communication->AddObserver( igstk::ClosePortErrorEvent(),
        this->m_errorObserver );

    m_Communication->SetPortNumber( trackerConfig->m_COMPort );
    m_Communication->SetParity( SerialCommunication::NoParity );
    m_Communication->SetBaudRate(SerialCommunication::BaudRate115200);
    m_Communication->SetDataBits( SerialCommunication::DataBits8 );
    m_Communication->SetStopBits( SerialCommunication::StopBits1 );
    m_Communication->SetHardwareHandshake( SerialCommunication::HandshakeOff);

// FIXME: see if we want to log data
//     m_Communication->SetLogger(m_Logger);
//     
//     m_Communication->SetCaptureFileName( "InitializeFlockOfBirdsTrackerLog.txt" );
//     m_Communication->SetCapture( true );

    m_Communication->OpenCommunication();
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

    m_Tracker = m_FlockOfBirdsTracker = FlockOfBirdsTracker::New();
    m_FlockOfBirdsTracker->SetCommunication( m_Communication );

    m_FlockOfBirdsTracker->RequestOpen();
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

    m_TrackerToolList.clear();
    for ( int i=0; i< trackerConfig->m_TrackerToolList.size(); i++)
    {
        FlockOfBirdsTrackerTool::Pointer tool = FlockOfBirdsTrackerTool::New();
        AscensionTrackerToolConfiguration * toolConfig =
           trackerConfig->m_TrackerToolList[i];

        tool->RequestSetBirdName("bird0");

        tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform );
        tool->RequestConfigure();

        tool->RequestAttachToTracker( m_FlockOfBirdsTracker );
        if( this->m_errorObserver->Error() )
        {
            this->m_errorObserver->ClearError();
            return 0;
        }

        TrackerTool::Pointer t = tool.GetPointer();
        m_TrackerToolList.push_back( t );
    }

    m_Tracker->RequestSetFrequency( trackerConfig->m_Frequency );
    m_Tracker->RequestStartTracking();
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

    return 1;
}

#ifdef IGSTKSandbox_USE_MicronTracker
// FIXME: Add error events listener
int TrackerInitializerNew::InitializeMicronTracker()
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
  if( this->m_errorObserver->Error() )
  {
      this->m_errorObserver->ClearError();
      return 0;
  }

  m_TrackerToolList.clear();
  for ( int i=0; i< trackerConfig->m_TrackerToolList.size(); i++)
    {
    MicronTrackerTool::Pointer tool = MicronTrackerTool::New();
    MicronTrackerToolConfiguration * toolConfig = 
      trackerConfig->m_TrackerToolList[i];

    tool->RequestSetMarkerName( toolConfig->m_MarkerName );
    tool->SetCalibrationTransform( toolConfig->m_CalibrationTransform );
    tool->RequestConfigure();

    tool->RequestAttachToTracker( m_MicronTracker );
    if( this->m_errorObserver->Error() )
    {
        this->m_errorObserver->ClearError();
        return 0;
    }

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
  if( this->m_errorObserver->Error() )
  {
      this->m_errorObserver->ClearError();
      return 0;
  }

  return 1;

}

#endif /* IGSTKSandbox_USE_MicronTracker */

/** Destructor */
TrackerInitializerNew::~TrackerInitializerNew()
{
  if (m_TrackerConfiguration)
    {
    delete m_TrackerConfiguration;
    }

}

} // end of namespace
