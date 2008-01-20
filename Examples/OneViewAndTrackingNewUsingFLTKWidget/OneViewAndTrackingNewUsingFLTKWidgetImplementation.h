/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingNewUsingFLTKWidgetImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __OneViewAndTrackingNewUsingFLTKWidgetImplementation_h
#define __OneViewAndTrackingNewUsingFLTKWidgetImplementation_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "OneViewAndTrackingNewUsingFLTKWidgetGUI.h"
#include "igstkPolarisTracker.h"
#include "igstkSerialCommunication.h"
#include "igstkLogger.h"
#include "igstkView.h"
#include "itkStdStreamLogOutput.h"

class OneViewAndTrackingNewUsingFLTKWidgetImplementation : 
  public OneViewAndTrackingNewUsingFLTKWidgetGUI
{
public:

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

  typedef igstk::PolarisTracker             TrackerType;
  typedef igstk::PolarisTrackerTool         TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;
  typedef igstk::SerialCommunication        CommunicationType;

public:

  OneViewAndTrackingNewUsingFLTKWidgetImplementation()
    {
    m_Tracker = TrackerType::New();

    m_Logger = LoggerType::New();
    m_LogOutput = LogOutputType::New();
    m_LogFile.open("logOneViewAndTrackingNewUsingFLTKWidget.txt");
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

    m_Logger->SetPriorityLevel( LoggerType::CRITICAL );
    m_Tracker->SetLogger( m_Logger );

    // set validity time of the tracker transforms
    m_Tracker->SetValidityTime( 1e20 );

    m_Communication = CommunicationType::New();
    m_Communication->SetLogger( m_Logger );
    m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
    m_Communication->SetParity( igstk::SerialCommunication::NoParity );
    m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
    m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
    m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
    m_Communication->SetHardwareHandshake( 
      igstk::SerialCommunication::HandshakeOff );
    m_Tracker->SetCommunication(m_Communication);

    m_Communication->OpenCommunication();

    m_Tracker->RequestOpen();
    
    m_Tracking = false;

    //enable interaction by default
    InteractionButton->set();
    }

  ~OneViewAndTrackingNewUsingFLTKWidgetImplementation()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracker->RequestClose();
    }
  
  LoggerType * GetLogger()
    {
    return m_Logger;
    }

  const TrackerType::Pointer GetTracker()
    {
    return m_Tracker;
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

  void EnableInteraction()
    {
    Display3D->RequestEnableInteractions();
    }
 
  void DisableInteraction()
    {
    Display3D->RequestDisableInteractions();
    }
 
  void AddTool( TrackerToolType * tool)
    {
    tool->RequestConfigure();
    tool->RequestAttachToTracker( m_Tracker );
    }

private:

  LoggerType::Pointer         m_Logger;
  LogOutputType::Pointer      m_LogOutput;
  TrackerType::Pointer        m_Tracker;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;
};

#endif
