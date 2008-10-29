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

/**
 * This class observes the TrackerToolTransformUpdateEvent for a specific tool.
 * It checks that the event is for the relevant tool and then gets the tool's
 * transform to its parent and prints it out.
 */
class ToolUpdatedObserver : public ::itk::Command
{
public:
  typedef  ToolUpdatedObserver    Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  ToolUpdatedObserver() {
    this->m_TransformObserver = igstk::TransformObserver::New();
    this->m_NumberOfIterations = 10;
  }

  ~ToolUpdatedObserver() {}

public:
  void Initialize( igstk::TrackerTool::Pointer trackerTool, bool *stop )
  {
    this->m_Stop = stop;
    this->m_Counter = 0;    
    this->m_Tool = trackerTool;
    this->m_TransformObserver->ObserveTransformEventsFrom( this->m_Tool );
  }

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
  }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {               //do something only for the correct tool    

    if( this->m_Tool.GetPointer() == caller )
    {               //the tool transform has been updated, get it
      if( dynamic_cast<const igstk::TrackerToolTransformUpdateEvent  *>( &event) )
      { 
        if ( this->m_Counter > this->m_NumberOfIterations )
        {
          *(this->m_Stop) = true;
          return;
        }
                        //request to get the transform               
        this->m_Tool->RequestGetTransformToParent();
                       //check that we got it
        if ( this->m_TransformObserver->GotTransform() )
        {
          igstk::Transform transform = this->m_TransformObserver->GetTransform();
          igstk::Transform::VectorType t = transform.GetTranslation();
          igstk::Transform::VersorType r = transform.GetRotation();
          std::cout<<"\nt:"<<t[0]<<"\t"<<t[1]<<"\t"<<t[2]<<"\n";
          std::cout<<"r:"<<r.GetX()<<"\t"<<r.GetY()<<"\t"<<r.GetZ()<<"\t"<<r.GetW()<<"\n";
          this->m_Counter++;
        }
      }
    }
  }

private:
  bool *m_Stop;
  unsigned int m_Counter;
  unsigned int m_NumberOfIterations;
  igstk::TransformObserver::Pointer m_TransformObserver; 
  igstk::TrackerTool::Pointer  m_Tool;
};

int igstkAuroraTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  if( argc < 3 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename " <<"\t"
                            << "Serial port number"  <<"\t" 
                            << "Control box port number"  <<"\t" 
                            << std::endl;
    return EXIT_FAILURE;
    }

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

  // todo: add error observer for coomunication/starting error here and return FAILED

  typedef igstk::TrackerTool                TrackerToolType;
  typedef igstk::AuroraTrackerTool          AuroraTrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // have a generic tracker tool
  TrackerToolType::Pointer trackerTool;

  // instantiate aurora tracker tool
  AuroraTrackerToolType::Pointer auroraTrackerTool = AuroraTrackerToolType::New();

  //we use a 5 DOF sensor
  auroraTrackerTool->RequestSelect5DOFTrackerTool();

  //Set the port number (we need to subtract by 1, as internally required in IGSTK [0 - 3])
  unsigned int portNumber = atoi( argv[3] ) - 1;
  auroraTrackerTool->RequestSetPortNumber( portNumber );

  // as we use a 5 DOF we need to set a channel number [0-1]
  auroraTrackerTool->RequestSetChannelNumber( 0 );

  //Configure
  auroraTrackerTool->RequestConfigure();

  //Attach to the tracker
  auroraTrackerTool->RequestAttachToTracker( tracker );

  //start tracking 
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  trackerTool = auroraTrackerTool;

  bool stop = false;
  // create an observer for the TrackerToolTransformUpdateEvent
  ToolUpdatedObserver::Pointer toolUpdateObserver = ToolUpdatedObserver::New();
  toolUpdateObserver->Initialize(trackerTool, &stop);

  // add the observer to the tracker tool
  trackerTool->AddObserver( igstk::TrackerToolTransformUpdateEvent(), toolUpdateObserver );

  while (!stop)
  {
    // this indeed is not an infinite loop. The stop variable will be set to true by the 
    // observer upon completed the number of iterations (m_NumberOfIterations)
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
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
