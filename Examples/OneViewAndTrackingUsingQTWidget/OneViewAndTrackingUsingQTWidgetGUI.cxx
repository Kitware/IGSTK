/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidgetGUI.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#
#include <QtGui>
#include "OneViewAndTrackingUsingQTWidgetGUI.moc"
#include "OneViewAndTrackingUsingQTWidgetGUI.h"

OneViewAndTrackingUsingQTWidgetGUI::OneViewAndTrackingUsingQTWidgetGUI()
{
  ui.setupUi(this);
  this->CreateActions();

  m_Tracker = TrackerType::New();

  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();
  m_LogFile.open("logOneViewAndTrackingUsingFLTKWidget.txt");
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

  m_Tracking = false;
  m_GUIQuit  = false;

  ui.Display3D->SetLogger( m_Logger );
  
}

OneViewAndTrackingUsingQTWidgetGUI::~OneViewAndTrackingUsingQTWidgetGUI()
{
  m_Tracker->RequestReset();
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
}

void OneViewAndTrackingUsingQTWidgetGUI::CreateActions()
{
  connect(ui.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(ui.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
}

void OneViewAndTrackingUsingQTWidgetGUI::OnQuitAction()
{
  QMessageBox::StandardButton value = 
    QMessageBox::information(this,
    "Tracking application", "Are you sure you want to quit ?",
    QMessageBox::Yes | QMessageBox::No );

  if( value == QMessageBox::Yes )
    {
    this->close();
    m_GUIQuit = true;
    }
}

void OneViewAndTrackingUsingQTWidgetGUI::SetView( igstk::ViewNew::Pointer view )
{
  ui.Display3D->RequestSetView (view);
}

void OneViewAndTrackingUsingQTWidgetGUI::OnTrackingAction( int state )
{
  if ( state )
    {
    std::cout << "Tracker is enabled: " << std::endl;
    m_Tracking = true;
    m_Tracker->RequestStartTracking();
    }
  else
    {
    std::cout << "Tracker is disabled: " << std::endl;
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    } 
}

void 
OneViewAndTrackingUsingQTWidgetGUI::
AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;
  m_Tracker->AttachObjectToTrackerTool( 
      toolPort, toolNumber, objectToTrack );
}

bool OneViewAndTrackingUsingQTWidgetGUI::HasQuitted( ) 
{
  return m_GUIQuit;
}
