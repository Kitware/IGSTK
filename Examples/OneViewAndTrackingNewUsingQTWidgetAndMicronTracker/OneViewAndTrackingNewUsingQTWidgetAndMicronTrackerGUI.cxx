/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI.cxx
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
#include "OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI.moc"
#include "OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI.h"

OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::
OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI()
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

  m_Tracker->RequestOpen();

  m_Tracking = false;
  m_GUIQuit  = false;

  ui.Display3D->SetLogger( m_Logger );

  //By default turn on interaction
  ui.InteractionCheckBox->setCheckState( Qt::Checked );

  //Set up an observer for the transform modified event
  m_ViewPickerObserver = ObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
     &OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::ParsePickedPoint );
}

OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::
~OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI()
{
  m_Tracker->RequestReset();
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
}

void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::AttachObjectToTrackerTool( int trackerToolNumber, 
                             igstk::SpatialObject * objectToTrack )
{
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                    igstk::TimeStamp::GetLongestPossibleTime() );

  // Attach a spatial object to the tracker tool
  if ( trackerToolNumber == 1 )
    {
    objectToTrack->RequestSetTransformAndParent( identityTransform,
                                                 m_TrackerTool );
    }
  else if (trackerToolNumber == 2 )
    {
    objectToTrack->RequestSetTransformAndParent( identityTransform,
                                                 m_TrackerTool2 );
    }
  else
    {
    std::cerr << "Unavailable tracker tool number is specified" << std::endl;
    }
}

 void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::GetTrackerToolTransform( int trackerToolNumber, TransformType & transform )
{
  if ( trackerToolNumber == 1 )
    {
    transform = m_TrackerTool->GetRawTransform();
    }
  else if ( trackerToolNumber == 2)
    {
    transform = m_TrackerTool2->GetRawTransform();
    }
  else
    {
    std::cerr << "Unavailable tracker tool number is specified" << std::endl;
    }
}


void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::CreateActions()
{
  connect(ui.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(ui.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
  connect(ui.InteractionCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnInteractionAction(int)));
}

void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::OnQuitAction()
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

void 
OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::SetView( igstk::View * view )
{
  ui.Display3D->RequestSetView (view);
  view->AddObserver( igstk::TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
}

void 
OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::OnTrackingAction( int state )
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

void
OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::OnInteractionAction( int state )
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

void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::InitializeTracker( std::string InitializationFile,
                     std::string CameraCalibrationFileDirectory,
                     std::string markerTemplateDirectory )
{
  m_Tracker->SetCameraCalibrationFilesDirectory( 
                            CameraCalibrationFileDirectory );

  m_Tracker->SetInitializationFile( InitializationFile );
  m_Tracker->LoadMarkerTemplate( markerTemplateDirectory );
  m_Tracker->RequestOpen();
}

void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::ConfigureTrackerToolsAndAttachToTheTracker()
{
  // Create two tracker tools and attach them to the tracker
  std::cout << " Attaching tracker tool with TTblock marker" << std::endl;
  m_TrackerTool = TrackerToolType::New();
  //m_TrackerTool->SetLogger( m_Logger );
  std::string markerNameTT = "TTblock";
  m_TrackerTool->RequestSetMarkerName( markerNameTT );  
  m_TrackerTool->RequestConfigure();
  m_TrackerTool->RequestAttachToTracker( m_Tracker );

  std::cout << " Attaching tracker tool with sPointer marker" << std::endl;
  m_TrackerTool2 = TrackerToolType::New();
  //m_TrackerTool2->SetLogger( m_Logger );
  std::string markerNamesPointer = "sPointer";
  m_TrackerTool2->RequestSetMarkerName( markerNamesPointer );  
  m_TrackerTool2->RequestConfigure();
  m_TrackerTool2->RequestAttachToTracker( m_Tracker );

}

bool OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI::HasQuitted( ) 
{
  return m_GUIQuit;
}

void OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::AttachTrackerToView( igstk::View * view )
{
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                    igstk::TimeStamp::GetLongestPossibleTime() );

  // Attach a viewer to the tracker 
  m_Tracker->RequestSetTransformAndParent( identityTransform, view );
}

void 
OneViewAndTrackingNewUsingQTWidgetAndMicronTrackerGUI
::ParsePickedPoint( const itk::EventObject & event)
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
