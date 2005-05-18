/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreterTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"

#include "igstkLogger.h"
#include "igstkNDICommandInterpreter.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForLinux.h"
#endif

int igstkNDICommandInterpreterTest( int, char * [] )
{
#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForLinux    CommunicationType;
#endif
  typedef igstk::NDICommandInterpreter  CommandInterpreterType;
  typedef igstk::Logger                 LoggerType; 
   
  // create the logger object
  LoggerType::Pointer   logger = LoggerType::New();
  logger->AddOutputStream( std::cout );
  logger->SetPriorityLevel( igstk::Logger::DEBUG );

  // create the communication object
  CommunicationType::Pointer  serialComm = CommunicationType::New();
  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0() );
  serialComm->SetParity( igstk::SerialCommunication::NoParity() );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600() );
  serialComm->SetByteSize( igstk::SerialCommunication::DataBits8() );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1() );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff() );

  // create the interpreter object
  CommandInterpreterType::Pointer interpreter = CommandInterpreterType::New();

  serialComm->OpenCommunication();

  interpreter->SetCommunication(serialComm);

  // send some commands to the device
  //interpreter->RESET();
  //interpreter->INIT();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
