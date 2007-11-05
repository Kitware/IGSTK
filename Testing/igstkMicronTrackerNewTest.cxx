/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerNewTest.cxx
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
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkMicronTrackerNew.h"
#include "igstkMicronTrackerTool.h"
#include "igstkTransform.h"

class MicronTrackerNewTrackerTestCommand : public itk::Command 
{
public:
  typedef  MicronTrackerNewTrackerTestCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  MicronTrackerNewTrackerTestCommand() {};

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


int igstkMicronTrackerNewTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  MicronTrackerNewTrackerTestCommand::Pointer 
                                my_command = MicronTrackerNewTrackerTestCommand::New();

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


  igstk::MicronTrackerNew::Pointer  tracker;

  tracker = igstk::MicronTrackerNew::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);
  tracker->SetLogger( logger );

  std::string calibrationFilesDirectory = argv[1];
  tracker->SetCameraCalibrationFilesDirectory( 
                            calibrationFilesDirectory );

  std::string initializationFile = argv[2];
  tracker->SetInitializationFile( initializationFile );

  std::string markerTemplateDirectory = argv[3];
  tracker->LoadMarkerTemplate( markerTemplateDirectory );

  // Add tracker tools
  typedef igstk::MicronTrackerToolNew  TrackerToolType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  std::string markerNameTT = "TTblock";
  trackerTool->RequestSetMarkerName( markerNameTT );  
  trackerTool->RequestInitialize();
  trackerTool->RequestAttachToTracker( tracker );

  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  std::string markerNamesPointer = "sPointer";
  trackerTool2->RequestSetMarkerName( markerNamesPointer );  
  trackerTool2->RequestInitialize();
  trackerTool2->RequestAttachToTracker( tracker );

  TrackerToolType::Pointer trackerTool3 = TrackerToolType::New();
  trackerTool3->SetLogger( logger );
  std::string inValidMarkerName = "InValidMarkerName";
  trackerTool3->RequestSetMarkerName( inValidMarkerName );  
  trackerTool3->RequestInitialize();
  trackerTool3->RequestAttachToTracker( tracker );

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
