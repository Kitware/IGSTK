/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTest.cxx
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
#include "igstkSerialCommunication.h"


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
    if ( typeid(event)== typeid( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent ))
    {
        std::cout << "SetupCommunicationParametersFailureEvent Error Occurred ...\n";
    }
    if ( typeid(event)== typeid( igstk::SerialCommunication::SetDataBufferSizeFailureEvent ))
    {
        std::cout << "SetDataBufferSizeFailureEvent Error Occurred ...\n";
    }
    if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent ))
    {
        std::cout << "CommunicationTimeoutSetupFailureEvent Error Occurred ...\n";
    }
  }
};


int igstkSerialCommunicationTest( int, char * [] )
{
  typedef igstk::Logger                   LoggerType; 

  igstk::SerialCommunication::Pointer serialComm = igstk::SerialCommunication::New();

  SerialCommunicationTestCommand::Pointer my_command = SerialCommunicationTestCommand::New();

  // logger object created for logging mouse activities
  LoggerType            logger;
  logger.AddOutputStream( std::cout );
  logger.SetPriorityLevel( igstk::Logger::DEBUG );

  serialComm->AddObserver( igstk::SerialCommunication::OpenPortFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::SetDataBufferSizeFailureEvent(), my_command);
  serialComm->AddObserver( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent(), my_command);

  serialComm->SetLogger( &logger );

  serialComm->OpenCommunication();

  serialComm->CloseCommunication();

  return EXIT_SUCCESS;
}


