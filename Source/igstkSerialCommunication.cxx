/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunication.cxx
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
#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <time.h>

#include "igstkSerialCommunication.h"


namespace igstk
{ 

/** Constructor */
SerialCommunication::SerialCommunication() :  m_StateMachine( this )
{
  m_PortNumber = PortNumber0;
  m_BaudRate = BaudRate9600;
  m_DataBits = DataBits8;
  m_Parity = NoParity;
  m_StopBits = StopBits1;
  m_HardwareHandshake = HandshakeOff;

  m_InputData = 0;
  m_OutputData = 0;
  m_BytesToWrite = 0;
  m_BytesToRead = 0;
  m_BytesRead = 0;
  m_TimeoutPeriod = 500;

  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState,
                           "IdleState" );

  m_StateMachine.AddState( m_AttemptingToOpenPortState,
                           "AttemptingToOpenPortState" );

  m_StateMachine.AddState( m_PortOpenState,
                           "PortOpenState" );

  m_StateMachine.AddState( m_AttemptingToSetTransferParametersState,
                           "AttemptingToSetTransferParametersState" );

  m_StateMachine.AddState( m_ReadyForCommunicationState,
                           "ReadyForCommunication" );

  m_StateMachine.AddState( m_AttemptingToClosePortState,
                           "m_AttemptingToClosePortState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_OpenPortInput,
                           "OpenPort");

  m_StateMachine.AddInput( m_OpenPortSuccessInput,
                           "OpenPortSuccessInput");

  m_StateMachine.AddInput( m_OpenPortFailureInput,
                           "OpenPortFailureInput");

  m_StateMachine.AddInput( m_SetTransferParametersInput,
                           "SetTransferParametersInput");

  m_StateMachine.AddInput( m_SetTransferParametersSuccessInput,
                           "SetTransferParametersSuccessInput");

  m_StateMachine.AddInput( m_SetTransferParametersFailureInput,
                           "SetTransferParametersFailureInput");

  m_StateMachine.AddInput( m_ClosePortInput,
                           "ClosePortInput");

  m_StateMachine.AddInput( m_ClosePortSuccessInput,
                           "ClosePortSuccessInput");

  m_StateMachine.AddInput( m_ClosePortFailureInput,
                           "ClosePortFailureInput");

  m_StateMachine.AddInput( m_SendBreakInput,
                           "SendBreakInput");

  m_StateMachine.AddInput( m_PurgeBuffersInput,
                           "PurgeBuffersInput");
  
  m_StateMachine.AddInput( m_SleepInput,
                           "SleepInput");

  m_StateMachine.AddInput( m_WriteInput,
                           "WriteInput");

  m_StateMachine.AddInput( m_ReadInput,
                           "ReadInput");

  // ----------------------------------------
  // Programming the state machine transitions

  const ActionType NoAction = 0;

  // IdleState
  m_StateMachine.AddTransition( m_IdleState,
                                m_OpenPortInput,
                                m_AttemptingToOpenPortState,
                                &SerialCommunication::AttemptToOpenPort);

  m_StateMachine.AddTransition( m_IdleState,
                                m_ClosePortInput,
                                m_IdleState,
                                NoAction);

  m_StateMachine.AddTransition( m_IdleState,
                                m_SetTransferParametersInput,
                                m_IdleState,
                                NoAction);

  // AttemptingToOpenPortState
  m_StateMachine.AddTransition( m_AttemptingToOpenPortState,
                                m_OpenPortSuccessInput,
                                m_PortOpenState,
                                &SerialCommunication::OpenPortSuccessProcessing);

  m_StateMachine.AddTransition( m_AttemptingToOpenPortState,
                                m_OpenPortFailureInput,
                                m_IdleState,
                                NoAction);

  // PortOpenState
  m_StateMachine.AddTransition( m_PortOpenState,
                                m_SetTransferParametersInput,
                                m_AttemptingToSetTransferParametersState,
                                &SerialCommunication::AttemptToSetTransferParameters);

  m_StateMachine.AddTransition( m_PortOpenState,
                                m_PurgeBuffersInput,
                                m_PortOpenState,
                                &SerialCommunication::InternalPurgeBuffers);

  m_StateMachine.AddTransition( m_PortOpenState,
                                m_ClosePortInput,
                                m_AttemptingToClosePortState,
                                &SerialCommunication::AttemptToClosePort);

  // AttemptingToSetTransferParametersState
  m_StateMachine.AddTransition( m_AttemptingToSetTransferParametersState,
                                m_SetTransferParametersSuccessInput,
                                m_ReadyForCommunicationState,
                                NoAction);

  m_StateMachine.AddTransition( m_AttemptingToSetTransferParametersState,
                                m_SetTransferParametersFailureInput,
                                m_PortOpenState,
                                NoAction);

  // ReadyForCommunicationState
  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_SendBreakInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::InternalSendBreak);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_PurgeBuffersInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::InternalPurgeBuffers);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_SleepInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::InternalSleep);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_WriteInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::InternalWrite);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_ReadInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::InternalRead);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_ClosePortInput,
                                m_AttemptingToClosePortState,
                                &SerialCommunication::AttemptToClosePort);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_SetTransferParametersInput,
                                m_AttemptingToSetTransferParametersState,
                                &SerialCommunication::AttemptToSetTransferParameters);

  // AttemptingToClosePortState
  m_StateMachine.AddTransition( m_AttemptingToClosePortState,
                                m_ClosePortSuccessInput,
                                m_IdleState,
                                NoAction);

  m_StateMachine.AddTransition( m_AttemptingToClosePortState,
                                m_ClosePortFailureInput,
                                m_PortOpenState,
                                NoAction);

  // Set the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  m_Recording = false;

} 


