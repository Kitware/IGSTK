/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingMicronTrackerImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __OneViewAndTrackingUsingMicronTrackerImplementation_h
#define __OneViewAndTrackingUsingMicronTrackerImplementation_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "OneViewAndTrackingUsingMicronTrackerGUI.h"
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#include "igstkLogger.h"
#include "igstkView.h"
#include "itkStdStreamLogOutput.h"

#include "igstkTransformObserver.h"

class OneViewAndTrackingUsingMicronTrackerImplementation : 
  public OneViewAndTrackingUsingMicronTrackerGUI
{
public:

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

  typedef igstk::MicronTracker                  TrackerType;
  typedef igstk::MicronTrackerTool              TrackerToolType;
  typedef TrackerToolType::TransformType        TransformType;

public:

  OneViewAndTrackingUsingMicronTrackerImplementation()
    {
    m_Tracker = TrackerType::New();

    m_Logger = LoggerType::New();
    m_LogOutput = LogOutputType::New();
    m_LogFile.open("logOneViewAndTrackingUsingMicronTracker.txt");
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

    m_Tracking = false;

    // connect a logger to the view
    Display3D->SetLogger( m_Logger );
    }

  void InitializeTracker( std::string InitializationFile,
                          std::string CameraCalibrationFileDirectory,
                          std::string markerTemplateDirectory )
    {
    m_Tracker->SetCameraCalibrationFilesDirectory( 
                              CameraCalibrationFileDirectory );

    m_Tracker->SetInitializationFile( InitializationFile );
    m_Tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );
    m_Tracker->RequestOpen();
    }

  void ConfigureTrackerToolsAndAttachToTheTracker()
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

    //enable interaction by default
    InteractionButton->set();
    }

  ~OneViewAndTrackingUsingMicronTrackerImplementation()
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

  bool IsTrackingTurnedOn()
    {
    return m_Tracking;
    }
  
  void DisableTracking()
    {
    m_Tracker->RequestReset();
    m_Tracker->RequestStopTracking();
    m_Tracking = false;
    }

  LoggerType * GetLogger()
    {
    return m_Logger;
    }

  void EnableInteraction()
    {
    Display3D->RequestEnableInteractions();
    }
 
  void DisableInteraction()
    {
    Display3D->RequestDisableInteractions();
    }

  void AttachTrackerToView( igstk::View * view )
    {
    TransformType identityTransform;
    identityTransform.SetToIdentity( 
                      igstk::TimeStamp::GetLongestPossibleTime() );
   
    // Attach a viewer to the tracker 
    view->RequestSetTransformAndParent( identityTransform, 
                                        m_Tracker );
    }


  void AttachObjectToTrackerTool( int trackerToolNumber,
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

  void GetTrackerToolTransform( int trackerToolNumber,
                                TransformType & transform )
    {
    typedef igstk::TransformObserver   ObserverType;
  
    if ( trackerToolNumber == 1 )
      {
      ObserverType::Pointer coordSystemAObserver = ObserverType::New();
      coordSystemAObserver->ObserveTransformEventsFrom( m_TrackerTool );

      coordSystemAObserver->Clear();
      m_TrackerTool->RequestGetTransformToParent();
      if (coordSystemAObserver->GotTransform())
        {
        transform = coordSystemAObserver->GetTransform();
        }
      }
    else if ( trackerToolNumber == 2)
      {
      ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
      coordSystemAObserver2->ObserveTransformEventsFrom( m_TrackerTool2 );

      coordSystemAObserver2->Clear();
      m_TrackerTool2->RequestGetTransformToParent();
      if (coordSystemAObserver2->GotTransform())
        {
        transform = coordSystemAObserver2->GetTransform();
        }
      }
    else
      {
      std::cerr << "Unavailable tracker tool number is specified" << std::endl;
      }
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
