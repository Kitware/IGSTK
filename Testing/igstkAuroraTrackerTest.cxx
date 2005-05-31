/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerTest.cxx
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

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForLinux.h"
#endif

#include "igstkAuroraTracker.h"
#include "igstkTransform.h"

class SerialCommunicationTestCommand : public itk::Command 
{
public:
  typedef  SerialCommunicationTestCommand   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  SerialCommunicationTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    if ( typeid(event)== typeid(igstk::SerialCommunication::OpenPortFailureEvent))
    {
        std::cout << "OpenPortFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent ))
    {
        std::cout << "SetupCommunicationParametersFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetDataBufferSizeFailureEvent ))
    {
        std::cout << "SetDataBufferSizeFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent ))
    {
        std::cout << "CommunicationTimeoutSetupFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringSuccessfulEvent ))
    {
        std::cout << "****** SendStringSuccessfulEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringFailureEvent ))
    {
        std::cout << "****** SendStringFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringWriteTimeoutEvent ))
    {
        std::cout << "****** SendStringWriteTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringWaitTimeoutEvent ))
    {
        std::cout << "****** SendStringWaitTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationStatusReportFailureEvent ))
    {
        std::cout << "****** CommunicationStatusReportFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringSuccessfulEvent ))
    {
        std::cout << "****** ReceiveStringSuccessfulEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringFailureEvent ))
    {
        std::cout << "****** ReceiveStringFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringReadTimeoutEvent ))
    {
        std::cout << "****** ReceiveStringReadTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringWaitTimeoutEvent ))
    {
        std::cout << "****** ReceiveStringWaitTimeoutEvent ******\n";
    }
   else 
    {
        std::cout << "Some other Error Occurred ...\n";
    }
 }
};


int igstkAuroraTrackerTest( int, char * [] )
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForLinux::Pointer serialComm = igstk::SerialCommunicationForLinux::New();
#endif

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::WARNING); //DEBUG );

  serialComm->AddObserver( igstk::SerialCommunication::OpenPortFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetDataBufferSizeFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::RestCommunicationFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::FlushOutputBufferFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::OverlappedEventCreationFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SendStringSuccessfulEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SendStringFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SendStringWriteTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SendStringWaitTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::CommunicationStatusReportFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReceiveStringSuccessfulEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReceiveStringFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReceiveStringReadTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReceiveStringWaitTimeoutEvent(), my_command);
  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0() );
  serialComm->SetParity( igstk::SerialCommunication::NoParity() );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600() );
  serialComm->SetByteSize( igstk::SerialCommunication::DataBits8() );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1() );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff() );

  serialComm->OpenCommunication();

  igstk::NDITracker::Pointer  tracker;

  tracker = igstk::NDITracker::New();

  tracker->SetLogger( logger );

  std::cout << "Entering  SetCommunication ..." << std::endl;

  tracker->SetCommunication( serialComm );

  std::cout << "Exited SetCommunication ..." << std::endl;

  tracker->AttachSROMFileNameToPort( 0, "C:/Program Files/Northern Digital Inc/SROM Image Files/5D.ROM" );

  tracker->Initialize();

  tracker->StartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;

  TransformType             transitions;
  VectorType                position;

  for(int i=0; i<10; i++)
  {
    tracker->UpdateStatus();
    tracker->GetToolTransform( 0, 0, transitions );
    position = transitions.GetTranslation();
    std::cout << "Position = (" << position[0] << "," << position[1] << "," << position[2] << ")" << std::endl;
  }

  tracker->StopTracking();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
