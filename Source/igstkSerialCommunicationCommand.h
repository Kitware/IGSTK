/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationCommand.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationCommand_h
#define __igstkSerialCommunicationCommand_h

#include "itkCommand.h"

#include "igstkMacros.h"
#include "igstkSerialCommunication.h"

namespace igstk
{
/** \class SerialCommunicationCommand
 * 
 * \brief 
 * 
 *
 * \ingroup SerialCommunication
 */

class SerialCommunicationCommand : public itk::Command 
{
public:
/*
  typedef  SerialCommunicationCommand   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
*/
  SerialCommunicationCommand( SerialCommunication::Pointer object ) 
  {
    m_pSerialCommunication = object;
  }

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    /*
    if ( typeid(event)== typeid(igstk::SerialCommunication::OpenPortFailureEvent))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::OpenPortFailureEvent Error Occurred ...\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SetupCommunicationParametersFailureEvent Error Occurred ...\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SetDataBufferSizeFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SetDataBufferSizeFailureEvent Error Occurred ...\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::CommunicationTimeoutSetupFailureEvent Error Occurred ...\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringSuccessfulEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SendStringSuccessfulEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SendStringFailureEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringWriteTimeoutEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SendStringWriteTimeoutEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::SendStringWaitTimeoutEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::SendStringWaitTimeoutEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::CommunicationStatusReportFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::CommunicationStatusReportFailureEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringSuccessfulEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::ReceiveStringSuccessfulEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringFailureEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::ReceiveStringFailureEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringReadTimeoutEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::ReceiveStringReadTimeoutEvent ******\n");
    }
    else if ( typeid(event)== typeid( igstk::SerialCommunication::ReceiveStringWaitTimeoutEvent ))
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::ReceiveStringWaitTimeoutEvent ******\n");
    }
   else 
    {
        igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::Some other Error Occurred ...\n");
    }
    */
 }

protected:

  SerialCommunication::Pointer        m_pSerialCommunication;
};

}

#endif  //__igstkSerialCommunicationCommand_h

