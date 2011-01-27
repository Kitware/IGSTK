/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PolarisTracker.cxx
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

#include "igstkSerialCommunication.h"

// BeginLatex
// 
// This example illustrates IGSTK's interface to Polaris NDI tracker. Polaris
// trackers are optical measurement system that measure the 3D positions of
// either active or passive markers.
//
// EndLatex

// BeginLatex
// 
// To communicate with Polaris tracking device, include the Polaris Tracker
// header files ( \doxygen{PolarisTracker} ) and (\doxygen{PolarisTrackerTool}).
//
// EndLatex

// BeginCodeSnippet
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
// EndCodeSnippet

#include "igstkTransform.h"
#include "igstkTransformObserver.h"

class PolarisTrackerCommand : public itk::Command 
{
public:
  typedef  PolarisTrackerCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  PolarisTrackerCommand() {};

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

int main( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Wireless_SROM_filename "
                            << "Port_Number"
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::PolarisTrackerTool::Pointer tool = igstk::PolarisTrackerTool::New();

  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();

  PolarisTrackerCommand::Pointer 
                                my_command = PolarisTrackerCommand::New();

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

  //BeginLatex
  //Instantiate serial communication object and set the communication parameters.
  //
  //EndLatex
  //
  //
  //BeginCodeSnippet
  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int portNumberIntegerValue = atoi(argv[3]);
  PortNumberType  polarisPortNumber = PortNumberType(portNumberIntegerValue); 
  serialComm->SetPortNumber( polarisPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  serialComm->SetCaptureFileName( "RecordedStreamByPolarisTracker.txt" );
  serialComm->SetCapture( true );
  //EndCodeSnippet

  serialComm->OpenCommunication();

  igstk::PolarisTracker::Pointer  tracker;

  tracker = igstk::PolarisTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::PolarisTrackerTool         TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  //BeginLatex
  //Wired and wireless tools can be handled by the Polaris Tracker class.
  //EndLatex
  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Add observer to listen to events throw by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Select wired tracker tool
  //BeginLatex
  //For wired tracker tool type, invoke RequestSelectWirelessTrackerTool method.
  //EndLatex
  //BeginCodeSnippet
  trackerTool->RequestSelectWiredTrackerTool();
  //EndCodeSnippet
  //Set the port number to zero
  trackerTool->RequestSetPortNumber( 0 );
  //Configure
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
 //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  // instantiate and attach wireless tracker tool
  std::cout << "Instantiate second tracker tool: " << std::endl;
  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  //Add observer to listen to events throw by the tracker tool
  trackerTool2->AddObserver( itk::AnyEvent(), my_command);
  //Select wireless tracker tool
  //BeginLatex
  //For wireless tracker tool type, invoke RequestSelectWirelessTrackerTool()
  //method and set SROM file.
  //EndLatex
  //BeginCodeSnippet
  trackerTool2->RequestSelectWirelessTrackerTool();
  //Set the SROM file 
  std::string romFile = argv[2];
  std::cout << "SROM file: " << romFile << std::endl;
  trackerTool2->RequestSetSROMFileName( romFile );
  //EndCodeSnippet
  //Configure
  trackerTool2->RequestConfigure();
  //Attach to the tracker
  //BeginLatex
  //After configuring the tracker tool, make a request to attach the tracker tool to the 
  // tracker.
  //EndLatex
  //BeginCodeSnippet
  trackerTool2->RequestAttachToTracker( tracker );
  //EndCodeSnippet
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
  coordSystemAObserver2->ObserveTransformEventsFrom( trackerTool2 );

  //start tracking 
  //BeginLatex
  //Start tracking and observer tracker tool pose information.
  //EndLatex
  //
  //BeginCodeSnippet
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  

  for(unsigned int i=0; i<100; i++)
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
        std::cout << "Trackertool :" 
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
        std::cout << "Trackertool2:" 
                << trackerTool2->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
    }
  //EndCodeSnippet
 
  //BeginLatex
  //To end the tracking process, stop and close the tracker and close the
  //serial communication channel.
  //EndLatex 
  //BeginCodeSnippet
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();
  //EndCodeSnippet

  return EXIT_SUCCESS;
}
