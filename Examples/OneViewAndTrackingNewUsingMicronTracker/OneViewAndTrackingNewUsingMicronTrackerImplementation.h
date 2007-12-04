/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingNewUsingMicronTrackerImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __OneViewAndTrackingNewUsingMicronTrackerImplementation_h
#define __OneViewAndTrackingNewUsingMicronTrackerImplementation_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "OneViewAndTrackingNewUsingMicronTrackerGUI.h"
#include "igstkMicronTrackerNew.h"
#include "igstkLogger.h"
#include "igstkViewNew.h"
#include "itkStdStreamLogOutput.h"

class OneViewAndTrackingNewUsingMicronTrackerImplementation : 
  public OneViewAndTrackingNewUsingMicronTrackerGUI
{
public:

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;

  typedef igstk::MicronTrackerNew     TrackerType;
  typedef igstk::MicronTrackerToolNew TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

public:

  OneViewAndTrackingNewUsingMicronTrackerImplementation()
    {
    m_Tracker = TrackerType::New();

    m_Logger = LoggerType::New();
    m_LogOutput = LogOutputType::New();
    m_LogFile.open("logOneViewAndTrackingNewUsingMicronTracker.txt");
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

    // set validity time of the tracker transforms
    m_Tracker->SetValidityTime( 1e20 );

    m_Tracking = false;
    }

  void InitializeTracker( std::string InitializationFile, std::string CameraCalibrationFileDirectory, std::string markerTemplateDirectory )
    {
    m_Tracker->SetCameraCalibrationFilesDirectory( 
                              CameraCalibrationFileDirectory );

    m_Tracker->SetInitializationFile( InitializationFile );
    m_Tracker->LoadMarkerTemplate( markerTemplateDirectory );
    m_Tracker->RequestOpen();
    }

  void ConfigureTrackerToolsAndAttachToTheTracker()
    {
    // Create two tracker tools and attach them to the tracker
    m_TrackerTool = TrackerToolType::New();
    m_TrackerTool->SetLogger( m_Logger );
    std::string markerNameTT = "TTblock";
    m_TrackerTool->RequestSetMarkerName( markerNameTT );  
    m_TrackerTool->RequestConfigure();
    m_TrackerTool->RequestAttachToTracker( m_Tracker );

    m_TrackerTool2 = TrackerToolType::New();
    m_TrackerTool2->SetLogger( m_Logger );
    std::string markerNamesPointer = "sPointer";
    m_TrackerTool2->RequestSetMarkerName( markerNamesPointer );  
    m_TrackerTool2->RequestConfigure();
    m_TrackerTool2->RequestAttachToTracker( m_Tracker );

    //enable interaction by default
    InteractionButton->set();    
    }

  ~OneViewAndTrackingNewUsingMicronTrackerImplementation()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracker->RequestClose();
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

  void AttachTrackerToView( igstk::ViewNew * view )
    {
    TransformType identityTransform;
    identityTransform.SetToIdentity( 
                      igstk::TimeStamp::GetLongestPossibleTime() );
   
    // Attach a viewer to the tracker 
    m_Tracker->RequestSetTransformAndParent( identityTransform, view );
    }


  void AttachObjectToTrackerTool( int trackerToolNumber, igstk::SpatialObject * objectToTrack )
    {
    TransformType identityTransform;
    identityTransform.SetToIdentity( 
                      igstk::TimeStamp::GetLongestPossibleTime() );
   
    // Attach a spatial object to the tracker tool
    if ( trackerToolNumber == 1 )
      {
      objectToTrack->RequestSetTransformAndParent( identityTransform, m_TrackerTool.GetPointer() );
      }
    else if (trackerToolNumber == 2 )
      {
      objectToTrack->RequestSetTransformAndParent( identityTransform, m_TrackerTool2.GetPointer() );
      }
    else
      {
      std::cerr << "Unavailable tracker tool number is specified" << std::endl;
      }
    }

  void GetTrackerToolTransform( TransformType & transform )
    {
    m_Tracker->GetToolTransform( m_TrackerTool->GetTrackerToolIdentifier(), transform ); 
    }

private:

  LoggerType::Pointer         m_Logger;
  LogOutputType::Pointer      m_LogOutput;
  TrackerType::Pointer        m_Tracker;
  TrackerToolType::Pointer    m_TrackerTool;
  TrackerToolType::Pointer    m_TrackerTool2;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;
};

#endif
