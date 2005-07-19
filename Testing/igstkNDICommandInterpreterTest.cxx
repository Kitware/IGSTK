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
  serialComm->SetUseReadTerminationCharacter(true);
  serialComm->SetReadTerminationCharacter('\r');
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

  std::cout << interpreter << std::endl;
  //---------------------------------
  // send some commands to the device
  int i, j, n, a, ph;
  int errorCode;
  unsigned int l;
  double vals[8];
  int portHandles[256];
  int numberOfHandles;

  // -- basic initialization commands --
  //interpreter->RESET();  // serial break not implemented yet
  std::cout << "Calling INIT" << std::endl;
  interpreter->INIT();
  std::cout << "Calling COMM" << std::endl;
  interpreter->COMM(CommandInterpreterType::NDI_9600,
                    CommandInterpreterType::NDI_8N1,
                    CommandInterpreterType::NDI_NOHANDSHAKE);
  std::cout << "Calling BEEP" << std::endl;
  interpreter->BEEP(2);

  // -- get information about the device --
  std::cout << "Calling VER" << std::endl;
  std::cout << interpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE)
            << std::endl;
  std::cout << "Calling SFLIST" << std::endl;
  interpreter->SFLIST(CommandInterpreterType::NDI_FEATURE_SUMMARY);

  // -- diagnostic commands, POLARIS only --
  std::cout << "Calling DSTART" << std::endl;
  interpreter->DSTART();
  std::cout << "Calling IRCHK" << std::endl;
  interpreter->IRCHK(CommandInterpreterType::NDI_DETECTED);
  errorCode = interpreter->GetError();
  if (errorCode != CommandInterpreterType::NDI_OKAY)
    {
    std::cout << interpreter->ErrorString(errorCode) << std::endl;
    }
  std::cout << "Calling IRCHK" << std::endl;
  interpreter->IRCHK(CommandInterpreterType::NDI_SOURCES);
  if (errorCode != CommandInterpreterType::NDI_OKAY)
    {
    std::cout << interpreter->ErrorString(errorCode) << std::endl;
    }
  n = interpreter->GetIRCHKNumberOfSources(0);
  for (i = 0; i < n; i++)
    {
    interpreter->GetIRCHKSourceXY(0, i, vals);
    }
  std::cout << "Calling DSTOP" << std::endl;
  interpreter->DSTOP();

  // -- enable tool ports --
  std::cout << "Calling PHSR" << std::endl;
  interpreter->PHSR(CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
  numberOfHandles = interpreter->GetPHSRNumberOfHandles();
  for (i = 0; i < numberOfHandles; i++)
    {
    portHandles[i] = interpreter->GetPHSRHandle(i);
    a = interpreter->GetPHSRInformation(i);
    }

  for (i = 0; i < numberOfHandles; i++)
    {
    char toolInformation[32];
    char toolPartNumber[24];
    char portLocation[16];

    ph = portHandles[i];
    // get information about tool
    std::cout << "Calling PHINF" << std::endl;
    interpreter->PHINF(ph,
                       CommandInterpreterType::NDI_BASIC |
                       CommandInterpreterType::NDI_PART_NUMBER |
                       CommandInterpreterType::NDI_ACCESSORIES |
                       CommandInterpreterType::NDI_PORT_LOCATION);
    
    a = interpreter->GetPHINFPortStatus();
    interpreter->GetPHINFToolInfo(toolInformation);
    interpreter->GetPHINFPartNumber(toolPartNumber);
    a = interpreter->GetPHINFAccessories();
    interpreter->GetPHINFPortLocation(portLocation);

    // initialize tool
    std::cout << "Calling PINIT" << std::endl;
    interpreter->PINIT(ph);

    // enable tool according to type
    int mode = 0;
    if (toolInformation[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (toolInformation[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
      { // reference
      mode = CommandInterpreterType::NDI_STATIC;
      }
    else
      { // anything else
      mode = CommandInterpreterType::NDI_DYNAMIC;
      }

    std::cout << "Calling PENA" << std::endl;
    interpreter->PENA(ph, mode);
    }

  // -- start tracking --
  std::cout << "Calling TSTART" << std::endl;
  interpreter->TSTART();
  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling TX" << std::endl;
    interpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
    a = interpreter->GetTXSystemStatus();      

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      a = interpreter->GetTXTransform(ph, vals);
      a = interpreter->GetTXPortStatus(ph);
      l = interpreter->GetTXFrame(ph);
      }
    }

  // -- do one more TX with additional options --
  std::cout << "Calling TX" << std::endl;
  interpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS |
                  CommandInterpreterType::NDI_ADDITIONAL_INFO);

  for (i = 0; i < numberOfHandles; i++)
    {
    ph = portHandles[i];
    a = interpreter->GetTXTransform(ph, vals);
    a = interpreter->GetTXPortStatus(ph);
    l = interpreter->GetTXFrame(ph);
    a = interpreter->GetTXPortStatus(ph);
    a = interpreter->GetTXToolInfo(ph);
    }

  std::cout << "Calling TSTOP" << std::endl;
  interpreter->TSTOP();

  // -- free the tool ports --
  for (i = 0; i < numberOfHandles; i++)
    {
    ph = portHandles[i];
    std::cout << "Calling PDIS" << std::endl;    
    interpreter->PDIS(ph);
    std::cout << "Calling PHF" << std::endl;    
    interpreter->PHF(ph);
    }
  numberOfHandles = 0;

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