// Destructor 
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}


void SerialCommunication::SetRecordingFileName(const char* filename)
{
  m_RecordingFilename = filename;
}


void SerialCommunication::OpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenCommunication called ...\n");

  // Open a file for writing data stream.
  if( m_Recording )
    {
    time_t ti;
    time(&ti);
    igstkLogMacro( DEBUG, "Recording is on. Filename: " << m_RecordingFilename << std::endl );
    m_FileStream.open(m_RecordingFilename.c_str(), std::ios::binary);
    if( !m_FileStream.is_open() )
      {
      igstkLogMacro( CRITICAL, "failed to open a file for writing data stream." << std::endl );
      }
    m_SendNo = 1;
    m_RecvNo = 1;
    m_FileStream << "# recorded " << asctime(localtime(&ti)) << "\r\n";
    }

  // Attempt to open communication port
  m_StateMachine.PushInput( m_OpenPortInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::CloseCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::CloseCommunication called ...\n");
  m_StateMachine.PushInput( m_ClosePortInput );
  m_StateMachine.ProcessInputs();
  m_FileStream.close();
}


void SerialCommunication::SetBaudRate( BaudRateType baudRate )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetBaudRate(" << baudRate << ") called ...\n");

  m_BaudRate = baudRate;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetDataBits( DataBitsType bits)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetDataBits(" << bits << ") called ...\n");

  m_DataBits = bits;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetParity( ParityType parity)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetParity(" << parity << ") called ...\n");

  m_Parity = parity;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetStopBits( StopBitsType bits)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetStopBits(" << bits << ") called ...\n");

  m_StopBits = bits;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetHardwareHandshake( HandshakeType handshake)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetHardwareHandshake(" << handshake << ") called ...\n");

  m_HardwareHandshake = handshake;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetTimeoutPeriod( int milliseconds )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetTimeoutPeriod(" << milliseconds << ") called ...\n");

  m_TimeoutPeriod = milliseconds;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SendBreak called ...\n");

  m_StateMachine.PushInput( m_SendBreakInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::PurgeBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::PurgeBuffers called ...\n");

  m_StateMachine.PushInput( m_PurgeBuffersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::Sleep( int milliseconds )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Sleep(" << milliseconds << ") called ...\n");

  m_SleepPeriod = milliseconds;
  m_StateMachine.PushInput( m_SleepInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::Write( const char *data, int numberOfBytes )
{
  // this call to the log macro assumes data is null terminated
  igstkLogMacro( DEBUG, "SerialCommunication::Write : (" << numberOfBytes << ") " << data << "...\n");

  m_OutputData = data;
  m_BytesToWrite = numberOfBytes;

  // Recording for data sent
  if( m_Recording )
    {
    m_SendNo = m_RecvNo;
    m_FileStream << m_SendNo << ". command[" << numberOfBytes << "] ";
    int i;
    for( i = 0; i < numberOfBytes; ++i )
      {
      m_FileStream << m_OutputData[i];
      }
    m_FileStream << '\n';
    }

  m_StateMachine.PushInput( m_WriteInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::Read( char *data, int numberOfBytes, int &bytesRead )
{
  m_BytesRead = 0;
  m_BytesToRead = numberOfBytes;
  m_InputData = data; 

  m_StateMachine.PushInput( m_ReadInput );
  m_StateMachine.ProcessInputs();

  bytesRead = m_BytesRead;

  // terminate the string
  data[bytesRead] = '\0';

  /** Recording for data received */
  if( m_Recording )
    {
    m_FileStream << m_RecvNo++ << ". receive[" << bytesRead << "] ";
    for(int i = 0; i < bytesRead; ++i )
      {
      m_FileStream << m_InputData[i];
      }
    m_FileStream << '\n';
    }

  igstkLogMacro( DEBUG, "SerialCommunication::Read : (" << bytesRead << ") " << data << "...\n");
}


void SerialCommunication::OpenPortSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenPortSuccessProcessing called ...\n");

  // if the port was opened successfully, then set transfer parameters next
  m_StateMachine.PushInput( m_SetTransferParametersInput );
}


void SerialCommunication::AttemptToOpenPort()
{
  m_StateMachine.PushInputBoolean( (bool)InternalOpenPort(), 
                                   m_OpenPortSuccessInput,
                                   m_OpenPortFailureInput );
}


void SerialCommunication::AttemptToSetTransferParameters()
{
  m_StateMachine.PushInputBoolean( (bool)InternalSetTransferParameters(),
                                   m_SetTransferParametersSuccessInput,
                                   m_SetTransferParametersFailureInput );
}


void SerialCommunication::AttemptToClosePort()
{
  m_StateMachine.PushInputBoolean( (bool)InternalClosePort(),
                                   m_ClosePortSuccessInput,
                                   m_ClosePortFailureInput );
}


void SerialCommunication::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortNumber: " << m_PortNumber << std::endl;
  os << indent << "BaudRate: " << m_BaudRate << std::endl;
  os << indent << "DataBits: " << m_DataBits << std::endl;
  os << indent << "Parity: " << m_Parity << std::endl;
  os << indent << "StopBits: " << m_StopBits << std::endl;
  os << indent << "HardwareHandshake: " << m_HardwareHandshake 
     << std::endl;
  os << indent << "TimeoutPeriod: " << m_TimeoutPeriod << std::endl;
}


} // end namespace igstk

