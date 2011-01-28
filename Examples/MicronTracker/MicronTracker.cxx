/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    MicronTracker.cxx
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

// BeginLatex
// 
// This example illustrates how to use IGSTK's Tracker component to communicate 
// with Micron Tracker and gather tool transform information. MicronTracker is a
// portable optical tracker that uses video image analysis techniques for
// localization and tracking of surgical tools.
//
// EndLatex


#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

// BeginLatex
// 
// To communicate with MicronTracker tracking device, include the MicronTracker
// header files \doxygen{MicronTracker} and \doxygen{MicronTrackerTool}
//
// EndLatex

// BeginCodeSnippet
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
// EndCodeSnippet
//
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

// BeginLatex
// Define a callback class to listen to events invoked by the tracker class. 
// EndLatex 
//
// BeginCodeSnippet
class MicronTrackerTrackerCommand : public itk::Command 
{
public:
  typedef  MicronTrackerTrackerCommand   Self;
  typedef  itk::Command                      Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  itkNewMacro( Self );
protected:
  MicronTrackerTrackerCommand() {};

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

//EndCodeSnippet

int main( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

//BeginLatex
//Instantiate tracker command callback object.
//EndLatex
//
//
//BeginCodeSnippet
  MicronTrackerTrackerCommand::Pointer 
                     my_command = MicronTrackerTrackerCommand::New();
//EndCodeSnippet

  if( argc < 5 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "MicronTracker_Camera_Calibration_file" << "\t"
                            << "MicronTracker_initialization_file"  << "\t"
                            << "Marker_template_directory " << "\t"
                            << "Logger_Output_filename" << std::endl;
    return EXIT_FAILURE;
    }

  std::string filename = argv[4];
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
 
  igstk::MicronTracker::Pointer  tracker;

  tracker = igstk::MicronTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);
  tracker->SetLogger( logger );

  // Set necessary parameters of the tracker
 
  //BeginLatex
  //Next, set the necessary paramters for the tracker. The required parameters are directory
  //that contains camera calibration files, micron tracker initialization file (.ini) and
  //marker template directory.
  //EndLatex 
  //
  //BeginCodeSnippet
  std::string calibrationFilesDirectory = argv[1];
  tracker->SetCameraCalibrationFilesDirectory( 
                            calibrationFilesDirectory );

  std::string initializationFile = argv[2];
  tracker->SetInitializationFile( initializationFile );

  std::string markerTemplateDirectory = argv[3];
  tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );
  //EndCodeSnippet


  //BeginLatex
  //Next, request tracker communication be opened.
  //EndLatex
  //BeginCodeSnippet
  tracker->RequestOpen();
  //EndCodeSnippet

  //BeginLatex
  //Next, add tracker tools to the tracker.
  //EndLatex
  //
  //BeginCodeSnippet
  typedef igstk::MicronTrackerTool  TrackerToolType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  std::string markerNameTT = "TTblock";
  trackerTool->RequestSetMarkerName( markerNameTT );  
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );


  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  std::string markerNamesPointer = "sPointer";
  trackerTool2->RequestSetMarkerName( markerNamesPointer );  
  trackerTool2->RequestConfigure();
  trackerTool2->RequestAttachToTracker( tracker );
  trackerTool2->AddObserver( itk::AnyEvent(), my_command);
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
  coordSystemAObserver2->ObserveTransformEventsFrom( trackerTool2 );
  //EndCodeSnippet


  //BeginLatex
  // Start tracking and gather the tracker tool transforms.
  //EndLatex

  //BeginCodeSnippet
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
  //EndCodeSnippet
  //
  //BeginLatex
  //Stop tracking and close the communication with the tracker.
  //EndLatex
  //
  //
  //BeginCodeSnippet
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();
  //EndCodeSnippet

  return EXIT_SUCCESS;
}
