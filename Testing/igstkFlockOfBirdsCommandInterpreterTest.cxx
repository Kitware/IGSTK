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
#include <cctype>

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

namespace igstk 
{

class FlockOfBirdsCommandInterpreterTestCommand : public itk::Command 
{
public:
  typedef  FlockOfBirdsCommandInterpreterTestCommand  Self;
  typedef  itk::Command                               Superclass;
  typedef  itk::SmartPointer<Self>                    Pointer;

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

} /* end namespace igstk */

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

  unsigned int birdMajorRevision = 0;
  unsigned int birdMinorRevision = 0;

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

  igstk::FlockOfBirdsCommandInterpreterTestCommand::Pointer my_command 
    = igstk::FlockOfBirdsCommandInterpreterTestCommand::New();

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
                                "flockofbirds_stream_05_31_2006.txt");
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

  // check the FBB status to find out how many birds can be addressed
  char status[128];
  unsigned int maxBirds = 0;
  std::cout << "ExamineValueBytes(FB_FBB_STATUS, status)" << std::endl;
  maxBirds = interp->ExamineValueBytes(igstk::FB_FBB_STATUS, status);
  checkError(interp,errorCheck);
  if (maxBirds > 127)
    {
    maxBirds = 0;
    }

  // examine the FBB status to find out how many birds there actually are
  unsigned int numBirds = 0;
  for (unsigned int bird = 0; bird < maxBirds; bird++)
    {
    if (status[bird] == 0)
      {
      numBirds = bird;
      break;
      }
    }
  
  // set if group mode
  int groupMode = 0;

  // if there are birds on the FBB, then configure them
  if (numBirds > 0)
    {
    groupMode = 1;

    std::cout << "FBBReset()" << std::endl;
    interp->FBBReset();
    checkError(interp,errorCheck);
    std::cout << "FBBAutoConfig(" << numBirds << ")" << std::endl;
    interp->FBBAutoConfig(numBirds);
    checkError(interp,errorCheck);
    std::cout << "ChangeValue(FB_GROUP_MODE, 1)" << std::endl;
    interp->ChangeValue(igstk::FB_GROUP_MODE, 1);
    checkError(interp,errorCheck);

    for (unsigned int bird = 0; bird < numBirds; bird++)
      {
      interp->RS232ToFBB(bird+1);
      checkError(interp,errorCheck);
      int rev = interp->ExamineValue(igstk::FB_REVISION);
      checkError(interp,errorCheck);
      // re-order the revision bytes
      rev = ((rev >> 8) & 0x00ff) | ((rev << 8) & 0xff00);
      birdMajorRevision = ((rev & 0xff00) >> 8);
      birdMinorRevision = (rev & 0x00ff);

      std::cout << "bird " << (bird+1) << " revision = "
                << birdMajorRevision << "." << birdMinorRevision
                << std::endl;
      
      char modelID[16];
      memset(modelID, 0, 16);
      interp->RS232ToFBB(bird+1);
      checkError(interp,errorCheck);
      interp->ExamineValueBytes(igstk::FB_IDENTIFICATION, modelID);
      modelID[10] = '\0';
      checkError(interp,errorCheck);
      for (unsigned int i = 0; i < 10; i++)
        {
        if (!isprint(modelID[i]))
          {
          modelID[i] = '#';
          }
        }
      std::cout << "bird " << (bird+1) << " identification = "
                << modelID << std::endl;

      // FB_SERIAL_NUMER command only works on newer birds
      if (birdMajorRevision > 3 || 
          (birdMajorRevision == 3 && birdMinorRevision >= 66))
        {  
        interp->RS232ToFBB(bird+1);
        checkError(interp,errorCheck);
        int serialNo = interp->ExamineValue(igstk::FB_SERIAL_NUMBER);
        checkError(interp,errorCheck);
        std::cout << "bird " << (bird+1) << " serial number = "
                  << serialNo << std::endl;
        }
    
      std::cout << "RS232ToFBB(" << (bird+1) << ")" << std::endl;
      interp->RS232ToFBB(bird+1);
      checkError(interp,errorCheck);
      std::cout << "SetHemisphere(FB_FORWARD)" << std::endl;
      interp->SetHemisphere(igstk::FB_FORWARD);
      checkError(interp,errorCheck);
      }
    }
  else
    {
    // single-bird mode, the FBB is not used
    groupMode = 0;
    numBirds = 1;

    int rev = interp->ExamineValue(igstk::FB_REVISION);
    checkError(interp,errorCheck);
    // re-order the revision bytes
    rev = ((rev >> 8) & 0x00ff) | ((rev << 8) & 0xff00);
    birdMajorRevision = ((rev & 0xff00) >> 8);
    birdMinorRevision = (rev & 0x00ff);

    std::cout << "revision = "
              << birdMajorRevision << "." << birdMinorRevision
              << std::endl;
      
    char modelID[16];
    memset(modelID, 0, 16);
    interp->ExamineValueBytes(igstk::FB_IDENTIFICATION, modelID);
    checkError(interp,errorCheck);
    modelID[11] = '\0';
    std::cout << "identification = " << modelID << std::endl;

    // FB_SERIAL_NUMER command only works on newer birds
    if (birdMajorRevision > 3 || 
        (birdMajorRevision == 3 && birdMinorRevision >= 66))
      {  
      int serialNo = interp->ExamineValue(igstk::FB_SERIAL_NUMBER);
      checkError(interp,errorCheck);
      std::cout << "serial number = " << serialNo << std::endl;
      }
    
    std::cout << "SetHemisphere(FB_FORWARD)" << std::endl;
    interp->SetHemisphere(igstk::FB_FORWARD);
    checkError(interp,errorCheck);
    }

  // the FB_P_HEMISPHERE is not available on old bird units
  if (birdMajorRevision > 3 || 
      (birdMajorRevision == 3 && birdMinorRevision >= 66))
    {  
    std::cout << "ExamineValue(FB_P_HEMISPHERE) == ";
    printHemisphere(interp->ExamineValue(igstk::FB_P_HEMISPHERE));
    checkError(interp,errorCheck);
  
    // again, this time use ChangeValueWords to do the work
    short hemisphere = static_cast<short>(igstk::FB_AFT);
    std::cout << "hemisphere == ";
    printHemisphere(hemisphere);
    std::cout << "ChangeValueWords(FB_HEMISPHERE, &hemisphere)" << std::endl;
    interp->ChangeValueWords(igstk::FB_P_HEMISPHERE, &hemisphere);

    // again, this time use ExamineValueWords to do the work
    std::cout << "ExamineValueWords(FB_HEMISPHERE, &hemisphere) == "
              << std::endl;
    interp->ExamineValueWords(igstk::FB_P_HEMISPHERE, &hemisphere);
    printHemisphere(hemisphere);

    // again, this time use ChangeValue (the way it is usually done)
    std::cout << "ChangeValue(FB_P_HEMISPHERE, FB_FORWARD)" << std::endl;
    interp->ChangeValue(igstk::FB_P_HEMISPHERE, igstk::FB_FORWARD);
    checkError(interp,errorCheck);

    std::cout << "ExamineValue(FB_P_HEMISPHERE) == ";
    printHemisphere(interp->ExamineValue(igstk::FB_P_HEMISPHERE));
    checkError(interp,errorCheck);
    }

  // comment out for now: the simulation file doesn't do it yet
  //int buttonState = 0;
  //std::cout << "ButtonRead(&buttonState)" << std::endl;
  //interp->ButtonRead(&buttonState);
  //checkError(interp,errorCheck);
  //std::cout << "buttonState = " << buttonState << std::endl;  

  std::cout << "SetFormat(FB_POSITION_MATRIX)" << std::endl;
  interp->SetFormat(igstk::FB_POSITION_MATRIX);
  checkError(interp,errorCheck);

  std::cout << "SetButtonMode(1)" << std::endl;
  interp->SetButtonMode(1);
  checkError(interp,errorCheck);

  std::cout << "Point()" << std::endl;
  interp->Point();
  checkError(interp,errorCheck);
  
  for (unsigned int i = 0; i < numBirds; i++)
    {
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
    
    float matrix[9];
    std::cout << "GetMatrix() = ";
    interp->GetMatrix(matrix);
    checkError(interp,errorCheck);
    std::cout << "(" 
              << "( " << matrix[0] << ", " << matrix[1] << ", " << matrix[2]
              << "), "
              << "( " << matrix[3] << ", " << matrix[4] << ", " << matrix[5]
              << "), "
              << "( " << matrix[6] << ", " << matrix[7] << ", " << matrix[8]
              << ") )" << std::endl;
  
    float angles[3];
    std::cout << "AnglesFromMatrix() = ";
    interp->AnglesFromMatrix(angles, matrix);
    checkError(interp,errorCheck);
    std::cout << "( " << angles[0] << ", " 
              << angles[1] << ", "
              << angles[2] << " )" << std::endl;

    std::cout << "MatrixFromAngles() = ";
    interp->MatrixFromAngles(matrix, angles);
    checkError(interp,errorCheck);
    std::cout << "(" 
              << "( " << matrix[0] << ", " << matrix[1] << ", " << matrix[2]
              << "), "
              << "( " << matrix[3] << ", " << matrix[4] << ", " << matrix[5]
              << "), "
              << "( " << matrix[6] << ", " << matrix[7] << ", " << matrix[8]
              << ") )" << std::endl;

    std::cout << "GetButton() = ";
    int buttonDown = interp->GetButton();
    std::cout << buttonDown << std::endl;
    }

  std::cout << "SetFormat(FB_POSITION_ANGLES)" << std::endl;
  interp->SetFormat(igstk::FB_POSITION_ANGLES);
  checkError(interp,errorCheck);

  std::cout << "Stream()" << std::endl;
  interp->Stream();
  checkError(interp,errorCheck);

  for (unsigned int j = 0; j < 10; j++)
    {
    for (unsigned int i = 0; i < numBirds; i++)
      {
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
      }
    }

  std::cout << "EndStream()" << std::endl;
  interp->EndStream();
  checkError(interp,errorCheck);

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
