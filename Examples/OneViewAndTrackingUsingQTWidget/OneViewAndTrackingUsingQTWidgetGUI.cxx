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
  ui.setupUi(this);
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
  m_Tracker->SetScaleFactor( 100.0 );

  m_Tool->RequestSetMouseName("Mouse1");
  m_Tool->RequestConfigure();
  m_Tool->RequestAttachToTracker( m_Tracker );

  m_Tracking = false;
  m_GUIQuit  = false;

  ui.Display3D->SetLogger( m_Logger );

  //By default turn on interaction
  ui.InteractionCheckBox->setCheckState( Qt::Checked );

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

void OneViewAndTrackingUsingQTWidgetGUI::CreateActions()
{
  connect(ui.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(ui.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
  connect(ui.InteractionCheckBox, SIGNAL(stateChanged(int )), this,
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
  ui.Display3D->RequestSetView (view);
  ui.Display3D->AddObserver( igstk::TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
}

void OneViewAndTrackingUsingQTWidgetGUI::OnTrackingAction( int state )
{
  if ( state )
    {
    m_Tracking = true;
    m_Tracker->RequestStartTracking();
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
    ui.Display3D->RequestEnableInteractions();
    }
  else
    {
    ui.Display3D->RequestDisableInteractions();
    } 
}


void 
OneViewAndTrackingUsingQTWidgetGUI::
AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  objectToTrack->RequestSetTransformAndParent( transform, m_Tool.GetPointer() );
}

bool OneViewAndTrackingUsingQTWidgetGUI::HasQuitted( ) 
{
  return m_GUIQuit;
}

void OneViewAndTrackingUsingQTWidgetGUI::ParsePickedPoint( const itk::EventObject & event)
{
  if ( igstk::TransformModifiedEvent().CheckEvent( &event ) )
    {
    igstk::TransformModifiedEvent *tmevent = 
                                     ( igstk::TransformModifiedEvent *) & event;
    
    std::cout << "Translation transform: " << tmevent->Get().GetTranslation()[0]
                                           << tmevent->Get().GetTranslation()[1]
                                           << tmevent->Get().GetTranslation()[2]
                                           << std::endl;
    }
}


