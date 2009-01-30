/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkAscensionTrackerTest.cxx
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
#include "igstkAscensionTracker.h"
#include "igstkAscensionTrackerTool.h"
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

class AscensionTrackerTestCommand : public itk::Command 
{
public:
  typedef  AscensionTrackerTestCommand    Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  AscensionTrackerTestCommand() {};

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

int igstkAscensionTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  if( argc < 3 )
  {
    std::cerr << " Usage: " << argv[0] << "\t" 
      << "Logger_Output_filename " <<"\t"
      << "Serial port number"  <<"\t" 
      << std::endl;
    return EXIT_FAILURE;
  }

  igstk::SerialCommunication::Pointer 
    serialComm = igstk::SerialCommunication::New();

  AscensionTrackerTestCommand::Pointer 
    my_command = AscensionTrackerTestCommand::New();

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
  PortNumberType  ascensionPortNumber = (PortNumberType) portNumberIntegerValue; 
  serialComm->SetPortNumber( ascensionPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  serialComm->SetCaptureFileName( "RecordedStreamByAscensionTrackerTest.txt" );
  serialComm->SetCapture( true );

  serialComm->OpenCommunication();

  igstk::AscensionTracker::Pointer  tracker;

  tracker = igstk::AscensionTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestSetFrequency: 60" << std::endl;
  tracker->RequestSetFrequency( 60 );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  // todo: add error observer for coomunication/starting error here and return FAILED

  typedef igstk::TrackerTool                TrackerToolType;
  typedef igstk::AscensionTrackerTool          AscensionTrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // have a generic tracker tool
  TrackerToolType::Pointer trackerTool;

  // instantiate ascension tracker tool
  AscensionTrackerToolType::Pointer ascensionTrackerTool = AscensionTrackerToolType::New();
  ascensionTrackerTool->AddObserver( itk::AnyEvent(), my_command);

  //Set the port number (we need to subtract by 1, as internally required in IGSTK [0 - 3])
  //unsigned int portNumber = atoi( argv[3] ) - 1;
  unsigned int portNumber = 1;
  ascensionTrackerTool->RequestSetPortNumber( portNumber );

  //Configure
  ascensionTrackerTool->RequestConfigure();

  //Attach to the tracker
  ascensionTrackerTool->RequestAttachToTracker( tracker );

  //////////////////////////////////////////////////////////////////////////
  // instantiate another ascension tracker tool
  AscensionTrackerToolType::Pointer ascensionTrackerTool2 = AscensionTrackerToolType::New();
  ascensionTrackerTool2->AddObserver( itk::AnyEvent(), my_command);
  ascensionTrackerTool2->RequestSetPortNumber( portNumber+1 );
  ascensionTrackerTool2->RequestConfigure();

  ascensionTrackerTool2->RequestAttachToTracker( tracker );
  //////////////////////////////////////////////////////////////////////////

  //start tracking 
  tracker->RequestStartTracking();

  //Sleep(10000);

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  //////////////////////////////////////////////////////////////////////////
  typedef igstk::TransformObserver      ObserverType;
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( ascensionTrackerTool );
  ObserverType::Pointer coordSystemAObserver2 = ObserverType::New();
  coordSystemAObserver2->ObserveTransformEventsFrom( ascensionTrackerTool2 );

  for(unsigned int i=0; i<100; i++)
  {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    ascensionTrackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
    {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
          << ascensionTrackerTool->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

    coordSystemAObserver2->Clear();
    ascensionTrackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
    {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool:" 
          << ascensionTrackerTool2->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

  }

  //////////////////////////////////////////////////////////////////////////

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  //Remove one of the tracker tools and restart tracking
  std::cout << "Detach the tracker tool1 from the tracker" << std::endl;
  ascensionTrackerTool->RequestDetachFromTracker( );

  // restart tracking 
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<100; i++)
  {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    ascensionTrackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
    {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
          << ascensionTrackerTool->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

    coordSystemAObserver2->Clear();
    ascensionTrackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
    {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool:" 
          << ascensionTrackerTool2->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

  }



  //////////////////////////////////////////////////////////////////////////

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  //Remove tracker tool2, reattach tool1 back, and restart tracking
  std::cout << "Detach the tracker tool2 from the tracker" << std::endl;
  ascensionTrackerTool2->RequestDetachFromTracker( );
  std::cout << "Attach tracker tool1 to the tracker" << std::endl;
  ascensionTrackerTool->RequestAttachToTracker( tracker );

  // restart tracking
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<100; i++)
  {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    ascensionTrackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
    {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
          << ascensionTrackerTool->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

    coordSystemAObserver2->Clear();
    ascensionTrackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
    {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool:" 
          << ascensionTrackerTool2->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

  }


  //////////////////////////////////////////////////////////////////////////

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  //Reattach tool2 and restart tracking
  std::cout << "Attach tracker tool2 to the tracker" << std::endl;
  ascensionTrackerTool2->RequestAttachToTracker( tracker );

  // restart tracking
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<100; i++)
  {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    ascensionTrackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
    {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
          << ascensionTrackerTool->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

    coordSystemAObserver2->Clear();
    ascensionTrackerTool2->RequestGetTransformToParent();
    if (coordSystemAObserver2->GotTransform())
    {
      transform = coordSystemAObserver2->GetTransform();
      if ( transform.IsValidNow() ) 
      {
        position = transform.GetTranslation();
        std::cout << "Trackertool:" 
          << ascensionTrackerTool2->GetTrackerToolIdentifier() 
          << "\t\t  Position = (" << position[0]
        << "," << position[1] << "," << position[2]
        << ")" << std::endl;
      }
      else
      {
        std::cout << "Invalid transform! make sure tool is within tracking volume\n";
      }
    }

  }
  //////////////////////////////////////////////////////////////////////////
 
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;


  return EXIT_SUCCESS;
}
