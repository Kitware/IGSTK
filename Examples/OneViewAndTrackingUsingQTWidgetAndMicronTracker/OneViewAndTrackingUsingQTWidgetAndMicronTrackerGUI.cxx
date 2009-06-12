/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.cxx
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
#include "OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.moc"
#include "OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.h"

#include "igstkCoordinateSystemTransformToResult.h"

OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::
OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI()
{
  m_GUI.setupUi(this);
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

  m_GUI.Display3D->SetLogger( m_Logger );

  //By default turn on interaction
  m_GUI.InteractionCheckBox->setCheckState( Qt::Checked );

  //Set up an observer for the transform modified event
  m_ViewPickerObserver = ObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
     &OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::ParsePickedPoint );
}

OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::
~OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI()
{
  m_Tracker->RequestReset();
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
}

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
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

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::CreateActions()
{
  connect(m_GUI.QuitPushButton, SIGNAL(clicked()), this, SLOT(OnQuitAction()));
  connect(m_GUI.TrackingCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnTrackingAction(int)));
  connect(m_GUI.InteractionCheckBox, SIGNAL(stateChanged(int )), this,
          SLOT(OnInteractionAction(int)));
}

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::OnQuitAction()
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
OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
::SetView( igstk::View * view )
{
  m_GUI.Display3D->RequestSetView (view);
  view->AddObserver( igstk::CoordinateSystemTransformToEvent(), 
                                   m_ViewPickerObserver );
}

void 
OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
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
OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
::OnInteractionAction( int state )
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

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
::InitializeTracker( std::string InitializationFile,
                     std::string CameraCalibrationFileDirectory,
                     std::string markerTemplateDirectory )
{
  m_Tracker->SetCameraCalibrationFilesDirectory( 
                            CameraCalibrationFileDirectory );

  m_Tracker->SetInitializationFile( InitializationFile );
  m_Tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );
  m_Tracker->RequestOpen();
}

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
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

bool OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI::HasQuitted( ) 
{
  return m_GUIQuit;
}

void OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
::AttachTrackerToView( igstk::View * view )
{
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                    igstk::TimeStamp::GetLongestPossibleTime() );

  // Attach a viewer to the tracker 
  m_Tracker->RequestSetTransformAndParent( identityTransform, view );
}

void 
OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI
::ParsePickedPoint( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
    {
    const igstk::CoordinateSystemTransformToEvent * tmevent = 
      dynamic_cast< const igstk::CoordinateSystemTransformToEvent *>( & event );
    
    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();

    const igstk::Transform transform = transformCarrier.GetTransform();

    std::cout << "Translation transform: " << transform.GetTranslation()[0]
                                           << transform.GetTranslation()[1]
                                           << transform.GetTranslation()[2]
                                           << std::endl;
    }
}
