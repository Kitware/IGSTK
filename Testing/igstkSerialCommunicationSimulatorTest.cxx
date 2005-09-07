/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulatorTest.cxx
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

#include <string.h>

#include "itkCommand.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkSystemInformation.h"
#include "igstkSerialCommunicationSimulator.h"

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
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetTransferParametersFailureEvent ))
    {
        std::cout << "SetupCommunicationParametersFailureEvent Error Occurred ...\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteSuccessEvent ))
    {
        std::cout << "****** WriteSuccessEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteFailureEvent ))
    {
        std::cout << "****** WriteFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::WriteTimeoutEvent ))
    {
        std::cout << "****** WriteTimeoutEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadSuccessEvent ))
    {
        std::cout << "****** ReadSuccessEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadFailureEvent ))
    {
        std::cout << "****** ReadFailureEvent ******\n";
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReadTimeoutEvent ))
    {
        std::cout << "****** ReadTimeoutEvent ******\n";
    }
   else 
    {
        std::cout << "Some other Error Occurred ...\n";
    }
 }
};


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


int igstkSerialCommunicationSimulatorTest( int, char * [] )
{
  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

  igstk::SerialCommunicationSimulator::Pointer serialComm = igstk::SerialCommunicationSimulator::New();

  std::cout << serialComm->GetNameOfClass() << std::endl;

  // for increasing test coverage
  serialComm->SetFileName("wrong_name");
  serialComm->OpenCommunication();

  // set the name of the actual data file
  char fullName[1024];
  joinDirAndFile( fullName, 1024,
                  IGSTK_DATA_ROOT,
                  "Input/polaris_stream_07_27_2005.bin" );
  std::cout << fullName << std::endl; 
  serialComm->SetFileName( fullName );
  serialComm->GetFileName();

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  std::cout << serialComm << std::endl;

  serialComm->OpenCommunication();

  igstk::AuroraTracker::Pointer  tracker;

  tracker = igstk::AuroraTracker::New();

  tracker->SetLogger( logger );

  std::cout << "Entering  SetCommunication ..." << std::endl;

  tracker->SetCommunication( serialComm );

  std::cout << "Exited SetCommunication ..." << std::endl;

  tracker->Open();

  tracker->AttachSROMFileNameToPort( 0, "C:/Program Files/Northern Digital Inc/SROM Image Files/5D.ROM" );

  std::cout << tracker << std::endl;

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

  tracker->Close();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}
