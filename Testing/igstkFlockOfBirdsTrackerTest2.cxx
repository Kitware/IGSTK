/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkFlockOfBirdsTrackerTest2.cxx
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
#include "igstkSerialCommunication.h"
#include "igstkSerialCommunicationSimulator.h"
#include "igstkFlockOfBirdsTrackerNew.h"
#include "igstkFlockOfBirdsTrackerToolNew.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"

class FlockOfBirdsTrackerTest2Command : public itk::Command 
{
public:
  typedef  FlockOfBirdsTrackerTest2Command   Self;
  typedef  itk::Command                 Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  itkNewMacro( Self );
protected:
  FlockOfBirdsTrackerTest2Command() {};

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

/** This program tests single bird tracker tool */
int igstkFlockOfBirdsTrackerTest2( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  if( argc < 3 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Serial_communication_port"
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::FlockOfBirdsTrackerTool::Pointer tool = igstk::FlockOfBirdsTrackerTool::New();

  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();

  FlockOfBirdsTrackerTest2Command::Pointer 
                                my_command = FlockOfBirdsTrackerTest2Command::New();

  typedef igstk::TransformObserver   ObserverType;

  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
 
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
  unsigned int fobNumberIntegerValue = atoi(argv[2]);
  PortNumberType  fobPortNumber = PortNumberType(fobNumberIntegerValue); 
  serialComm->SetPortNumber( fobPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  serialComm->SetCaptureFileName( "RecordedStreamByFlockOfBirdsTrackerTest2.txt" );
  serialComm->SetCapture( true );

  if( !serialComm->OpenCommunication())
  {
      std::cout << "Serial port open failure\n";
      return EXIT_FAILURE;
  }

  igstk::FlockOfBirdsTracker::Pointer  tracker;

  tracker = igstk::FlockOfBirdsTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::FlockOfBirdsTrackerTool    TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );

  //Configure
  trackerTool->RequestSetBirdName("bird0");
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
  //Add observer to listen to events thrown by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events 
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );
  //start tracking 
  std::cout << "Start tracking..." << std::endl;
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  for(unsigned int i=0; i<10; i++)
  {
      TransformType             transform;
      VectorType                position;

      coordSystemAObserver->Clear();
      trackerTool->RequestGetTransformToParent();
      if (coordSystemAObserver->GotTransform())
      {
          transform = coordSystemAObserver->GetTransform();
          position = transform.GetTranslation();
          std::cout << "\t\t  Position = (" << position[0]
          << "," << position[1] << "," << position[2]
          << ")" << std::endl;
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
