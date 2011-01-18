/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerSimulatedTest.cxx
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
#include "igstkAuroraTrackerTool.h"
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

class AuroraTrackerTestCommand : public itk::Command 
{
public:
  typedef  AuroraTrackerTestCommand    Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  AuroraTrackerTestCommand() {};

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

int igstkAuroraTrackerSimulatedTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  if( argc < 5 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Simulation_filename "
                            << "MultiChannel_ROM_filename "
                            << "SericalCommunication_PORT_Number"
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::AuroraTrackerTool::Pointer tool = igstk::AuroraTrackerTool::New();

  igstk::SerialCommunicationSimulator::Pointer 
                      serialComm = igstk::SerialCommunicationSimulator::New();

  AuroraTrackerTestCommand::Pointer 
                                my_command = AuroraTrackerTestCommand::New();

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

  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int portNumberIntegerValue = atoi(argv[4]);
  PortNumberType  auroraPortNumber = PortNumberType(portNumberIntegerValue); 
  serialComm->SetPortNumber( auroraPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  std::string simulationFile = argv[2];
  std::cout << "Serial communication simulation file: " 
           << simulationFile << std::endl;
  serialComm->SetFileName( simulationFile.c_str() );

  serialComm->OpenCommunication();

  igstk::AuroraTracker::Pointer  tracker;

  tracker = igstk::AuroraTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  //This improves coverage by preventing early return.
  tracker->SetSimulatedTestMaintainCoverage(true);

  typedef igstk::AuroraTrackerTool          TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach 5DOF tracker tool type 
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Select 5DOF type
  trackerTool->RequestSelect5DOFTrackerTool();
  //Set the port number to zero
  trackerTool->RequestSetPortNumber( 0 );
  trackerTool->RequestSetChannelNumber( 0 );
  std::string SROMfilename = argv[3];
  trackerTool->RequestSetSROMFileName( SROMfilename );
  //Configure
  trackerTool->RequestConfigure();
  trackerTool->GetPartNumber();
  trackerTool->GetChannelNumber();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
  //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  // instantiate and attach a second 5DOF tracker tool type 
  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  //Select 5DOF type
  trackerTool2->RequestSelect5DOFTrackerTool();
  //Set the port number to zero
  trackerTool2->RequestSetPortNumber( 0 );
  trackerTool2->RequestSetChannelNumber( 1 );
  //Configure
  trackerTool2->RequestConfigure();
  trackerTool2->GetPartNumber();
  trackerTool2->GetChannelNumber();
  //Attach to the tracker
  trackerTool2->RequestAttachToTracker( tracker );
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
  coordSystemAObserver2->ObserveTransformEventsFrom( trackerTool2 );

  //start tracking 
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
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() )
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool transform using observer:" 
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
        std::cout << "Trackertool2 transform using observer:" 
                << trackerTool2->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
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
