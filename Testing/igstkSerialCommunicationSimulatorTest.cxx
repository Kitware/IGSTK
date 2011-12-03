/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulatorTest.cxx
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
#include "igstkSerialCommunicationSimulator.h"

#include "igstkAuroraTracker.h"

#include "igstkTransform.h"
#include "igstkTransformObserver.h"

class SerialCommunicationTestCommand : public itk::Command 
{
public:
  typedef  SerialCommunicationTestCommand   Self;
  typedef  itk::Command                     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  itkNewMacro( Self );
protected:
  SerialCommunicationTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
    std::cout << event.GetEventName() << std::endl;
    }
};


int igstkSerialCommunicationSimulatorTest( int argc, char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
    std::cerr << "Error missing argument " << std::endl;
    std::cerr << "Usage:  " << argv[0]  
              << "Test_Output_Directory" 
              << "Simulated data"<< std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  igstk::SerialCommunicationSimulator::Pointer 
                      serialComm = igstk::SerialCommunicationSimulator::New();

  std::cout << serialComm->GetNameOfClass() << std::endl;

  // for increasing test coverage
  serialComm->SetFileName("wrong_name");
  serialComm->OpenCommunication();

  // set the name of the actual data file
  std::string simulationFile = argv[2];
  serialComm->SetFileName( simulationFile.c_str() );
  serialComm->GetFileName();

  SerialCommunicationTestCommand::Pointer 
                           my_command = SerialCommunicationTestCommand::New();

  // logger object created 
  std::string testName = argv[0];
  std::string outputDirectory = argv[1];
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

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  std::cout << serialComm << std::endl;

  serialComm->OpenCommunication();

  igstk::AuroraTracker::Pointer       tracker;
  igstk::AuroraTrackerTool::Pointer   tool;

  tracker = igstk::AuroraTracker::New();
  tool    = igstk::AuroraTrackerTool::New();

  tool->RequestConfigure();
  tool->RequestAttachToTracker( tracker );
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( tool );

  tracker->SetLogger( logger );

  std::cout << "Entering  SetCommunication ..." << std::endl;

  tracker->SetCommunication( serialComm );

  std::cout << "Exited SetCommunication ..." << std::endl;

  tracker->RequestOpen();

  std::cout << tracker << std::endl;

  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<10; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    tool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
                << tool->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
    }

  tracker->RequestStopTracking();

  tracker->RequestClose();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
