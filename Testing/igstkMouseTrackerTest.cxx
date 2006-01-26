/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
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

#include "igstkSystemInformation.h"
#include "igstkMouseTracker.h"
#include "igstkTransform.h"

class MouseTrackerTestCommand : public itk::Command 
{
public:
  typedef  MouseTrackerTestCommand   Self;
  typedef  itk::Command               Superclass;
  typedef itk::SmartPointer<Self>     Pointer;
  itkNewMacro( Self );
protected:
  MouseTrackerTestCommand() {};

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

int igstkMouseTrackerTest( int argc, char * argv[] )
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  MouseTrackerTestCommand::Pointer my_command = MouseTrackerTestCommand::New();

  // logger object created 
  std::string testName;
  if (argc > 0)
    {
    testName = argv[0];
    }

  std::string outputDirectory = IGSTK_TEST_OUTPUT_DIR;
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

  igstk::MouseTracker::Pointer  tracker;

  tracker = igstk::MouseTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "Open()" << std::endl;
  tracker->Open();

  std::cout << "Initialize()" << std::endl;
  tracker->Initialize();

  std::cout << tracker << std::endl;

  std::cout << "StartTracking()" << std::endl;
  tracker->StartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<10; i++)
    {
    tracker->UpdateStatus();
    for (unsigned int port = 0; port < 4; port++)
      {
      for (unsigned int channel = 0; channel < 2; channel++)
        {
        TransformType             transform;
        VectorType                position;

        tracker->GetToolTransform( port, channel, transform );
        position = transform.GetTranslation();
        std::cout << "Port, Channel (" << port << "," << channel
                  << ") Position = (" << position[0]
                  << "," << position[1] << "," << position[2]
                  << ")" << std::endl;
        }
      }
    }
  
  std::cout << "Reset()" << std::endl;
  tracker->Reset();
  
  std::cout << "Initialize()" << std::endl;
  tracker->Initialize();
  
  std::cout << "StartTracking()" << std::endl;
  tracker->StartTracking();

  std::cout << "StopTracking()" << std::endl;
  tracker->StopTracking();

  std::cout << "Close()" << std::endl;
  tracker->Close();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}

