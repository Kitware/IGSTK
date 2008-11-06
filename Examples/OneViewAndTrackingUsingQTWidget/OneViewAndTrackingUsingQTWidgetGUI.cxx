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
#include <QtGui>
#include "OneViewAndTrackingUsingQTWidgetGUI.moc"
#include "OneViewAndTrackingUsingQTWidgetGUI.h"

OneViewAndTrackingUsingQTWidgetGUI::OneViewAndTrackingUsingQTWidgetGUI()
{
  m_GUI.setupUi(this);
  this->CreateActions();

  m_Tracker = TrackerType::New();
  m_Tool = TrackerToolType::New();

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

  m_Tracker->RequestOpen();
  m_Tracker->SetScaleFactor( 2.0 );

  m_Tool->RequestSetMouseName("Mouse1");
  m_Tool->RequestConfigure();
  m_Tool->RequestAttachToTracker( m_Tracker );

  m_Tracking = false;
  m_GUIQuit  = false;

  m_GUI.Display3D->SetLogger( m_Logger );

  //By default turn on interaction
  m_GUI.InteractionCheckBox->setCheckState( Qt::Checked );

  //Set up an observer for the transform modified event
  m_ViewPickerObserver = ObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this,
                        &OneViewAndTrackingUsingQTWidgetGUI::ParsePickedPoint );
}

OneViewAndTrackingUsingQTWidgetGUI::~OneViewAndTrackingUsingQTWidgetGUI()
{
  m_Tracker->RequestReset();
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
}

OneViewAndTrackingUsingQTWidgetGUI::TrackerType::Pointer
OneViewAndTrackingUsingQTWidgetGUI::GetTracker()
{
  return m_Tracker;
}

void OneViewAndTrackingUsingQTWidgetGUI::CreateActions()
{
  connect(m_GUI.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(m_GUI.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
  connect(m_GUI.InteractionCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnInteractionAction(int)));
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

void OneViewAndTrackingUsingQTWidgetGUI::SetView( igstk::View * view )
{
  m_GUI.Display3D->RequestSetView (view);
  view->AddObserver( igstk::CoordinateSystemTransformToEvent(),
                                   m_ViewPickerObserver );
}

void OneViewAndTrackingUsingQTWidgetGUI::OnTrackingAction( int state )
{
  if ( state )
    {
    m_Tracking = true;
    m_Tracker->RequestStartTracking();

    igstk::Transform transform;
    transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
    m_ObjectToTrack->RequestSetTransformAndParent( transform, m_Tool );
    }
  else
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    }
}

void OneViewAndTrackingUsingQTWidgetGUI::OnInteractionAction( int state )
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
OneViewAndTrackingUsingQTWidgetGUI::
AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  m_ObjectToTrack = objectToTrack;
  m_ObjectToTrack->RequestSetTransformAndParent( transform, m_Tracker );
}

bool OneViewAndTrackingUsingQTWidgetGUI::HasQuitted( )
{
  return m_GUIQuit;
}

void
OneViewAndTrackingUsingQTWidgetGUI
::ParsePickedPoint( const itk::EventObject & event)
{
  typedef igstk::CoordinateSystemTransformToEvent ExpectedEvent;
  if ( ExpectedEvent().CheckEvent( &event ) )
    {
    const ExpectedEvent * tmevent =
      dynamic_cast< const ExpectedEvent *>( & event );

    const igstk::CoordinateSystemTransformToResult transformCarrier
      = tmevent->Get(); 

    const igstk::Transform transform = transformCarrier.GetTransform();

    std::cout << "Translation transform:"
              << transform.GetTranslation()[0] << "\t"
              << transform.GetTranslation()[1] << "\t"
              << transform.GetTranslation()[2]
              << std::endl;
    }
}
