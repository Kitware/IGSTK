/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTrackerTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkInfiniTrackTracker.h"
#include "igstkInfiniTrackTrackerTool.h"
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

class InfiniTrackTrackerTrackerTestCommand : public itk::Command 
{
public:
  typedef  InfiniTrackTrackerTrackerTestCommand   Self;
  typedef  itk::Command                      Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  itkNewMacro( Self );
protected:
  InfiniTrackTrackerTrackerTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};


int igstkInfiniTrackTrackerTest( int argc, char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  InfiniTrackTrackerTrackerTestCommand::Pointer 
                     my_command = InfiniTrackTrackerTrackerTestCommand::New();

  if( argc < 2 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Marker_template_directory " << "\t"
                            << "Logger_Output_filename" << std::endl;
    return EXIT_FAILURE;
    }

  std::string filename = argv[2];
  std::cout << "Logger output saved here:\n";
  std::cout << filename << "\n"; 

  std::ofstream loggerFile;
  loggerFile.open( filename.c_str() );
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( loggerFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  typedef igstk::TransformObserver   ObserverType;
 
  igstk::InfiniTrackTracker::Pointer  tracker;

  tracker = igstk::InfiniTrackTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);
  tracker->SetLogger( logger );

  // Set necessary parameters of the tracker
  std::string markerTemplateDirectory = argv[1];
  tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );


  // Start communication
  tracker->RequestOpen();

  // Add tracker tools
  typedef igstk::InfiniTrackTrackerTool  TrackerToolType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  std::string markerNameTT = "ISTB0025";
  trackerTool->RequestSetMarkerName( markerNameTT );  
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );


  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  std::string markerNamesPointer = "ISTB0030";
  trackerTool2->RequestSetMarkerName( markerNamesPointer );  
  trackerTool2->RequestConfigure();
  trackerTool2->RequestAttachToTracker( tracker );
  trackerTool2->AddObserver( itk::AnyEvent(), my_command);
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
  coordSystemAObserver2->ObserveTransformEventsFrom( trackerTool2 );


  //start tracking 
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<100; i++)
    {
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

    coordSystemAObserver2->Clear();
    trackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
      {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() )
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool2 :" 
                << trackerTool2->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
 
    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  //Remove one of the tracker tools and restart tracking
  std::cout << "Detach the tracker tool from the tracker" << std::endl;
  trackerTool->RequestDetachFromTracker( );

  // restart tracking

  tracker->RequestStartTracking();

  for(unsigned int i=0; i<100; i++)
    {
    igstk::PulseGenerator::CheckTimeouts(); 

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver2->Clear();
    trackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
      {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() )
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool2 :" 
                << trackerTool2->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
    }

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
