/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerTest.cxx
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

#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#include "igstkTransform.h"

class MicronTrackerTrackerTestCommand : public itk::Command 
{
public:
  typedef  MicronTrackerTrackerTestCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  MicronTrackerTrackerTestCommand() {};

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


int igstkMicronTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  MicronTrackerTrackerTestCommand::Pointer 
                                my_command = MicronTrackerTrackerTestCommand::New();

  if( argc < 5 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "MicronTracker camera calibration file"
                            << "MicronTracker initialization file"
                            << "Marker template directory "
                            << "Output directory" << std::endl;
    return EXIT_FAILURE;
    }

  /*
  std::string outputDirectory = argv[4];
  std::string testName = argv[0];
  std::string filename = outputDirectory +"/";
  filename = filename + testName;
  filename = filename + "LoggerOutput.txt";
  std::cout << "Logger output saved here:\n";
  std::cout << filename << "\n"; 

  std::ofstream loggerFile;
  loggerFile.open( filename.c_str() );
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( loggerFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);
  */

  /* dump debug information to the standard output */
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);


  igstk::MicronTracker::Pointer  tracker;

  tracker = igstk::MicronTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);
  //tracker->SetLogger( logger );

  std::string calibrationFilesDirectory = argv[1];
  tracker->SetCameraCalibrationFilesDirectory( 
                            calibrationFilesDirectory );

  std::string initializationFile = argv[2];
  tracker->SetInitializationFile( initializationFile );

  std::string markerTemplateDirectory = argv[3];
  tracker->LoadMarkerTemplate( markerTemplateDirectory );

  // Add tracker tools
  typedef igstk::MicronTrackerTool  MicronTrackerToolType;

  // Add a tracker tool with "TTblock" marker type attached to it.
  MicronTrackerToolType::Pointer tool1 = MicronTrackerToolType::New();
  std::string markerNameTT = "TTblock";
  tool1->RequestSetMarkerName( markerNameTT );  
  tracker->RequestAddTool( tool1 );

  // Add a tracker tool with "a" marker type attached to it.
  MicronTrackerToolType::Pointer tool2 = MicronTrackerToolType::New();
  std::string markerNameA = "a";
  tool2->RequestSetMarkerName( markerNameA );  
  tracker->RequestAddTool( tool2 );

  // Add a tracker tool with "sPointer" marker type attached to it.
  MicronTrackerToolType::Pointer tool3 = MicronTrackerToolType::New();
  std::string markerNamePointer = "sPointer";
  tool3->RequestSetMarkerName( markerNamePointer );  
  tracker->RequestAddTool( tool3 );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  std::cout << "RequestInitialize()" << std::endl;
  tracker->RequestInitialize();

  std::cout << tracker << std::endl;

  std::cout << "GetNumberOfTools()" << std::endl;
  unsigned int ntools = tracker->GetNumberOfTools();

  std::cout << "NumberOfTools : " << ntools << std::endl;

  std::cout << "RequestStartTracking()" << std::endl;
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<500; i++)
    {
    tracker->RequestUpdateStatus();
    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
