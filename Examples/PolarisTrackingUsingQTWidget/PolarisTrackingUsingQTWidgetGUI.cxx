/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PolarisTrackingUsingQTWidgetGUI.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <QtGui>
#include "PolarisTrackingUsingQTWidgetGUI.moc"
#include "PolarisTrackingUsingQTWidgetGUI.h"

PolarisTrackingUsingQTWidgetGUI::PolarisTrackingUsingQTWidgetGUI(int portNo, const std::string & romFile)
{
  m_GUI.setupUi(this);
  this->CreateActions();

  m_Tracker = TrackerType::New();
  m_TrackerTool = TrackerToolType::New();

  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();
  m_LogFile.open("logPolarisAndTrackingUsingQtWidget.txt");
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
  m_Communication->SetPortNumber( ( igstk::SerialCommunication::PortNumberType ) portNo );
  m_Communication->SetParity( igstk::SerialCommunication::NoParity );
  m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake(
    igstk::SerialCommunication::HandshakeOff );
  m_Communication->SetCaptureFileName( "RecordedStreamByPolarisTrackerUsingQtWidget.txt" );
  m_Communication->SetCapture( true );
  std::cout << "SetCommunication()" << std::endl;
  m_Tracker->SetCommunication(m_Communication);

  if ( !m_Communication->OpenCommunication())
  {
    std::cout << "Serial port open failure\n";
  }
  std::cout << "RequestOpen()" << std::endl;
  m_Tracker->RequestOpen();

  // instantiate and attach wireless tracker tool
  std::cout << "Instantiate tracker tool: " << std::endl;
  m_TrackerTool->SetLogger( m_Logger );
  //Select wireless tracker tool
  m_TrackerTool->RequestSelectWirelessTrackerTool();
  //Set the SROM file
  m_TrackerTool->RequestSetSROMFileName( romFile );
  //Configure
  m_TrackerTool->RequestConfigure();
  //Attach to the tracker
  std::cout << "RequestAttachToTracker()" << std::endl;
  m_TrackerTool->RequestAttachToTracker( m_Tracker );


  m_Tracking = false;
  m_GUIQuit  = false;

  m_GUI.Display3D->SetLogger( m_Logger );

  //By default turn on interaction
  m_GUI.InteractionCheckBox->setCheckState( Qt::Checked );
}

PolarisTrackingUsingQTWidgetGUI::~PolarisTrackingUsingQTWidgetGUI()
{
  m_Tracker->RequestReset();
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
}

PolarisTrackingUsingQTWidgetGUI::TrackerType::Pointer
PolarisTrackingUsingQTWidgetGUI::GetTracker()
{
  return m_Tracker;
}

void PolarisTrackingUsingQTWidgetGUI::CreateActions()
{
  connect(m_GUI.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(m_GUI.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
  connect(m_GUI.InteractionCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnInteractionAction(int)));
}

void PolarisTrackingUsingQTWidgetGUI::OnQuitAction()
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

void PolarisTrackingUsingQTWidgetGUI::SetView( igstk::View * view )
{
  m_GUI.Display3D->RequestSetView (view);
}

void PolarisTrackingUsingQTWidgetGUI::OnTrackingAction( int state )
{
  if ( state )
    {
    m_Tracking = true;
    m_Tracker->RequestStartTracking();
    igstk::Transform transform;
    transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
    m_ObjectToTrack->RequestSetTransformAndParent( transform, m_TrackerTool );
    }
  else
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    }
}

void PolarisTrackingUsingQTWidgetGUI::OnInteractionAction( int state )
{
  if ( state )
    {
    m_GUI.Display3D->RequestEnableInteractions();
    }
  else
    {
    m_GUI.Display3D->RequestDisableInteractions();
    }
}

void
PolarisTrackingUsingQTWidgetGUI::
AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  m_ObjectToTrack = objectToTrack;
  m_ObjectToTrack->RequestSetTransformAndParent( transform, m_Tracker );
}

bool PolarisTrackingUsingQTWidgetGUI::HasQuitted( )
{
  return m_GUIQuit;
}
