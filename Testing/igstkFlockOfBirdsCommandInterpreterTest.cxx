/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsCommandInterpreterTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
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
#include "igstkFlockOfBirdsCommandInterpreter.h"
#include "igstkTransform.h"

class FlockOfBirdsCommandInterpreterTestCommand : public itk::Command 
{
public:
  typedef  FlockOfBirdsCommandInterpreterTestCommand  Self;
  typedef  itk::Command                    Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  itkNewMacro( Self );
protected:
  FlockOfBirdsCommandInterpreterTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
    if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};

#define IGSTK_SIMULATOR_TEST 1

// helper function to print out the hemisphere
void printHemisphere(int hemisphere)
  {
  switch(hemisphere)
    {
    case igstk::FB_FORWARD:
      std::cout << "FB_FORWARD";
      break;
    case igstk::FB_AFT:
      std::cout << "FB_AFT";
      break;
    case igstk::FB_UPPER:
      std::cout << "FB_UPPER";
      break;
    case igstk::FB_LOWER:
      std::cout << "FB_LOWER";
      break;
    case igstk::FB_LEFT:
      std::cout << "FB_LEFT";
      break;
    case igstk::FB_RIGHT:
      std::cout << "FB_RIGHT";
      break;
    default:
      std::cout << "unrecognized hemisphere";
      break;
    }
  std::cout << std::endl;
}


// set a flag if error occurred
void checkError(igstk::FlockOfBirdsCommandInterpreter *interp, int &hasError)
{
  if (interp->GetError() != igstk::FB_NO_ERROR)
    {
    hasError = 1;
    std::cout << "**ERROR** " << interp->GetErrorMessage() << std::endl;
    }
}

int igstkFlockOfBirdsCommandInterpreterTest( int argc, char * argv[] )
{
  int errorCheck = 0;

  igstk::RealTimeClock::Initialize();

  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;

#ifdef IGSTK_SIMULATOR_TEST
    igstk::SerialCommunicationSimulator::Pointer serialComm 
                                 = igstk::SerialCommunicationSimulator::New();
#else  /* IGSTK_SIMULATOR_TEST */
#ifdef WIN32
  igstk::SerialCommunicationForWindows::Pointer serialComm 
                                 = igstk::SerialCommunicationForWindows::New();
#else
  igstk::SerialCommunicationForPosix::Pointer serialComm 
                                  = igstk::SerialCommunicationForPosix::New();
#endif /* WIN32 */
#endif /* IGSTK_SIMULATOR_TEST */

  FlockOfBirdsCommandInterpreterTestCommand::Pointer my_command 
                                      = FlockOfBirdsCommandInterpreterTestCommand::New();

  // logger object created 
  std::string testName;
  if (argc > 0)
    {
    testName = argv[0];
    }
  std::string outputDirectory = IGSTK_TEST_OUTPUT_DIR;
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
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  serialComm->AddObserver( itk::AnyEvent(), my_command);

  serialComm->SetLogger( logger );

  serialComm->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate19200 );

#ifdef IGSTK_SIMULATOR_TEST
  // load a previously captured file
  std::string igstkDataDirectory = IGSTKSandbox_DATA_ROOT;
  std::string inputDirectory = igstkDataDirectory + "/Input";
  std::string simulationFile = (inputDirectory + "/" +
                                "flockofbirds_stream_03_21_2006.txt");
  serialComm->SetFileName( simulationFile.c_str() );
#else /* IGSTK_SIMULATOR_TEST */
  serialComm->SetCaptureFileName( 
                              "RecordedStreamByFlockOfBirdsCommandInterpreterTest.txt" );
  serialComm->SetCapture( true );
