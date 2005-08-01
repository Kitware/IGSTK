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

#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForLinux  CommunicationType;
#endif

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
    if ( typeid(event)== typeid(CommunicationType::OpenPortFailureEvent))
    {
        std::cout << "OpenPortFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::SetCommunicationParametersFailureEvent ))
    {
        std::cout << "SetupCommunicationParametersFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::SetDataBufferSizeFailureEvent ))
    {
        std::cout << "SetDataBufferSizeFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::CommunicationTimeoutSetupFailureEvent ))
    {
        std::cout << "CommunicationTimeoutSetupFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::WriteSuccessEvent ))
    {
        std::cout << "****** WriteSuccessEvent ******\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::WriteFailureEvent ))
    {
        std::cout << "****** WriteFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::WriteTimeoutEvent ))
    {
        std::cout << "****** WriteTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::ReadSuccessEvent ))
    {
        std::cout << "****** ReadSuccessEvent ******\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::ReadFailureEvent ))
    {
        std::cout << "****** ReadFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( CommunicationType::ReadTimeoutEvent ))
    {
        std::cout << "****** ReadTimeoutEvent ******\n";
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

#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForLinux  CommunicationType;
#endif

  // this is set if the test failed in any way
  int testStatus = EXIT_SUCCESS;

  CommunicationType::Pointer serialComm = CommunicationType::New();

  serialComm->ExportStateMachineDescription( std::cout );

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  std::cout << serialComm << std::endl;

  serialComm->AddObserver( CommunicationType::OpenPortFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::SetCommunicationParametersFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::SetDataBufferSizeFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::CommunicationTimeoutSetupFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::SendBreakFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::FlushOutputBufferFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::WriteSuccessEvent(), my_command);
  serialComm->AddObserver( CommunicationType::WriteFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::WriteTimeoutEvent(), my_command);
  serialComm->AddObserver( CommunicationType::ReadSuccessEvent(), my_command);
  serialComm->AddObserver( CommunicationType::ReadFailureEvent(), my_command);
  serialComm->AddObserver( CommunicationType::ReadTimeoutEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( CommunicationType::PortNumber0 );
  serialComm->SetParity( CommunicationType::NoParity );
  serialComm->SetBaudRate( CommunicationType::BaudRate9600 );
  serialComm->SetDataBits( CommunicationType::DataBits8 );
  serialComm->SetStopBits( CommunicationType::StopBits1 );
  serialComm->SetHardwareHandshake( CommunicationType::HandshakeOff );

  serialComm->OpenCommunication();

  int len = strlen("Hello World!!!");
  int numberOfBytesRead = 0;
  const int MAX_REPLY_SIZE = 1024;
  char reply[MAX_REPLY_SIZE];
  
  // test a simple write/read sequence
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len, numberOfBytesRead);

  // if no bytes were read, that means a loopback is not attached to
  // the serial port and the test cannot run properly
  if (numberOfBytesRead == 0)
    {
    std::cerr << "Warning: no loopback on serial port, test is terminating prematurely" << std::endl;
    serialComm->CloseCommunication();
    return EXIT_SUCCESS;
    }

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

  // test a couple baud rates
  CommunicationType::BaudRateType allBaudRates[2] = {
    CommunicationType::BaudRate9600,
    CommunicationType::BaudRate19200,
  };

  CommunicationType::DataBitsType allDataBits[2] = {
    CommunicationType::DataBits7,
    CommunicationType::DataBits8,
  };
  
  CommunicationType::ParityType allParities[3] = {
    CommunicationType::NoParity,
    CommunicationType::OddParity,
    CommunicationType::EvenParity,
  };

  CommunicationType::StopBitsType allStopBits[2] = {
    CommunicationType::StopBits1,
    CommunicationType::StopBits2,
  };

  // note that we can't test hardware handshaking unless the loopback
  //  connects the handshaking pins.

  int counter;

  for (counter = 0; counter < 2 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->CloseCommunication();
    serialComm->SetBaudRate(allBaudRates[counter]);
    serialComm->OpenCommunication();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cerr << "failed SetBaudRate test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 2 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->CloseCommunication();
    serialComm->SetDataBits(allDataBits[counter]);
    serialComm->OpenCommunication();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cerr << "failed SetDataBits test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 3 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->CloseCommunication();
    serialComm->SetParity(allParities[counter]);
    serialComm->OpenCommunication();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cerr << "failed SetParity test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 2 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->CloseCommunication();
    serialComm->SetStopBits(allStopBits[counter]);
    serialComm->OpenCommunication();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cerr << "failed SetStopBits test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  serialComm->CloseCommunication();

  return testStatus;
}


