/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTest.cxx
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

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForLinux.h"
#endif

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
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetCommunicationParametersFailureEvent ))
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
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteSuccessfulEvent ))
    {
        std::cout << "****** WriteSuccessfulEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteFailureEvent ))
    {
        std::cout << "****** WriteFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteTimeoutEvent ))
    {
        std::cout << "****** WriteTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteWaitTimeoutEvent ))
    {
        std::cout << "****** WriteWaitTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationStatusReportFailureEvent ))
    {
        std::cout << "****** CommunicationStatusReportFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadSuccessfulEvent ))
    {
        std::cout << "****** ReadSuccessfulEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadFailureEvent ))
    {
        std::cout << "****** ReadFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadTimeoutEvent ))
    {
        std::cout << "****** ReadTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadWaitTimeoutEvent ))
    {
        std::cout << "****** ReadWaitTimeoutEvent ******\n";
    }
   else 
    {
        std::cout << "Some other Error Occurred ...\n";
    }
 }
};


int igstkSerialCommunicationTest( int, char * [] )
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  // this is set if the test failed in any way
  int testStatus = EXIT_SUCCESS;

#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForLinux::Pointer serialComm = igstk::SerialCommunicationForLinux::New();
#endif

  serialComm->ExportStateMachineDescription( std::cout );

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  std::cout << serialComm << std::endl;

  serialComm->AddObserver( igstk::SerialCommunication::OpenPortFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetCommunicationParametersFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetDataBufferSizeFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SendBreakFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::FlushOutputBufferFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::OverlappedEventCreationFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::WriteSuccessfulEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::WriteFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::WriteTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::WriteWaitTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::CommunicationStatusReportFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReadSuccessfulEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReadFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReadTimeoutEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::ReadWaitTimeoutEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0() );
  serialComm->SetParity( igstk::SerialCommunication::NoParity() );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600() );
  serialComm->SetByteSize( igstk::SerialCommunication::DataBits8() );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1() );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff() );

  serialComm->OpenCommunication();

  int len = strlen("Hello World!!!");
  int numberOfBytesRead = 0;
  const int MAX_REPLY_SIZE = 1024;
  char reply[MAX_REPLY_SIZE];
  
  // test a simple write/read sequence
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len, numberOfBytesRead);

  if (numberOfBytesRead != len ||
      strncmp(reply, "Hello World!!!", len) != 0)
    {
    std::cerr << "Failed simple read/write test" << std::endl;
    serialComm->CloseCommunication();
    return EXIT_FAILURE;
    }

  // try reading more chars than available to generate a timeout
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, MAX_REPLY_SIZE, numberOfBytesRead);

  if (numberOfBytesRead != len)
    {
    std::cerr << "Failed timout test" << std::endl;
    testStatus = EXIT_FAILURE;
    }

  // try reading only part of the reply
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len-2, numberOfBytesRead);
  if (numberOfBytesRead != len-2)
    {
    std::cerr << "failed partial read test 1st part" << std::endl;
    testStatus = EXIT_FAILURE;
    }
  serialComm->Read(reply, 2, numberOfBytesRead);
  if (numberOfBytesRead != 2)
    {
    std::cerr << "failed partial read test 2nd part" << std::endl;
    testStatus = EXIT_FAILURE;
    }

  // send a serial break (there isn't any way to test that it was sent)
  serialComm->SendBreak();
  
  serialComm->CloseCommunication();

  return testStatus;
}


