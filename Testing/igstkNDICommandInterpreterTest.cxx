/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreterTest.cxx
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

// for memset
#include <string.h>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkSerialCommunication.h"
#include "igstkSerialCommunicationSimulator.h"


class NDICommandInterpreterTestCommand : public itk::Command 
{
public:

  typedef NDICommandInterpreterTestCommand  Self;
  typedef itk::Command                      Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  itkNewMacro( Self );

protected:
  NDICommandInterpreterTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
    const igstk::NDIErrorEvent * ndiEvent = 
      dynamic_cast< const igstk::NDIErrorEvent * >( &event );

    if( ndiEvent )
      {
      int errorCode = ndiEvent->GetErrorCode();
 
      std::cout << "NDI Error " << std::showbase << std::hex << errorCode
                << std::dec << std::noshowbase << ": "
                << igstk::NDICommandInterpreter::ErrorString(errorCode)
                << "." << std::endl;
      }
    }
};

#ifdef IGSTK_SIMULATOR_TEST
int igstkNDICommandInterpreterSimulatedTest( int argc, char * argv[] )
#else  /* IGSTK_SIMULATOR_TEST */
int igstkNDICommandInterpreterTest( int argc, char * argv[] )
#endif
{

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
    {
    std::cerr << "Error missing argument " << std::endl;
    std::cerr << "Usage:  " << argv[0]  
              << "Test_Output_Directory" 
              << "Port_number" 
              << "Data_root" << std::endl;
    return EXIT_FAILURE;
    }


#ifdef IGSTK_SIMULATOR_TEST
  typedef igstk::SerialCommunicationSimulator   CommunicationType;
#else  /* IGSTK_SIMULATOR_TEST */
  typedef igstk::SerialCommunication            CommunicationType;
#endif /* IGSTK_SIMULATOR_TEST */

  typedef igstk::NDICommandInterpreter    CommandInterpreterType;
  typedef igstk::Object::LoggerType       LoggerType;
  typedef itk::StdStreamLogOutput         LogOutputType;
   
  NDICommandInterpreterTestCommand::Pointer errorCommand =
    NDICommandInterpreterTestCommand::New();
   
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

  // create the communication object
  CommunicationType::Pointer  serialComm = CommunicationType::New();
  serialComm->SetLogger(logger);

  // create the interpreter object
  CommandInterpreterType::Pointer interpreter = CommandInterpreterType::New();

  // set an observer for the interpreter
  interpreter->AddObserver( igstk::NDIErrorEvent(), errorCommand );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  std::string igstkDataDirectory = argv[3];
  std::string simulationFile = igstkDataDirectory + "/";
  simulationFile = simulationFile + 
    "Input/polaris_stream_08_31_2005_NDICommandInterpreter.txt";
  serialComm->SetFileName( simulationFile.c_str() );
#else /* IGSTK_SIMULATOR_TEST */
  // capture a fresh data file
  serialComm->SetCaptureFileName( 
                          "PolarisCaptureForNDICommandInterpreterTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  unsigned int igstk_Test_Polaris_Port_Number = atoi(argv[2]);
  typedef igstk::SerialCommunication::PortNumberType  PortNumberType;
  PortNumberType polarisPortNumber =  
                  PortNumberType( igstk_Test_Polaris_Port_Number );

  serialComm->SetPortNumber(polarisPortNumber);
  serialComm->OpenCommunication();

  // NDICommandInterpreter will set the communication parameters itself
  interpreter->SetCommunication(serialComm);
  // GetCommunication for coverage
  if (interpreter->GetCommunication() != serialComm)
    {
    return EXIT_FAILURE;
    }

  std::cout << interpreter << std::endl;
  //---------------------------------
  // send some commands to the device
  int i, j, n, a, ph;
  unsigned int l;
  double vals[8] = { 0, };
  int portHandles[256];
  int numberOfHandles;
  int errorCode = 0;

  // -- check the error code function
  std::cout << "Initial error code should be \"No Error\"" << std::endl;
  errorCode = interpreter->GetError();
  std::cout << interpreter->ErrorString(errorCode) << std::endl;
  std::cout << "Other errors:" << std::endl;
  std::cout << "Bad CRC on data sent to device:" << std::endl;
  std::cout << interpreter->ErrorString(
                    CommandInterpreterType::NDI_BAD_COMMAND_CRC) << std::endl;
  std::cout << "Bad CRC on data received from device:" << std::endl;
  std::cout << interpreter->ErrorString(
                            CommandInterpreterType::NDI_BAD_CRC) << std::endl;
  std::cout << "Background infrared is too bright, can't track:" << std::endl;
  std::cout << interpreter->ErrorString(
                        CommandInterpreterType::NDI_ENVIRONMENT) << std::endl;
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
    std::cout << "Test FAILED !" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Calling COMM" << std::endl;
  interpreter->COMM(CommandInterpreterType::NDI_38400,
                    CommandInterpreterType::NDI_8N1,
                    CommandInterpreterType::NDI_NOHANDSHAKE);
  std::cout << "Calling BEEP" << std::endl;
  interpreter->BEEP(2);

  // -- get information about the device --
  std::cout << "Calling VER" << std::endl;
  interpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE);
  std::cout << interpreter->GetVERText() << std::endl;
  std::cout << "Calling SFLIST" << std::endl;
  interpreter->SFLIST(CommandInterpreterType::NDI_FEATURE_SUMMARY);

  std::cout << "Calling SSTAT" << std::endl;
  interpreter->SSTAT(CommandInterpreterType::NDI_CONTROL);
  a = interpreter->GetSSTATControl();
  std::cout << "SSTATControl : " << std::hex << std::showbase << a <<std::endl;
  interpreter->SSTAT(CommandInterpreterType::NDI_SENSORS);
  a = interpreter->GetSSTATSensors();
  std::cout << "SSTATSensors : " << std::hex << std::showbase << a <<std::endl;
  interpreter->SSTAT(CommandInterpreterType::NDI_TIU);
  a = interpreter->GetSSTATTIU();
  std::cout << "SSTATTIU : " << std::hex << std::showbase << a << std::endl;

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
  a = interpreter->GetIRCHKDetected();
  std::cout << "IRCHKDetected : " << std::dec << a << std::endl;
  std::cout << "Calling IRCHK" << std::endl;
  interpreter->IRCHK(CommandInterpreterType::NDI_SOURCES);
  if (errorCode != CommandInterpreterType::NDI_OKAY)
    {
    std::cout << interpreter->ErrorString(errorCode) << std::endl;
    }
  n = interpreter->GetIRCHKNumberOfSources(CommandInterpreterType::NDI_LEFT);
  std::cout << "IRCHK Number Of Sources : " << std::dec << n << std::endl;
  for (i = 0; i < n; i++)
    {
    std::cout << "IRCHK Source XY (LEFT) : " << interpreter->GetIRCHKSourceXY(
                                   CommandInterpreterType::NDI_LEFT, i, vals);
    std::cout << "\tX : " << vals[0] << "\tY : " << vals[1] << std::endl;
    }
  interpreter->GetIRCHKSourceXY(CommandInterpreterType::NDI_LEFT, 0, vals);
  n = interpreter->GetIRCHKNumberOfSources(CommandInterpreterType::NDI_RIGHT);
  for (i = 0; i < n; i++)
    {
    std::cout << "IRCHK Source XY (LEFT) : " << interpreter->GetIRCHKSourceXY(
                                  CommandInterpreterType::NDI_RIGHT, i, vals);
    std::cout << "\tX : " << vals[0] << "\tY : " << vals[1] << std::endl;
    }
  interpreter->GetIRCHKSourceXY(CommandInterpreterType::NDI_RIGHT, 0, vals);
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
  std::string romDataRoot = argv[3];
  std::string romFileName = romDataRoot +
    "/Input/polaris_passive_pointer_1.rom";
  
  std::ifstream romFile;
  romFile.open( romFileName.c_str() );
  romFile.read( data, 1024 );
  romFile.close();

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
  std::cout << "PHSR Number of Handles : " 
            << std::dec << numberOfHandles << std::endl;
  for (i = 0; i < numberOfHandles; i++)
    {
    portHandles[i] = interpreter->GetPHSRHandle(i);
    a = interpreter->GetPHSRInformation(i);
    std::cout << "PHSR Handle : " << std::hex << std::showbase
              << portHandles[i] << "    PHSRInformation : " 
              << std::hex << std::showbase << a << std::endl;
    }

  for (i = 0; i < numberOfHandles; i++)
    {
    // initialize tool
    std::cout << "Calling PINIT " << std::endl;
    ph = portHandles[i];
    interpreter->PINIT(ph);
    }

  std::cout << "Calling PHSR" << std::endl;
  interpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);
  numberOfHandles = interpreter->GetPHSRNumberOfHandles();
  std::cout << "PHSR Number of Handles : " << numberOfHandles << std::endl;
  for (i = 0; i < numberOfHandles; i++)
    {
    portHandles[i] = interpreter->GetPHSRHandle(i);
    a = interpreter->GetPHSRInformation(i);
    std::cout << "PHSR Handle : " << portHandles[i] 
              << "    PHSRInformation : " << a << std::endl;
    }

  for (i = 0; i < numberOfHandles; i++)
    {
    char toolInformation[32];
    char toolPartNumber[24];
    char portLocation[16];
    int toolType;

    ph = portHandles[i];
    // get information about tool
    std::cout << "Calling PHINF" << std::endl;
    interpreter->PHINF(ph,
                       CommandInterpreterType::NDI_BASIC |
                       CommandInterpreterType::NDI_PART_NUMBER |
                       CommandInterpreterType::NDI_ACCESSORIES |
                       CommandInterpreterType::NDI_PORT_LOCATION);
    a = interpreter->GetPHINFPortStatus();
    std::cout << "PHINF Port Status : " << a << std::endl;
    interpreter->GetPHINFToolInfo(toolInformation);
    std::cout << "PHINF Tool Info : " << toolInformation << std::endl;
    interpreter->GetPHINFPartNumber(toolPartNumber);
    std::cout << "PHINF Part Number : " << toolPartNumber << std::endl;
    toolType = interpreter->GetPHINFToolType();
    std::cout << "PHINF Tool Type : " << toolType << std::endl;
    a = interpreter->GetPHINFAccessories();
    std::cout << "PHINF Accessories : " << a << std::endl;
    interpreter->GetPHINFPortLocation(portLocation);
    std::cout << "PHINF Port Location : " << portLocation << std::endl;

    // check if this is a wired tool
    if (portLocation[9] == '0')
      {
      std::cout << "Calling PHINF with wired-tool specifics" << std::endl;
      interpreter->PHINF(ph,
                         CommandInterpreterType::NDI_TESTING |
                         CommandInterpreterType::NDI_MARKER_TYPE);
      a = interpreter->GetPHINFMarkerType();
      std::cout << "PHINF MarkerType : " << a << std::endl;
      a = interpreter->GetPHINFCurrentTest();
      std::cout << "PHINF CurrentTest : " << a << std::endl;
      }

    // enable tool according to type
    int mode = 0;
    if (toolType == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (toolType == CommandInterpreterType::NDI_TYPE_REFERENCE)
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

  // -- do 50 basic BX calls
  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling BX #" << std::dec << j << std::endl;
    interpreter->BX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
    a = interpreter->GetBXSystemStatus();
    std::cout << "  BX system status : " << std::hex << std::showbase
              << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      std::cout << "    port[" << std::hex << std::showbase << ph << "] : "
                << std::endl;

      a = interpreter->GetBXTransform(ph, vals);

      std::cout << "      Transform : {return:"
                << std::hex << std::showbase << a << "}" << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::scientific << std::setprecision(7) << std::showpos
                  << "        " << vals[0] << " " << vals[1] << " "
                  << vals[2] << ", " << vals[3] << std::endl 
                  << "        " << vals[4] << " " << vals[5] << " "
                  << vals[6] << std::endl
                  << "        " << vals[7] << std::noshowpos << std::endl;
        }

      a = interpreter->GetBXPortStatus(ph);

      std::cout << "      PortStatus : " << std::hex << std::showbase
                << a << std::endl;

      l = interpreter->GetBXFrame(ph);

      std::cout << "      Frame : " << std::dec << l << std::endl;
      }
    }

  std::cout << std::endl;
  // -- do some more BX commands, with passive stray tracking

  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling BX with NDI_PASSIVE_STRAY" << std::endl;

    interpreter->BX(CommandInterpreterType::NDI_PASSIVE_STRAY);
    a = interpreter->GetBXSystemStatus();
    std::cout << "  BXSystemStatus : " << std::hex << std::showbase
              << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      std::cout << "    port[" << std::hex << std::showbase << ph << "] : "
                << std::endl;

      a = interpreter->GetBXTransform(ph, vals);
      std::cout << "      Transform : {return:" << std::hex << a << "}"
                << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::scientific << std::setprecision(7) << std::showpos
                  << "        " << vals[0] << " " << vals[1] << " "
                  << vals[2] << ", " << vals[3] << std::endl 
                  << "        " << vals[4] << " " << vals[5] << " "
                  << vals[6] << std::endl
                  << "        " << vals[7] << std::noshowpos << std::endl;
        }

      a = interpreter->GetBXPortStatus(ph);
      std::cout << "      PortStatus : " << std::hex << a << std::endl;

      l = interpreter->GetBXFrame(ph);
      std::cout << "      Frame : " << std::dec << l << std::endl;
      }

    n = interpreter->GetBXNumberOfPassiveStrays();
    std::cout << "  NumberOfPassiveStrays : " << std::dec << n << std::endl;
    for (i = 0; i < n; i++)
      {
      double coord[3];
      a = interpreter->GetBXPassiveStray(i, coord);
      std::cout << "    PassiveStray : #" << std::dec << i << std::endl;
      std::cout << "      OutOfVolume : " << std::dec
                << interpreter->GetBXPassiveStrayOutOfVolume(i) << std::endl;
      std::cout << std::scientific << std::setprecision(7) << std::showpos
                << "        " << coord[0] << " " << coord[1] << " " << coord[2]
                << std::noshowpos << std::endl;
      }
    std::cout << std::endl;
    }
  std::cout << std::endl;

  // -- do 50 calls with NDI_SINGLE_STRAY
  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling BX with NDI_SINGLE_STRAY" << std::endl;
    interpreter->BX(CommandInterpreterType::NDI_XFORMS_AND_STATUS |
                    CommandInterpreterType::NDI_SINGLE_STRAY);
    a = interpreter->GetBXSystemStatus();
    std::cout << "  BXSystemStatus : " << std::hex << std::showbase
              << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      double coord[3];
      ph = portHandles[i];
      std::cout << "    port[" << std::hex << std::showbase << ph << "] : "
                << std::endl;

      a = interpreter->GetBXSingleStray(ph, coord);
      std::cout << "      SingleStray : {return:" << std::hex << std::showbase
                << a << "}" << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << "      " << std::scientific << std::setprecision(7) 
                  << coord[0] << " " << coord[1] << " " << coord[2]
                  << std::endl;
        }

      a = interpreter->GetBXTransform(ph, vals);
      std::cout << "      Transform : {return:"
                << std::hex << std::showbase << a << "}" << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::scientific << std::setprecision(7) << std::showpos
                  << "        " << vals[0] << " " << vals[1] << " "
                  << vals[2] << ", " << vals[3] << std::endl 
                  << "        " << vals[4] << " " << vals[5] << " "
                  << vals[6] << std::endl
                  << "        " << vals[7] << std::noshowpos << std::endl;
        }

      a = interpreter->GetBXPortStatus(ph);
      std::cout << "      PortStatus : " << std::hex << std::showbase 
                << a << std::endl;

      l = interpreter->GetBXFrame(ph);
      std::cout << "      Frame : " << std::dec << l << std::endl;
      }
    }
  std::cout << std::endl;

  // -- do one more BX with additional options --
  std::cout << "Calling BX with NDI_ADDITIONAL_INFO" << std::endl;
  interpreter->BX(CommandInterpreterType::NDI_XFORMS_AND_STATUS |
                  CommandInterpreterType::NDI_ADDITIONAL_INFO);

  for (i = 0; i < numberOfHandles; i++)
    {
    ph = portHandles[i];
      std::cout << "  port[" << ph << "] : " << std::endl;

    a = interpreter->GetBXTransform(ph, vals);
    std::cout << "      Transform : {return:" << std::hex << a << "}"
              << std::endl;
    if (a == CommandInterpreterType::NDI_VALID)
      {
      std::cout << std::scientific << std::setprecision(7) << std::showpos
                << "        " << vals[0] << " " << vals[1] << " "
                << vals[2] << ", " << vals[3] << std::endl 
                << "        " << vals[4] << " " << vals[5] << " "
                << vals[6] << std::endl
                << "        " << vals[7] << std::noshowpos << std::endl;
      }

    a = interpreter->GetBXPortStatus(ph);
    std::cout << "  PortStatus : " << std::hex << std::showbase
              << a << std::endl;
    l = interpreter->GetBXFrame(ph);
    std::cout << "  Frame : " << std::dec << l << std::endl;
    a = interpreter->GetBXToolInfo(ph);
    std::cout << "  ToolInfo : " << std::hex << std::showbase
              << a << std::endl;
    a = interpreter->GetBXMarkerInfo(ph, 0);
    std::cout << "  MarkerInfo : " << std::hex << std::showbase
              << a << std::endl;
    }
  std::cout << std::endl;

  // -- do 50 basic TX calls
  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling TX" << std::endl;
    interpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
    a = interpreter->GetTXSystemStatus(); 
    std::cout << "TX System Status : " << std::hex << std::showbase
              << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      a = interpreter->GetTXTransform(ph, vals);
      std::cout << "TX Transform return value : " << std::hex
                << std::showbase << a << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[0] << " " << vals[1] << " " << vals[2] << " " 
                  << vals[3] << " ";
        std::cout << std::fixed << std::setprecision(2) << std::showpos
                  << vals[4] << " " << vals[5] << " " << vals[6] << " ";
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[7] << std::noshowpos << std::endl;
        }
      a = interpreter->GetTXPortStatus(ph);
      std::cout << "TX Port Status : " << std::hex << std::showbase
                << a << std::endl;
      l = interpreter->GetTXFrame(ph);
      std::cout << "TX Frame : " << std::dec << l << std::endl;
      }
    }

  // -- do some more TX commands, with passive stray tracking

  for (j = 0; j < 50; j++)
    {
    std::cout << "Calling TX wth NDI_PASSIVE_STRAY" << std::endl;
    interpreter->TX(CommandInterpreterType::NDI_PASSIVE_STRAY);
    a = interpreter->GetTXSystemStatus();
    std::cout << "TX System Status : " << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      ph = portHandles[i];
      a = interpreter->GetTXTransform(ph, vals);
      std::cout << "TX Transform return value : " << std::hex
                << std::showbase << a << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[0] << " " << vals[1] << " " << vals[2] << " " 
                  << vals[3] << " ";
        std::cout << std::fixed << std::setprecision(2) << std::showpos
                  << vals[4] << " " << vals[5] << " " << vals[6] << " ";
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[7] << std::noshowpos << std::endl;
        }
      a = interpreter->GetTXPortStatus(ph);
      std::cout << "TX PortStatus : " << std::hex << std::showbase
                << a << std::endl;
      l = interpreter->GetTXFrame(ph);
      std::cout << "TX Frame : " << std::dec << l << std::endl;
      }

    n = interpreter->GetTXNumberOfPassiveStrays();
    std::cout << "TX NumberOfPassiveStrays : " << std::dec << n << std::endl;
    for (i = 0; i < n; i++)
      {
      double coord[3];
      a = interpreter->GetTXPassiveStray(i, coord);
      std::cout << std::dec << interpreter->GetTXPassiveStrayOutOfVolume(i)
                << ":";
      std::cout << std::fixed << std::setprecision(2) << std::showpos
                << coord[0] << " " << coord[1] << " " << coord[2]
                << std::noshowpos << std::endl;
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
    std::cout << "TX System Status : " << std::hex << std::showbase
              << a << std::endl;

    for (i = 0; i < numberOfHandles; i++)
      {
      double coord[3];
      ph = portHandles[i];
      a = interpreter->GetTXSingleStray(ph, coord);
      std::cout << "TX Single Stray return value : " << std::hex
                << std::showbase << a << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::fixed << std::setprecision(2) << std::showpos
                  << coord[0] << " " << coord[1] << " " << coord[2]
                  << std::noshowpos << std::endl;
        }
      a = interpreter->GetTXTransform(ph, vals);
      std::cout << "TX Transform return value : " << std::hex
                << std::showbase << a << std::endl;
      if (a == CommandInterpreterType::NDI_VALID)
        {
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[0] << " " << vals[1] << " " << vals[2] << " " 
                  << vals[3] << " ";
        std::cout << std::fixed << std::setprecision(2) << std::showpos
                  << vals[4] << " " << vals[5] << " " << vals[6] << " ";
        std::cout << std::fixed << std::setprecision(4) << std::showpos
                  << vals[7] << std::noshowpos << std::endl;
        }
      a = interpreter->GetTXPortStatus(ph);
      std::cout << "TX Port Status : " << std::hex << std::showbase
                << a << std::endl;
      l = interpreter->GetTXFrame(ph);
      std::cout << "TX Frame : " << std::dec << l << std::endl;
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
    std::cout << "TX Transform return value : " << std::hex
              << std::showbase << a << std::endl;
    std::cout << std::fixed << std::setprecision(4) << std::showpos
              << vals[0] << " " << vals[1] << " " << vals[2] << " " 
              << vals[3] << " ";
    std::cout << std::fixed << std::setprecision(2) << std::showpos
              << vals[4] << " " << vals[5] << " " << vals[6] << " ";
    std::cout << std::fixed << std::setprecision(4) << std::showpos
              << vals[7] << std::noshowpos << std::endl;
    a = interpreter->GetTXPortStatus(ph);
    std::cout << "TX Port Status : " << std::hex << std::showbase
              << a << std::endl;
    l = interpreter->GetTXFrame(ph);
    std::cout << "TX Frame : " << std::dec << l << std::endl;
    a = interpreter->GetTXPortStatus(ph);
    std::cout << "TX Port Status : " << std::hex << std::showbase
              << a << std::endl;
    a = interpreter->GetTXToolInfo(ph);
    std::cout << "TX Tool Info : " << std::hex << std::showbase
              << a << std::endl;
    for (j = 0; j < 20; j++)
      {
      a = interpreter->GetTXMarkerInfo(ph, j);
      std::cout << "TX Marker Info (" << std::dec << j << ") : "
                << std::hex << std::showbase << a << std::endl;
      }
    }

  std::cout << "Calling TSTOP" << std::endl;
  interpreter->TSTOP();

  // call PHINF again
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
    std::cout << "PHINF Port Status : " << std::hex << std::showbase
              << a << std::endl;
    interpreter->GetPHINFToolInfo(toolInformation);
    std::cout << "PHINF Tool Info : " << toolInformation << std::endl;
    interpreter->GetPHINFPartNumber(toolPartNumber);
    std::cout << "PHINF Part Number : " << toolPartNumber << std::endl;
    a = interpreter->GetPHINFToolType();
    std::cout << "PHINF Tool Type : " << std::hex << std::showbase
              << a << std::endl;
    a = interpreter->GetPHINFAccessories();
    std::cout << "PHINF Accessories : " << std::hex << std::showbase
              << a << std::endl;
    interpreter->GetPHINFPortLocation(portLocation);
    std::cout << "PHINF Port Location : " << portLocation << std::endl;

    // check if this is a wired tool
    if (portLocation[9] == '0')
      {
      std::cout << "Calling PHINF with wired-tool specifics" << std::endl;
      interpreter->PHINF(ph,
                         CommandInterpreterType::NDI_TESTING |
                         CommandInterpreterType::NDI_MARKER_TYPE);
      a = interpreter->GetPHINFMarkerType();
      std::cout << "PHINF Marker Type : " << std::hex << std::showbase
                << a << std::endl;
      a = interpreter->GetPHINFCurrentTest();
      std::cout << "PHINF Current Test : " << std::hex << std::showbase
                << a << std::endl;
      }
    }

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
  
  // to do check, need a full uppercase hex version
  char hexString3[22];  // extra space for null at end
  for (i = 0; i < 22; i++)
    {
    if (hexString[i] == '\0')
      {
      hexString3[i] = '\0';
      break;
      }
    hexString3[i] = toupper(hexString[i]);
    }

  // do the check
  if (strcmp(hexString3, hexString2) != 0)
    {
    std::cout << "Encode/Decode mismatch: \"" << hexString << "\" != \""
              << hexString2 << "\"" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
