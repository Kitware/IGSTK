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

#include "igstkSerialCommunication.h"


namespace igstk
{ 
/** Constructor */
SerialCommunication::SerialCommunication() :  m_StateMachine( this ),
                                              m_ReadBufferSize(1600),
                                              m_WriteBufferSize(1600)
{
  m_PortNumber = PortNumber0;
  m_BaudRate = BaudRate9600;
  m_DataBits = DataBits8;
  m_Parity = NoParity;
  m_StopBits = StopBits1;
  m_HardwareHandshake = HandshakeOff;

  m_InputBuffer = 0;
  m_OutputBuffer = 0;
  m_ReadDataSize = 0;
  m_TimeoutPeriod = 500;

  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState, "IdleState" );
  m_StateMachine.AddState( m_AttemptToOpenPortState, "AttemptToOpenPortState" );
  m_StateMachine.AddState( m_PortOpenState, "PortOpenState" );

  m_StateMachine.AddState( m_AttemptToSetUpDataBuffersState, "AttemptToSetUpDataBuffersState" );
  m_StateMachine.AddState( m_DataBuffersSetState, "DataBuffersSetState" );

  m_StateMachine.AddState( m_AttemptToSetTransferParametersState, "AttemptToSetTransferParametersState" );
  m_StateMachine.AddState( m_PortReadyForCommunicationState, "PortReadyForCommunication" );

  m_StateMachine.AddState( m_AttemptToClosePortState, "m_AttemptToClosePortState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_OpenPortInput, "OpenPort");
  m_StateMachine.AddInput( m_OpenPortSuccessInput, "OpenPortSuccessInput");
  m_StateMachine.AddInput( m_OpenPortFailureInput, "OpenPortFailureInput");

  m_StateMachine.AddInput( m_SetUpDataBuffersInput, "SetUpDataBuffersInput");
  m_StateMachine.AddInput( m_DataBuffersSetUpSuccessInput, "DataBuffersSetUpSuccessInput");
  m_StateMachine.AddInput( m_DataBuffersSetUpFailureInput, "DataBuffersSetUpFailureInput");
 
  m_StateMachine.AddInput( m_SetTransferParametersInput, "SetTransferParametersInput");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpSuccessInput, "DataTransferParametersSetUpSuccessInput");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpFailureInput, "DataTransferParametersSetUpFailureInput");

  m_StateMachine.AddInput( m_ClosePortInput, "ClosePortInput");
  m_StateMachine.AddInput( m_ClosePortSuccessInput, "ClosePortSuccessInput");
  m_StateMachine.AddInput( m_ClosePortFailureInput, "ClosePortFailureInput");

  m_StateMachine.AddInput( m_SendBreakInput, "SendBreakInput");

  m_StateMachine.AddInput( m_FlushOutputBufferInput, "FlushOutputBufferInput");
  m_StateMachine.AddInput( m_WriteInput, "WriteInput");
  m_StateMachine.AddInput( m_ReadInput, "ReadInput");

  const ActionType NoAction = 0;

  // Programming the state machine transitions
  m_StateMachine.AddTransition( m_IdleState, m_OpenPortInput, m_AttemptToOpenPortState, &SerialCommunication::AttemptToOpenCommunication);
  m_StateMachine.AddTransition( m_AttemptToOpenPortState, m_OpenPortSuccessInput, m_PortOpenState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToOpenPortState, m_OpenPortFailureInput, m_IdleState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_SetUpDataBuffersInput, m_AttemptToSetUpDataBuffersState, &SerialCommunication::AttemptToSetUpDataBuffers);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataBuffersState, m_DataBuffersSetUpSuccessInput, m_DataBuffersSetState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataBuffersState, m_DataBuffersSetUpFailureInput, m_PortOpenState, NoAction);

  m_StateMachine.AddTransition( m_DataBuffersSetState, m_SetTransferParametersInput, m_AttemptToSetTransferParametersState, &SerialCommunication::AttemptToSetTransferParameters);
  m_StateMachine.AddTransition( m_AttemptToSetTransferParametersState, m_DataTransferParametersSetUpSuccessInput, m_PortReadyForCommunicationState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToSetTransferParametersState, m_DataTransferParametersSetUpFailureInput, m_DataBuffersSetState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::AttemptToClosePort);
  m_StateMachine.AddTransition( m_DataBuffersSetState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::AttemptToClearBuffersAndClosePort);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::AttemptToClearBuffersAndClosePort);
  m_StateMachine.AddTransition( m_IdleState, m_ClosePortInput, m_IdleState, NoAction);

  m_StateMachine.AddTransition( m_AttemptToClosePortState, m_ClosePortSuccessInput, m_IdleState, &SerialCommunication::ClosePortSuccessProcessing);
  m_StateMachine.AddTransition( m_IdleState, m_ClosePortSuccessInput, m_IdleState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToClosePortState, m_ClosePortFailureInput, m_PortOpenState, &SerialCommunication::ClosePortFailureProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_SendBreakInput, m_PortReadyForCommunicationState, &SerialCommunication::InternalSendBreak);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_FlushOutputBufferInput, m_PortReadyForCommunicationState, &SerialCommunication::InternalFlushOutputBuffer);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_WriteInput, m_PortReadyForCommunicationState, &SerialCommunication::InternalWrite);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ReadInput, m_PortReadyForCommunicationState, &SerialCommunication::InternalRead);

  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 


