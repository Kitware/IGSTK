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

#include <ctype.h>
#include <stdio.h>

#include "itkCommand.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkNDICommandInterpreter.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif
#include "igstkSerialCommunicationSimulator.h"


/** append a file name to a directory name and provide the result */
static void joinDirAndFile(char *result, int maxLen,
                           const char *dirName, const char *fileName)
{
  int dirNameLen = strlen( dirName );
  int fileNameLen = strlen( fileName );
  const char* slash = ( (dirName[dirNameLen-1] == '/') ? "" : "/" );
  int slashLen = strlen( slash );

  // allocate temporary string, concatenate
  char* fullName = new char[dirNameLen + slashLen + fileNameLen + 1];
  strncpy(&fullName[0], dirName, dirNameLen);
  strncpy(&fullName[dirNameLen], slash, slashLen);
  strncpy(&fullName[dirNameLen + slashLen], fileName, fileNameLen);
  fullName[dirNameLen + slashLen + fileNameLen] = '\0';

  // copy to the result
  strncpy(result, fullName, maxLen);
  result[maxLen-1] = '\0';

  // delete temporary string
  delete [] fullName;
}

int igstkNDICommandInterpreterTest( int, char * [] )
{
//#ifdef WIN32
//  typedef igstk::SerialCommunicationForWindows  CommunicationType;
//#else
//  typedef igstk::SerialCommunicationForPosix    CommunicationType;
//#endif
  typedef igstk::SerialCommunicationSimulator     CommunicationType;
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

  // create the interpreter object
  CommandInterpreterType::Pointer interpreter = CommandInterpreterType::New();

  // set the name of the actual data file
  char fullName[1024];
  joinDirAndFile( fullName, 1024,
                  IGSTK_DATA_ROOT,
                  "Input/polaris_stream_08_18_2005.bin" );
  serialComm->SetFileName( fullName );

  // uncomment to capture a new file
  //serialComm->SetCaptureFileName( "PolarisCaptureForNDICommandInterpreterTest.bin" );
  //serialComm->SetCapture( true );
  serialComm->SetPortNumber(CommunicationType::PortNumber0);

  serialComm->OpenCommunication();

  // NDICommandInterpreter will set the communication parameters itself
  interpreter->SetCommunication(serialComm);

  std::cout << interpreter << std::endl;
  //---------------------------------
  // send some commands to the device
  int i, j, n, a, ph;
  unsigned int l;
  double vals[8];
  int portHandles[256];
  int numberOfHandles;
  int errorCode = 0;

  // -- check the error code function
  std::cout << "Initial error code should be \"No Error\"" << std::endl;
  errorCode = interpreter->GetError();
  std::cout << interpreter->ErrorString(errorCode) << std::endl;
  std::cout << "Other errors:" << std::endl;
  std::cout << "Bad CRC on data sent to device:" << std::endl;
  std::cout << interpreter->ErrorString(CommandInterpreterType::NDI_BAD_COMMAND_CRC) << std::endl;
  std::cout << "Bad CRC on data received from device:" << std::endl;
  std::cout << interpreter->ErrorString(CommandInterpreterType::NDI_BAD_CRC) << std::endl;
  std::cout << "Background infrared is too bright, can't track:" << std::endl;
  std::cout << interpreter->ErrorString(CommandInterpreterType::NDI_ENVIRONMENT) << std::endl;
  std::cout << "This error doesn't exist:" << std::endl;
  std::cout << interpreter->ErrorString(0x80) << std::endl;

  // -- the most basic test is to see if the device will reset --
  std::cout << "Sending RESET" << std::endl;
  interpreter->RESET();

  // -- basic initialization commands --
  std::cout << "Calling INIT" << std::endl;
  interpreter->INIT();
  if (interpreter->GetError())
    {
    std::cout << "Failed to init, device must not be attached" << std::endl;
    // set a fast timeout so that the test won't take forever
    serialComm->SetTimeoutPeriod(100);
    }
  std::cout << "Calling COMM" << std::endl;
  interpreter->COMM(CommandInterpreterType::NDI_38400,
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

  std::cout << "Calling SSTAT" << std::endl;
  interpreter->SSTAT(CommandInterpreterType::NDI_CONTROL);
  a = interpreter->GetSSTATControl();
  interpreter->SSTAT(CommandInterpreterType::NDI_SENSORS);
  a = interpreter->GetSSTATSensors();
  interpreter->SSTAT(CommandInterpreterType::NDI_TIU);
  a = interpreter->GetSSTATTIU();

  // -- diagnostic commands, POLARIS only --
  std::cout << "Calling DSTART" << std::endl;
  interpreter->DSTART();
  std::cout << "Calling IRINIT" << std::endl;
  interpreter->IRINIT();
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

  // -- request a passive port on a POLARIS--
  interpreter->PHRQ("********",  // device number (any device)
                    "*",         // any system
                    "*",         // wireless
                    "0A",        // specifically request wireless port A
                    "**");       // AURORA tool channel (not used for POLARIS)
  ph = interpreter->GetPHRQHandle();


  // -- write a virtual SROM to this port
  char data[1024]; // to hold the srom data
  memset( data, 0, 1024 );
  joinDirAndFile( fullName, 1024,
                  IGSTK_DATA_ROOT,
                  "Input/polaris_passive_pointer_1.rom" );
  FILE *file = fopen( fullName, "rb" );
  fread( data, 1, 1024, file );
  fclose( file );
  for ( i = 0; i < 1024; i += 64 )
    {
    // convert data to hexidecimal and write to virtual SROM in
    // 64-byte chunks, as described in the manual
    char hexbuffer[128]; // holds hexidecimal data to be sent to device
    interpreter->HexEncode(hexbuffer, &data[i], 64);
    interpreter->PVWR(ph, i, hexbuffer);
    }

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

  // -- do 50 basic TX calls
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

  // -- do some more TX commands, with passive stray tracking

  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling TX wth NDI_PASSIVE_STRAY" << std::endl;
    interpreter->TX(CommandInterpreterType::NDI_PASSIVE_STRAY);
    a = interpreter->GetTXSystemStatus();      

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      a = interpreter->GetTXTransform(ph, vals);
      a = interpreter->GetTXPortStatus(ph);
      l = interpreter->GetTXFrame(ph);
      }

    n = interpreter->GetTXNumberOfPassiveStrays();
    std::cout << n << ": ";
    for (i = 0; i < n; i++)
      {
      double coord[3];
      a = interpreter->GetTXPassiveStray(i, coord);
      std::cout << "(" << coord[0] << "," << coord[1] << "," << coord[2]
                << "), ";
      }
    std::cout << std::endl;
    }

  // -- do 50 calls with NDI_SINGLE_STRAY
  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling TX with NDI_SINGLE_STRAY" << std::endl;
    interpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS |
                    CommandInterpreterType::NDI_SINGLE_STRAY);
    a = interpreter->GetTXSystemStatus();      

    for (i = 0; i < numberOfHandles; i++)
      {
      double coord[3];
      ph = portHandles[i];
      a = interpreter->GetTXSingleStray(ph, coord);
      a = interpreter->GetTXTransform(ph, vals);
      a = interpreter->GetTXPortStatus(ph);
      l = interpreter->GetTXFrame(ph);
      }
    }

  // -- do one more TX with additional options --
  std::cout << "Calling TX with NDI_ADDITIONAL_INFO" << std::endl;
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

  // -- test the utility functions --
  std::cout << "Testing HexDecode/HexEncode" << std::endl;
  const char *hexString = "789ABC0123def45600ff";
  char hexString2[22];  // extra space for null at end
  unsigned char binaryString[10];

  CommandInterpreterType::HexDecode(binaryString,
                                    hexString,
                                    10);
  CommandInterpreterType::HexEncode(hexString2,
                                    binaryString,
                                    10);
  // HexEncode does not null-terminate, so do it here
  hexString2[20] = '\0';
  
  // to do check, need a full uppercase version
  char hexString3[22];  // extra space for null at end
  strncpy(hexString3, hexString, 22);
  for (char *cp = hexString3; *cp != '\0'; cp++)
    {
    *cp = toupper(*cp);
    }

  if (strcmp(hexString3, hexString2) != 0)
    {
    std::cout << "Encode/Decode mismatch: \"" << hexString << "\" != \""
              << hexString2 << "\"" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
