/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkArucoTrackerSimulatedTest.cxx
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

#include "igstkSystemInformation.h"
#include "igstkArucoTracker.h"
#include "igstkArucoTrackerTool.h"
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

class ArucoTrackerTestCommand : public itk::Command 
{
public:
  typedef  ArucoTrackerTestCommand    Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  ArucoTrackerTestCommand() {};

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

int igstkArucoTrackerSimulatedTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Simulation_filename "
                            << "Camera_Calibration_file "
                            << std::endl;
    return EXIT_FAILURE;
    }

  double meanPosition[3];
  meanPosition[0] = 5.8188;
  meanPosition[1] = -22.4492;
  meanPosition[2] = 211.2865;
  double threshold = 1;

  ArucoTrackerTestCommand::Pointer 
                                my_command = ArucoTrackerTestCommand::New();

  std::string filename = argv[1];
  std::cout << "Logger output saved here:\n";
  std::cout << filename << "\n"; 

  std::ofstream loggerFile;
  loggerFile.open( filename.c_str() );
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( loggerFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  std::string simulationFile = argv[2];
  std::cout << "Tracker simulation file: " 
           << simulationFile << std::endl;

  std::string cameraCalibrationFile = argv[3];
  std::cout << "Tracker calibration file: "
            << cameraCalibrationFile << std::endl;


  igstk::ArucoTracker::Pointer  tracker;

  tracker = igstk::ArucoTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetCameraParametersFromYAMLFile(cameraCalibrationFile);

  tracker->SetMarkerSize(50);

  tracker->RequestSetFrequency(30);
  
  tracker->SetSimulationVideo(simulationFile);

  tracker->SetLogger( logger );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::ArucoTrackerTool          TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  
  // instantiate and attach 5DOF tracker tool type 
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Set marker type
  trackerTool->RequestSetMarkerName(102);
  //Configure
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
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
    igstk::PulseGenerator::Sleep( 10 );
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
        std::cout << "Trackertool transform using observer:" 
              << trackerTool->GetTrackerToolIdentifier()
              << "\t\t  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;
        std::ostringstream message;

        // check position of marker
        if( (position[0] - meanPosition[0]) > threshold ||
            (position[1] - meanPosition[1]) > threshold ||
            (position[2] - meanPosition[2]) > threshold )
        {
          return EXIT_FAILURE;
        }
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