/** Destructor */
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}


void SerialCommunication::OpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenCommunication called ...\n");
  // Attempt to open communication port
  this->m_StateMachine.PushInput( m_OpenPortInput );
  this->m_StateMachine.ProcessInputs();
}


void SerialCommunication::CloseCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::CloseCommunication called ...\n");
  this->m_StateMachine.PushInput( m_ClosePortInput );
  this->m_StateMachine.ProcessInputs();
}


void SerialCommunication::SendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SendBreak called ...\n");
  this->m_StateMachine.PushInput( m_SendBreakInput );
  this->m_StateMachine.ProcessInputs();

  return;
}


void SerialCommunication::Flush( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Flush called ...\n");
  this->m_StateMachine.PushInput( m_FlushOutputBufferInput );
  this->m_StateMachine.ProcessInputs();

  return;
}


void SerialCommunication::Write( const char *data, int numberOfBytes )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Write called ...\n");
  if( m_OutputBuffer != NULL )
    {
    m_WriteNumberOfBytes = ( m_WriteBufferSize < numberOfBytes ) ? m_WriteBufferSize : numberOfBytes;
    memcpy(m_OutputBuffer, data, m_WriteNumberOfBytes );
    m_OutputBuffer[m_WriteNumberOfBytes] = 0;
    igstkLogMacro( DEBUG, "Message = " << m_OutputBuffer << std::endl );
    }
  else
    {
    m_WriteNumberOfBytes = 0;
    }
  igstkLogMacro( DEBUG, "Message length = " << m_WriteNumberOfBytes << std::endl );
  this->m_StateMachine.PushInput( m_WriteInput );
  this->m_StateMachine.ProcessInputs();

  return;
}


void SerialCommunication::Read( char *data, int numberOfBytes, int &bytesRead )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Read called ...\n");
  m_ReadNumberOfBytes = ( m_ReadBufferSize < numberOfBytes ) ? m_ReadBufferSize : numberOfBytes;
  this->m_StateMachine.PushInput( m_ReadInput );
  this->m_StateMachine.ProcessInputs();
  if( m_InputBuffer != NULL )
    {
    memcpy(data, m_InputBuffer, m_ReadDataSize);
    bytesRead = m_ReadDataSize;
    }
  else
    {
    bytesRead = 0;
    }
  data[bytesRead] = '\0'; // terminate the string
  igstkLogMacro( DEBUG, "SerialCommunication::Read : (" << bytesRead << ") " << data << "...\n");

  return;
}


void SerialCommunication::ClosePortSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::ClosePortSuccessProcessing called ...\n");
}


void SerialCommunication::ClosePortFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::ClosePortFailureProcessing called ...\n");
}


void SerialCommunication::AttemptToOpenCommunication()
{
  ResultType result;
  result = InternalOpenCommunication();

  m_StateMachine.PushInputBoolean( (bool)result, m_OpenPortSuccessInput, m_OpenPortFailureInput );
  if( result )
  {
    this->m_StateMachine.PushInput( m_SetUpDataBuffersInput );
  }
}


void SerialCommunication::AttemptToSetUpDataBuffers()
{
  ResultType result;
  result = InternalSetUpDataBuffers();
  m_StateMachine.PushInputBoolean( (bool)result, m_DataBuffersSetUpSuccessInput, m_DataBuffersSetUpFailureInput );
  if( result )
  {
    this->m_StateMachine.PushInput( m_SetTransferParametersInput );
  }
}


void SerialCommunication::AttemptToSetTransferParameters()
{
  ResultType result;
  result = InternalSetTransferParameters();
  m_StateMachine.PushInputBoolean( (bool)result, m_DataTransferParametersSetUpSuccessInput, m_DataTransferParametersSetUpFailureInput );
}


void SerialCommunication::AttemptToClosePort()
{
  ResultType result;
  result = InternalClosePort();
  m_StateMachine.PushInputBoolean( (bool)result, m_ClosePortSuccessInput, m_ClosePortFailureInput );
}


void SerialCommunication::AttemptToClearBuffersAndClosePort()
{
  ResultType result;
  result = InternalClearBuffersAndClosePort();
  m_StateMachine.PushInputBoolean( (bool)result, m_ClosePortSuccessInput, m_ClosePortFailureInput );
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
  os << indent << "ReadBufferOffset: " << m_ReadBufferOffset << std::endl;
  os << indent << "ReadDataSize: " << m_ReadDataSize << std::endl;
}


} // end namespace igstk

