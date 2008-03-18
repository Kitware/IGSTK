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
#include "igstkSerialCommunication.h"
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

int igstkAuroraTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  if( argc < 3 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename " <<"\t"
                            << "Port number"  <<"\t" 
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::AuroraTrackerTool::Pointer tool = igstk::AuroraTrackerTool::New();

  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();

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
  logger->SetPriorityLevel( LoggerType::DEBUG);

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int portNumberIntegerValue = atoi(argv[2]);
  PortNumberType  auroraPortNumber = (PortNumberType) portNumberIntegerValue; 
  serialComm->SetPortNumber( auroraPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  serialComm->SetCaptureFileName( "RecordedStreamByAuroraTrackerTest.txt" );
  serialComm->SetCapture( true );

  serialComm->OpenCommunication();

  igstk::AuroraTracker::Pointer  tracker;

  tracker = igstk::AuroraTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::AuroraTrackerTool          TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate tracker tool with 6DOF 
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  trackerTool->RequestSelect6DOFTrackerTool();
  //Set the port number 
  unsigned int portNumber = atoi( argv[2] );
  trackerTool->RequestSetPortNumber( portNumber );
  //Configure
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
  //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );


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

    coordSystemAObserver->Clear();
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      position = transform.GetTranslation();
      std::cout << "Trackertool transform using observer:" 
              << trackerTool->GetTrackerToolIdentifier() 
              << "\t\t  Position = (" << position[0]
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
