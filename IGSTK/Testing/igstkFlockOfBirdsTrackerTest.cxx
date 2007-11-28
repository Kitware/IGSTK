/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerTest.cxx
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
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkSystemInformation.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif
#include "igstkSerialCommunicationSimulator.h"
#include "igstkFlockOfBirdsTracker.h"
#include "igstkTransform.h"

class FlockOfBirdsTrackerTestCommand : public itk::Command 
{
public:
  typedef  FlockOfBirdsTrackerTestCommand  Self;
  typedef  itk::Command                    Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  itkNewMacro( Self );
protected:
  FlockOfBirdsTrackerTestCommand() {};

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

#define IGSTK_SIMULATOR_TEST 1

int igstkFlockOfBirdsTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::FlockOfBirdsTrackerTool::Pointer tool 
                                      = igstk::FlockOfBirdsTrackerTool::New();
  std::cout << "FlockOfBirdsTrackerTool class name : " 
            << tool->GetNameOfClass() << std::endl;
  std::cout << tool << std::endl;

#ifdef IGSTK_SIMULATOR_TEST
    igstk::SerialCommunicationSimulator::Pointer serialComm 
                                 = igstk::SerialCommunicationSimulator::New();
#else  /* IGSTK_SIMULATOR_TEST */
#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm 
                                 = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer serialComm 
                                  = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */
#endif /* IGSTK_SIMULATOR_TEST */

  FlockOfBirdsTrackerTestCommand::Pointer my_command 
                                      = FlockOfBirdsTrackerTestCommand::New();

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

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate19200 );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  std::string igstkDataDirectory = IGSTK_DATA_ROOT;
  std::string inputDirectory = igstkDataDirectory + "/Input";
  std::string simulationFile = (inputDirectory + "/" +
                                "flockofbirds_stream_03_21_2006.txt");
  serialComm->SetFileName( simulationFile.c_str() );
#else /* IGSTK_SIMULATOR_TEST */
  serialComm->SetCaptureFileName( 
                              "RecordedStreamByFlockOfBirdsTrackerTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::FlockOfBirdsTracker::Pointer  tracker;

  tracker = igstk::FlockOfBirdsTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

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

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<10; i++)
    {
    tracker->RequestUpdateStatus();
    unsigned int port = 0;
    unsigned int channel = 0;
    TransformType             transform;
    VectorType                position;

    tracker->GetToolTransform( port, channel, transform );
    position = transform.GetTranslation();
    std::cout << "Port, Channel (" << port << "," << channel
              << ") Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;
    }
  
  std::cout << "RequestReset()" << std::endl;
  tracker->RequestReset();
  
  std::cout << "RequestInitialize()" << std::endl;
  tracker->RequestInitialize();
  
  std::cout << "RequestStartTracking()" << std::endl;
  tracker->RequestStartTracking();

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
