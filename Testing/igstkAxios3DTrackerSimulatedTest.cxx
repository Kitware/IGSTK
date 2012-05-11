/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxios3DTrackerSimulatedTest.cxx
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

#include "igstkAxios3DTracker.h"
#include "igstkAxios3DTrackerTool.h"
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

class Axios3DTrackerTestCommand : public itk::Command
{
public:
  typedef  Axios3DTrackerTestCommand   Self;
  typedef  itk::Command                      Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  itkNewMacro( Self );
protected:
  Axios3DTrackerTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
    if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};


int igstkAxios3DTrackerSimulatedTest( int argc, char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  Axios3DTrackerTestCommand::Pointer
                     my_command = Axios3DTrackerTestCommand::New();

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t"
		                    << "Axios3DTrackerTool configuration xml" << "\t"
                            << "Axios3DTracker simulation files directory" << "\t"
                            << "Logger_Output_filename" << std::endl;
    return EXIT_FAILURE;
    }

  std::string filename = argv[3];
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

  igstk::Axios3DTracker::Pointer  tracker;

  tracker = igstk::Axios3DTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);
  tracker->SetLogger( logger );

  // Set necessary parameters of the tracker
  tracker->RequestLoadLocatorsXML(argv[1]);
  tracker->setVirtualMode(true);
  tracker->setPath(argv[2]);

  // Start communication
  tracker->RequestOpen();

  // Add tracker tools
  typedef igstk::Axios3DTrackerTool  TrackerToolType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  std::string locatorName = "DIGITIZED";
  trackerTool->RequestSetMarkerName( locatorName );
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  //start tracking
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<50; i++)
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
    }

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
