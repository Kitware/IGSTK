/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerSimulatedTest.cxx
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
#if defined(WIN32) || defined(_WIN32)
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkSerialCommunicationSimulator.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
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

int igstkPolarisTrackerSimulatedTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType       LoggerType; 
  typedef itk::StdStreamLogOutput         LogOutputType;

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Simulation_filename "
                            << "Wireless_SROM_filename "
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::PolarisTrackerTool::Pointer tool = igstk::PolarisTrackerTool::New();

  igstk::SerialCommunicationSimulator::Pointer 
                      serialComm = igstk::SerialCommunicationSimulator::New();

  PolarisTrackerTestCommand::Pointer 
                                my_command = PolarisTrackerTestCommand::New();

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

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( IGSTK_TEST_POLARIS_PORT_NUMBER );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  std::string simulationFile = argv[2];
  serialComm->SetFileName( simulationFile.c_str() );

  serialComm->OpenCommunication();

  igstk::PolarisTracker::Pointer  tracker;

  tracker = igstk::PolarisTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::PolarisTrackerTool      TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Select wired tracker tool
  trackerTool->RequestSelectWiredTrackerTool();
  //Set the port number to zero
  trackerTool->RequestSetPortNumber( 0 );
  //Configure
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );

  // instantiate and attach wireless tracker tool
  std::cout << "Instantiate second tracker tool: " << std::endl;
  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  //Select wireless tracker tool
  trackerTool2->RequestSelectWirelessTrackerTool();
  //Set the SROM file 
  std::string romFile = argv[3];
  std::cout << "SROM file: " << romFile << std::endl;
  trackerTool2->RequestSetSROMFileName( romFile );
  //Configure
  trackerTool2->RequestConfigure();
  //Attach to the tracker
  trackerTool2->RequestAttachToTracker( tracker );

  //start tracking 
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;


  for(unsigned int i=0; i<400; i++)
    {
    tracker->RequestUpdateStatus();

    TransformType             transform;
    VectorType                position;

    transform = trackerTool->GetCalibratedTransform();
    position = transform.GetTranslation();
    std::cout << "Trackertool:" << trackerTool->GetTrackerToolIdentifier() 
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    transform = trackerTool2->GetCalibratedTransform();
    position = transform.GetTranslation();
    std::cout << "Trackertool:" << trackerTool2->GetTrackerToolIdentifier() 
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;
 
    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;


  return EXIT_SUCCESS;
}
