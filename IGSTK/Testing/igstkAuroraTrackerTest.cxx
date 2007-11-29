/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerTest.cxx
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
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif
#include "igstkSerialCommunicationSimulator.h"
#include "igstkAuroraTracker.h"
#include "igstkTransform.h"

class AuroraTrackerTestCommand : public itk::Command 
{
public:
  typedef  AuroraTrackerTestCommand   Self;
  typedef  itk::Command               Superclass;
  typedef itk::SmartPointer<Self>     Pointer;
  itkNewMacro( Self );
protected:
  AuroraTrackerTestCommand() {};

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

#ifdef IGSTK_SIMULATOR_TEST
int igstkAuroraTrackerSimulatedTest( int argc, char * argv[] )
#else  /* IGSTK_SIMULATOR_TEST */
int igstkAuroraTrackerTest( int argc, char * argv[] )
#endif
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::AuroraTrackerTool::Pointer tool = igstk::AuroraTrackerTool::New();
  std::cout << "AuroraTrackerTool class name : " 
            << tool->GetNameOfClass() << std::endl;
  std::cout << tool << std::endl;

#ifdef IGSTK_SIMULATOR_TEST
    igstk::SerialCommunicationSimulator::Pointer 
                      serialComm = igstk::SerialCommunicationSimulator::New();
#else  /* IGSTK_SIMULATOR_TEST */
#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer 
                     serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer 
                       serialComm = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */
#endif /* IGSTK_SIMULATOR_TEST */

  AuroraTrackerTestCommand::Pointer 
                                 my_command = AuroraTrackerTestCommand::New();

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
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  std::string igstkDataDirectory = IGSTK_DATA_ROOT;
  std::string inputDirectory = igstkDataDirectory + "/Input";
  std::string simulationFile = (inputDirectory + "/" +
                                "aurora_stream_multichan_11_11_2005.txt");
  serialComm->SetFileName( simulationFile.c_str() );
#else /* IGSTK_SIMULATOR_TEST */
  serialComm->SetPortNumber( IGSTK_TEST_AURORA_PORT_NUMBER );
  serialComm->SetCaptureFileName( "RecordedStreamByAuroraTrackerTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::AuroraTracker::Pointer  tracker;

  tracker = igstk::AuroraTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

#ifdef IGSTK_SIMULATOR_TEST
  std::string romFile = inputDirectory + "/" + "aurora_multichan.rom";
  std::cout << "AttachSROMFileNameToPort()" << std::endl;
  tracker->AttachSROMFileNameToPort( 0, romFile.c_str() );
#endif /* IGSTK_SIMULATOR_TEST */

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
