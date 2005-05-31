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
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;
   
  // create the logger object
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // create the communication object
  CommunicationType::Pointer  serialComm = CommunicationType::New();
  serialComm->SetLogger(logger);
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

  //---------------------------------
  // send some commands to the device
  int i, j, a, ph;
  unsigned long l;
  double vals[8];
  int portHandles[256];
  int numberOfHandles;

  // -- basic initialization commands --
  //interpreter->RESET();  // serial break not implemented yet
  interpreter->INIT();
  interpreter->COMM(NDI_9600, NDI_8N1, NDI_NOHANDSHAKE);
  interpreter->BEEP(2);

  // -- get information about the device --
  std::cout << interpreter->VER(0) << std::endl;
  interpreter->SFLIST(0);

  // -- diagnostic commands, POLARIS only --
  /*
  interpreter->DSTART();
  interpreter->IRCHK(NDI_DETECTED);
  a = interpreter->GetIRCHKDetected();
  interpreter->IRCHK(NDI_SOURCES);
  n = interpreter->GetIRCHKNumberOfSources(0);
  for (i = 0; i < n; i++)
    {
    interpreter->GetIRCHKSourceXY(0, i, vals);
    }
  interpreter->DSTOP();
  */

  // -- enable tool ports --
  interpreter->PHSR(0x02); // all unenabled handles
  numberOfHandles = interpreter->GetPHSRNumberOfHandles();
  for (i = 0; i < numberOfHandles; i++)
    {
    portHandles[i] = interpreter->GetPHSRHandle(i);
    a = interpreter->GetPHSRInformation(i);
    }

  for (i = 0; i < numberOfHandles; i++)
    {
    ph = portHandles[i];
    interpreter->PINIT(ph);
    interpreter->PENA(ph, NDI_DYNAMIC);
    }

  // -- start tracking --
  interpreter->TSTART();
  for (j = 0; j < 50; j++)
    {
    interpreter->TX(NDI_XFORMS_AND_STATUS);
    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      a = interpreter->GetTXTransform(ph, vals);
      a = interpreter->GetTXPortStatus(ph);
      l = interpreter->GetTXFrame(ph);
      }
    }
  interpreter->TSTOP();

  // -- free the tool ports --
  for (i = 0; i < numberOfHandles; i++)
    {
    ph = portHandles[i];
    interpreter->PDIS(ph);
    interpreter->PHF(ph);
    }
  numberOfHandles = 0;

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