#endif /* IGSTK_SIMULATOR_TEST */

  serialComm->OpenCommunication();

  igstk::FlockOfBirdsCommandInterpreter::Pointer  interp;

  interp = igstk::FlockOfBirdsCommandInterpreter::New();

  interp->AddObserver( itk::AnyEvent(), my_command);

  interp->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  interp->SetCommunication( serialComm );
  checkError(interp,errorCheck);

  std::cout << "Open()" << std::endl;
  interp->Open();
  checkError(interp,errorCheck);

  std::cout << "Reset()" << std::endl;
  interp->Reset();
  checkError(interp,errorCheck);

  std::cout << "SetHemisphere(FB_UPPER)" << std::endl;
  interp->SetHemisphere(igstk::FB_UPPER);
  checkError(interp,errorCheck);

  std::cout << "ExamineValue(FB_P_HEMISPHERE) == ";
  printHemisphere(interp->ExamineValue(igstk::FB_P_HEMISPHERE));
  checkError(interp,errorCheck);
  
  // again, this time use SendCommand to do the work
  //short hemisphere = static_cast<short>(igstk::FB_AFT);
  //std::cout << "SendCommandWords(FB_HEMISPHERE, FB_AFT)" << std::endl;
  //interp->SendCommandWords(FB_HEMISPHERE, &hemisphere);

  //std::cout << "ExamineValue(igstk::FB_P_HEMISPHERE) == ";
  //printHemisphere(interp->ExamineValue(igstk::FB_P_HEMISPHERE));

  // again, this time use ChangeValue
  std::cout << "ChangeValue(FB_P_HEMISPHERE, FB_FORWARD)" << std::endl;
  interp->ChangeValue(igstk::FB_P_HEMISPHERE, igstk::FB_FORWARD);
  checkError(interp,errorCheck);

  std::cout << "ExamineValue(FB_P_HEMISPHERE) == ";
  printHemisphere(interp->ExamineValue(igstk::FB_P_HEMISPHERE));
  checkError(interp,errorCheck);

  std::cout << "SetButtonMode(1)" << std::endl;
  interp->SetButtonMode(1);
  checkError(interp,errorCheck);

  std::cout << "SetFormat(FB_POSITION_ANGLES)" << std::endl;
  interp->SetFormat(igstk::FB_POSITION_ANGLES);
  checkError(interp,errorCheck);

  std::cout << "Point()" << std::endl;
  interp->Point();
  checkError(interp,errorCheck);
  
  std::cout << "Update()" << std::endl;
  interp->Update();
  checkError(interp,errorCheck);
  
  std::cout << "GetBird() = ";
  std::cout << interp->GetBird() << std::endl;

  float position[3];
  std::cout << "GetPosition() = ";
  interp->GetPosition(position);
  checkError(interp,errorCheck);
  std::cout << "( " << position[0] << ", " 
            << position[1] << ", "
            << position[2] << " )" << std::endl; 

  float angles[3];
  std::cout << "GetAngles() = ";
  interp->GetAngles(angles);
  checkError(interp,errorCheck);
  std::cout << "( " << angles[0] << ", " 
            << angles[1] << ", "
            << angles[2] << " )" << std::endl;

  float matrix[9];
  std::cout << "MatrixFromAngles() = ";
  interp->MatrixFromAngles(matrix, angles);
  checkError(interp,errorCheck);
  std::cout << "(" 
            << "( " << matrix[0] << ", " << matrix[1] << ", " << matrix[2]
            << "), "
            << "( " << matrix[3] << ", " << matrix[4] << ", " << matrix[5]
            << "), "
            << "( " << matrix[6] << ", " << matrix[7] << ", " << matrix[8]
            << ") " << std::endl;
  
  float newangles[3];
  std::cout << "AnglesFromMatrix() = ";
  interp->AnglesFromMatrix(newangles, matrix);
  checkError(interp,errorCheck);
  std::cout << "( " << newangles[0] << ", " 
            << newangles[1] << ", "
            << newangles[2] << " )" << std::endl;

  std::cout << "Close()" << std::endl;
  interp->Close();
  checkError(interp,errorCheck);

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  if (errorCheck)
    {
    std::cout << "[FAILED]" << std::endl;

    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
