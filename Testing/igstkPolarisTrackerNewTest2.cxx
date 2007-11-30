/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerNewTest2.cxx
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
#include "igstkPolarisTrackerNew.h"
#include "igstkPolarisTrackerToolNew.h"
#include "igstkTransform.h"

class CoordinateReferenceSystemObserver : public ::itk::Command
{
public:
  typedef igstk::CoordinateReferenceSystemTransformToEvent  EventType;
  typedef igstk::CoordinateReferenceSystemTransformToResult PayloadType;

  /** Standard class typedefs. */
  typedef CoordinateReferenceSystemObserver         Self;
  typedef ::itk::Command                            Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(CoordinateReferenceSystemObserver, ::itk::Command);
  itkNewMacro(CoordinateReferenceSystemObserver);

  CoordinateReferenceSystemObserver()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  ~CoordinateReferenceSystemObserver()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  void ClearPayload()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    this->ClearPayload();
    if( EventType().CheckEvent( &event ) )
      {
      const EventType * transformEvent = 
                dynamic_cast< const EventType *>( &event );
      if( transformEvent )
        {
        m_Payload = transformEvent->Get();
        m_GotPayload = true;
        }
      }
    else
      {
      std::cout << "Got unexpected event : " << std::endl;
      event.Print(std::cout);
      }
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    this->Execute(static_cast<const itk::Object*>(caller), event);
    }

  bool GotPayload()
    {
    return m_GotPayload;
    }

  PayloadType GetPayload()
    {
    return m_Payload;
    }

protected:

  PayloadType   m_Payload;
  bool          m_GotPayload;

};

class PolarisTrackerNewTest2Command : public itk::Command 
{
public:
  typedef  PolarisTrackerNewTest2Command   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  PolarisTrackerNewTest2Command() {};

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

int igstkPolarisTrackerNewTest2( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  if( argc < 3 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Wireless_SROM_filename "
                            << std::endl;
    return EXIT_FAILURE;
    }


  igstk::PolarisTrackerToolNew::Pointer tool = igstk::PolarisTrackerToolNew::New();

#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer 
                     serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer
                       serialComm = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */

  PolarisTrackerNewTest2Command::Pointer 
                                my_command = PolarisTrackerNewTest2Command::New();

  typedef CoordinateReferenceSystemObserver ObserverType;
  typedef ObserverType::EventType CoordinateSystemEventType;

  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
 
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

  serialComm->SetCaptureFileName( "RecordedStreamByPolarisTrackerNewTest2.txt" );
  serialComm->SetCapture( true );

  serialComm->OpenCommunication();

  igstk::PolarisTrackerNew::Pointer  tracker;

  tracker = igstk::PolarisTrackerNew::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::PolarisTrackerToolNew      TrackerToolType;
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
  //Add observer to listen to events throw by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events 
  trackerTool->AddObserver( CoordinateSystemEventType(), 
                                  coordSystemAObserver );


  // instantiate and attach wireless tracker tool
  std::cout << "Instantiate second tracker tool: " << std::endl;
  TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
  trackerTool2->SetLogger( logger );
  //Select wireless tracker tool
  trackerTool2->RequestSelectWirelessTrackerTool();
  //Set the SROM file 
  std::string romFile = argv[2];
  std::cout << "SROM file: " << romFile << std::endl;
  trackerTool2->RequestSetSROMFileName( romFile );
  //Configure
  trackerTool2->RequestConfigure();
  //Attach to the tracker
  trackerTool2->RequestAttachToTracker( tracker );
  //Add observer to listen to events throw by the tracker tool
  trackerTool2->AddObserver( itk::AnyEvent(), my_command);
  //Add observer to listen to transform events 
  trackerTool2->AddObserver( CoordinateSystemEventType(), 
                                  coordSystemAObserver );

  //Use the first trackertool as a reference
  tracker->RequestSetReferenceTool( trackerTool );

  //start tracking 
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;


  for(unsigned int i=0; i<100; i++)
    {
    tracker->RequestUpdateStatus();

    TransformType             transform;
    VectorType                position;


    //There are two ways of accessing the transform
    //First option: use GetToolTransform method
    tracker->GetToolTransform( 
      trackerTool2->GetTrackerToolIdentifier(), transform );

    position = transform.GetTranslation();
    std::cout << "Trackertool:" << trackerTool2->GetTrackerToolIdentifier() 
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    //Second option: use coordinate system convenience method
    trackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver->GotPayload())
      {
      transform = coordSystemAObserver->GetPayload().m_Transform;
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

  return EXIT_SUCCESS;
}
