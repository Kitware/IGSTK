/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "itkStdStreamLogOutput.h"
#include "igstkRealTimeClock.h"
#include "igstkSystemInformation.h"
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#include "igstkTrackerToolObserverToOpenIGTLinkRelay.h"
#include "igstkTransformObserver.h"

int igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest( int argc, char * argv [] )
{

  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " hostname portnumber numberOfTransformsToSend MicronTracker_Camera_Calibration_file "
                         << "MicronTracker_initialization_file Marker_template_directory" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::RealTimeClock::Initialize();

  typedef igstk::MicronTracker                         TrackerType;
  typedef igstk::MicronTrackerTool                     TrackerToolType;
  typedef igstk::TrackerToolObserverToOpenIGTLinkRelay      ObserverToSocketRelayType;

  typedef igstk::TransformObserver   ObserverType;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  // Set necessary parameters of the tracker
  TrackerType::Pointer      tracker                   = TrackerType::New();
  tracker->SetLogger( logger );
  std::string calibrationFilesDirectory = argv[4];
  tracker->SetCameraCalibrationFilesDirectory( 
                            calibrationFilesDirectory );

  std::string initializationFile = argv[5];
  tracker->SetInitializationFile( initializationFile );

  std::string markerTemplateDirectory = argv[6];
  tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );

  // Start communication
  tracker->RequestOpen();

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  std::string markerNameTT = "sPointer";
  trackerTool->RequestSetMarkerName( markerNameTT );  
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );
 //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  ObserverToSocketRelayType::Pointer     toolObserver = 
                                              ObserverToSocketRelayType::New();

  toolObserver->RequestSetTrackerTool( trackerTool );
  toolObserver->RequestSetHostName( argv[1] );
  toolObserver->RequestSetPort( atoi( argv[2] ) );
  toolObserver->RequestStart();

  tracker->RequestStartTracking();

  const unsigned int numberOfTransformsToSend = atoi( argv[3] );

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for( unsigned int i = 0; i < numberOfTransformsToSend; i++ )
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
                << trackerTool->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
    }

  tracker->RequestStopTracking();
  tracker->RequestReset();
  tracker->RequestClose();

  return EXIT_SUCCESS;
}
