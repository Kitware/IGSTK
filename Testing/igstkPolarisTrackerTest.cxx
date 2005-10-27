/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerTest.cxx
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

#include "igstkSystemInformation.h"
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "igstkSerialCommunicationSimulator.h"
#include "igstkPolarisTracker.h"
#include "igstkTransform.h"

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
    std::cout << event.GetEventName() << std::endl;
  }
};


#ifdef IGSTK_SIMULATOR_TEST
int igstkPolarisTrackerSimulatedTest( int, char * [] )
#else  /* IGSTK_SIMULATOR_TEST */
int igstkPolarisTrackerTest( int, char * [] )
#endif
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::PolarisTrackerTool::Pointer tool = igstk::PolarisTrackerTool::New();
  std::cout << tool->GetNameOfClass() << std::endl;
  std::cout << tool << std::endl;

#ifdef IGSTK_SIMULATOR_TEST
    igstk::SerialCommunicationSimulator::Pointer serialComm = igstk::SerialCommunicationSimulator::New();
#else  /* IGSTK_SIMULATOR_TEST */
#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer serialComm = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */
#endif /* IGSTK_SIMULATOR_TEST */

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG); //DEBUG );

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  char pathToCaptureFile[1024];
  joinDirAndFile( pathToCaptureFile, 1024,
                  IGSTK_DATA_ROOT,
                  "polaris_stream_09_29_2005.txt" );
  serialComm->SetFileName( pathToCaptureFile );
#else /* IGSTK_SIMULATOR_TEST */
  serialComm->SetCaptureFileName( "RecordedStreamByPolarisTrackerTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::PolarisTracker::Pointer  tracker;

  tracker = igstk::PolarisTracker::New();

  tracker->SetLogger( logger );

  std::cout << "Entering  SetCommunication ..." << std::endl;

  tracker->SetCommunication( serialComm );

  std::cout << "Exited SetCommunication ..." << std::endl;

  tracker->Open();

#ifdef IGSTK_SIMULATOR_TEST
  char pathToSROMFile[1024];
  joinDirAndFile( pathToSROMFile, 1024,
                  IGSTK_DATA_ROOT,
                  "ta2p0003-3-120.rom" );
  tracker->AttachSROMFileNameToPort( 3, pathToSROMFile );
#endif /* IGSTK_SIMULATOR_TEST */

  std::cout << tracker << std::endl;

  tracker->Initialize();

  unsigned int ntools = tracker->GetNumberOfTools();

  std::cout << "number of tools : " << ntools << std::endl;

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
  
  tracker->Reset();
  
  tracker->Initialize();
  
  tracker->StartTracking();

  tracker->StopTracking();

  tracker->Close();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
