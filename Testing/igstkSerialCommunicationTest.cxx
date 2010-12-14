/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTest.cxx
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

#include "igstkSystemInformation.h"
#include "igstkEvents.h"
#include "igstkSerialCommunication.h"

class SerialCommunicationTestCommand : public itk::Command 
{
public:

  typedef igstk::SerialCommunication     CommunicationType;

  typedef  SerialCommunicationTestCommand   Self;
  typedef  itk::Command                     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  itkNewMacro( Self );

protected:
  SerialCommunicationTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
    std::cout << event.GetEventName() << std::endl;
    }
};


int igstkSerialCommunicationTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 2 )
    {
    std::cerr << "Error missing argument " << std::endl;
    std::cerr << "Usage:  " << argv[0]  
              << "Test_Output_Directory" << std::endl; 
    return EXIT_FAILURE;
    }

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  typedef igstk::SerialCommunication    CommunicationType;

  // this is set if the test failed in any way
  int testStatus = EXIT_SUCCESS;

  CommunicationType::Pointer serialComm = CommunicationType::New();

  SerialCommunicationTestCommand::Pointer 
                           my_command = SerialCommunicationTestCommand::New();

  // logger object created 
  std::string testName = argv[0];
  std::string outputDirectory = argv[1];
  std::string filename = outputDirectory +"/";
  filename = filename + testName;
  filename = filename + "LoggerOutput.txt";
  std::cout << "Logger output saved here:\n";
  std::cout << filename << "\n"; 

  std::ofstream loggerFile;
  loggerFile.open( filename.c_str() );
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( loggerFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  std::cout << serialComm->GetNameOfClass() << std::endl;
  std::cout << serialComm << std::endl;

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( CommunicationType::PortNumber0 );
  serialComm->SetParity( CommunicationType::NoParity );
  serialComm->SetBaudRate( CommunicationType::BaudRate115200 );
  serialComm->SetDataBits( CommunicationType::DataBits8 );
  serialComm->SetStopBits( CommunicationType::StopBits1 );
  serialComm->SetHardwareHandshake( CommunicationType::HandshakeOff );
  serialComm->SetTimeoutPeriod(100);

  std::cout << "PortNumber: " << serialComm->GetPortNumber() << std::endl;
  std::cout << "Parity: " << serialComm->GetParity() << std::endl;
  std::cout << "BaudRate: " << serialComm->GetBaudRate() << std::endl;
  std::cout << "DataBits: " << serialComm->GetDataBits() << std::endl;
  std::cout << "StopBits: " << serialComm->GetStopBits() << std::endl;
  std::cout << "HardwareHandshake: " 
            << serialComm->GetHardwareHandshake() << std::endl;
  std::cout << "TimeoutPeriod: " 
            << serialComm->GetTimeoutPeriod() << std::endl;

  serialComm->SetCaptureFileName( 
                              "RecordedStreamBySerialCommunicationTest.txt" );
  serialComm->SetCapture( true );
  if( serialComm->GetCapture() != true )
    {
    std::cout << "Set/GetCapture() failed" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    }
  std::cout << "CaptureFileName: " 
            << serialComm->GetCaptureFileName() << std::endl;

  unsigned int len = strlen("Hello World!!!");
  unsigned int numberOfBytesRead = 0;
  const unsigned int MAX_REPLY_SIZE = 1024;
  char reply[MAX_REPLY_SIZE];

#ifndef IGSTK_TEST_LOOPBACK_ATTACHED
  // do a simple write/read sequence
  serialComm->OpenCommunication();  
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len, numberOfBytesRead);
  serialComm->PurgeBuffers();
  serialComm->Sleep(1000);
  serialComm->SendBreak();
  serialComm->CloseCommunication();
  // exit since a real test isn't possible without loopback device
  return testStatus;
#endif /* IGSTK_TEST_LOOPBACK_ATTACHED */

#ifdef IGSTK_TEST_LOOPBACK_ATTACHED
  // run tests assuming a loopback is attached, which will reply
  // exactly the same text that is sent

  serialComm->SetPortNumber( IGSTK_TEST_LOOPBACK_PORT_NUMBER );
  serialComm->OpenCommunication();  

  // perform a simple read/write test
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len, numberOfBytesRead);
  serialComm->PurgeBuffers();
  serialComm->Sleep(1000);

  if (numberOfBytesRead != len ||
      strncmp(reply, "Hello World!!!", len) != 0)
    {
    std::cout << "Failed simple read/write test" << std::endl;
    serialComm->CloseCommunication();
    return EXIT_FAILURE;
    }

  // try reading more chars than available to generate a timeout
  serialComm->SetTimeoutPeriod(50);
  serialComm->Write("Hello World!!!", len);
  serialComm->SetTimeoutPeriod(500);
  serialComm->Read(reply, MAX_REPLY_SIZE, numberOfBytesRead);

  if (numberOfBytesRead != len)
    {
    std::cout << "Failed timout test" << std::endl;
    testStatus = EXIT_FAILURE;
    }

  // try reading only part of the reply
  serialComm->Write("Hello World!!!", len);
  serialComm->Read(reply, len-2, numberOfBytesRead);
  if (numberOfBytesRead != len-2)
    {
    std::cout << "failed partial read test 1st part" << std::endl;
    testStatus = EXIT_FAILURE;
    }
  serialComm->Read(reply, 2, numberOfBytesRead);
  if (numberOfBytesRead != 2)
    {
    std::cout << "failed partial read test 2nd part" << std::endl;
    testStatus = EXIT_FAILURE;
    }

  // send a serial break (there isn't any way to test that it was sent)
  serialComm->SendBreak();

  // purge buffers of all the zeros that were looped back from the break
  serialComm->Sleep(10);
  serialComm->PurgeBuffers();

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
    serialComm->SetBaudRate(allBaudRates[counter]);
    serialComm->UpdateParameters();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cout << "failed SetBaudRate test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 2 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->SetDataBits(allDataBits[counter]);
    serialComm->UpdateParameters();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cout << "failed SetDataBits test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 3 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->SetParity(allParities[counter]);
    serialComm->UpdateParameters();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cout << "failed SetParity test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  for (counter = 0; counter < 2 && !(testStatus == EXIT_FAILURE); counter++)
    {
    serialComm->SetStopBits(allStopBits[counter]);
    serialComm->UpdateParameters();
    serialComm->Write("Hello World!!!", len);
    serialComm->Read(reply, len, numberOfBytesRead);
    if (strncmp(reply, "Hello World!!!", len) != 0)
      {
      std::cout << "failed SetStopBits test" << std::endl;
      testStatus = EXIT_FAILURE;
      }
    }

  serialComm->CloseCommunication();

  return testStatus;

#endif /* IGSTK_TEST_LOOPBACK_ATTACHED */
}
