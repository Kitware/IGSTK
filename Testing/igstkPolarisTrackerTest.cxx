/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerTest.cxx
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
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkSerialCommunicationSimulator.h"
#include "igstkPolarisTracker.h"
#include "igstkTransform.h"

class PolarisTrackerTestCommand : public itk::Command 
{
public:
  typedef  PolarisTrackerTestCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  PolarisTrackerTestCommand() {};

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
int igstkPolarisTrackerTest( int argc, char * argv[] )
#endif
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::PolarisTrackerTool::Pointer tool = igstk::PolarisTrackerTool::New();
  std::cout << tool->GetNameOfClass() << std::endl;
  std::cout << tool << std::endl;

#ifdef IGSTK_SIMULATOR_TEST
    igstk::SerialCommunicationSimulator::Pointer serialComm = igstk::SerialCommunicationSimulator::New();
#else  /* IGSTK_SIMULATOR_TEST */
#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer serialComm = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */
#endif /* IGSTK_SIMULATOR_TEST */

  PolarisTrackerTestCommand::Pointer my_command = PolarisTrackerTestCommand::New();

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
  serialComm->SetCaptureFileName( "RecordedStreamByPolarisTrackerTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::PolarisTracker::Pointer  tracker;

  tracker = igstk::PolarisTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "Open()" << std::endl;
  tracker->Open();

#ifdef IGSTK_SIMULATOR_TEST
  std::string romFile = igstkDataDirectory + "/";
  romFile = romFile + "ta2p0003-3-120.rom";
  std::cout << "AttachSROMFileNameToPort()" << std::endl;
  tracker->AttachSROMFileNameToPort( 3, romFile.c_str() );
#endif /* IGSTK_SIMULATOR_TEST */

  std::cout << "Initialize()" << std::endl;
  tracker->Initialize();

  std::cout << tracker << std::endl;

  std::cout << "GetNumberOfTools()" << std::endl;
  unsigned int ntools = tracker->GetNumberOfTools();

  std::cout << "NumberOfTools : " << ntools << std::endl;

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
      TransformType             transform;
      VectorType                position;

      tracker->GetToolTransform( port, 0, transform );
      position = transform.GetTranslation();
      std::cout << "Port " << port << "  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
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

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
