/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerNewTest.cxx
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

#include "igstkSystemInformation.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkSerialCommunicationSimulator.h"
#include "igstkPolarisTrackerNew.h"
#include "igstkPolarisTrackerToolNew.h"
#include "igstkTransform.h"

class PolarisTrackerNewTestCommand : public itk::Command 
{
public:
  typedef  PolarisTrackerNewTestCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  PolarisTrackerNewTestCommand() {};

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
int igstkPolarisTrackerSimulatedTest( int argc, char * argv[] )
#else  /* IGSTK_SIMULATOR_TEST */
int igstkPolarisTrackerNewTest( int argc, char * argv[] )
#endif
{

  igstk::RealTimeClock::Initialize();

  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::PolarisTrackerToolNew::Pointer tool = igstk::PolarisTrackerToolNew::New();

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

  PolarisTrackerNewTestCommand::Pointer 
                                my_command = PolarisTrackerNewTestCommand::New();

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

  serialComm->SetPortNumber( IGSTK_TEST_POLARIS_PORT_NUMBER );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  std::string igstkDataDirectory = IGSTK_DATA_ROOT;
  std::string simulationFile = igstkDataDirectory + "/";
  simulationFile = simulationFile + "polaris_stream_11_05_2005.txt";
  serialComm->SetFileName( simulationFile.c_str() );
#else /* IGSTK_SIMULATOR_TEST */
  serialComm->SetCaptureFileName( "RecordedStreamByPolarisTrackerNewTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::PolarisTrackerNew::Pointer  tracker;

  tracker = igstk::PolarisTrackerNew::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

#ifdef IGSTK_SIMULATOR_TEST
  std::string romFile = igstkDataDirectory + "/";
  romFile = romFile + "ta2p0003-3-120.rom";
#endif /* IGSTK_SIMULATOR_TEST */

#ifndef IGSTK_SIMULATOR_TEST
  // Initialize wired tracker tool
  typedef igstk::PolarisTrackerToolNew      TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;
  
  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Select wired tracker tool
  trackerTool->RequestSelectWiredTrackerTool();
  //Set the port number to zero
  trackerTool->RequestSetPortNumber( 1 );
  //Initialize
  trackerTool->RequestInitialize();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );

  // instantiate and attach wireless tracker tool
  std::cout << "Instantiate second tracker tool: " << std::endl;
  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  //Select wireless tracker tool
  trackerTool2->RequestSelectWirelessTrackerTool();
  //Set the SROM file 
  std::string dataDirectory = IGSTK_DATA_ROOT;
  std::string romFile = dataDirectory + "/";
  romFile = romFile + "ta2p0003-3-120.rom";
  std::cout << "SROM file: " << romFile << std::endl;
  trackerTool2->RequestSetSROMFileName( romFile );
  //Initialize
  trackerTool2->RequestInitialize();
  //Attach to the tracker
  trackerTool2->RequestAttachToTracker( tracker );

  //initialize tracker
  tracker->RequestInitialize();

  //start tracking 
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<10; i++)
    {
    tracker->RequestUpdateStatus();
    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;

 

#endif

  return EXIT_SUCCESS;
}
