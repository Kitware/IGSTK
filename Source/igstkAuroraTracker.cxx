
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForLinux.h"
#endif

#include "igstkAuroraTracker.h"



namespace igstk
{

AuroraTracker::AuroraTracker(void) : Tracker()
{
}

AuroraTracker::~AuroraTracker(void)
{
}

void AuroraTracker::AttemptToSetUpCommunicationProcessing( void )
{
  this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentFailureInput;

#ifdef WIN32
  m_pCommunication = igstk::SerialCommunicationForWindows::New();
#else
  m_pCommunication = igstk::SerialCommunicationForLinux::New();
#endif

  if (m_pCommunication==NULL)
  {
    igstkLogMacro( igstk::Logger::DEBUG, "SerialCommunication::ReceiveStringFailureEvent ******\n");
    return;
  }
/*
  m_pCommunication->AddObserver( igstk::SerialCommunication::OpenPortFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SetupCommunicationParametersFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SetDataBufferSizeFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::CommunicationTimeoutSetupFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::RestCommunicationFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::FlushOutputBufferFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::OverlappedEventCreationFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SendStringSuccessfulEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SendStringFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SendStringWriteTimeoutEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::SendStringWaitTimeoutEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::CommunicationStatusReportFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::ReceiveStringSuccessfulEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::ReceiveStringFailureEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::ReceiveStringReadTimeoutEvent(), my_command);
  m_pCommunication->AddObserver( igstk::SerialCommunication::ReceiveStringWaitTimeoutEvent(), my_command);
*/
  if (m_pCommunication->OpenCommunication())
  {
    this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
  }
}


}
