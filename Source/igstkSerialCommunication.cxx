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

#include "igstkSerialCommunication.h"
#include "igstkSerialCommunicationCommand.h"


namespace igstk
{ 
/** Constructor */
SerialCommunication::SerialCommunication() :  m_StateMachine( this ),
                                              m_ReadBufferSize(3000),
                                              m_WriteBufferSize(200),
                                              m_InputBuffer( NULL),
                                              m_OutputBuffer( NULL),
                                              m_PortRestSpan(10),
                                              m_PortNumber( PortNumber0() ),
                                              m_BaudRate( BaudRate9600() ),
                                              m_ByteSize( DataBits8() ),
                                              m_Parity( NoParity() ),
                                              m_StopBits( StopBits1() ),
                                              m_HardwareHandshake( HandshakeOff() )
{

  /** Communication Time Out Settings */
  //ReadTimeout = m_ReadTotalTimeoutConstant + m_ReadTotalTimeoutMultiplier*Number_Of_Bytes_Read 
  m_ReadIntervalTimeout = 0xFFFF;
  m_ReadTotalTimeoutMultiplier = 0;
  m_ReadTotalTimeoutConstant = 0;
  //WriteTimeout = m_WriteTotalTimeoutConstant + m_WriteTotalTimeoutMultiplier*Number_Of_Bytes_Written 
  m_WriteTotalTimeoutMultiplier = 10;
  m_WriteTotalTimeoutConstant = 500;

  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState, "IdleState" );
  m_StateMachine.AddState( m_AttemptToOpenPortState, "AttemptToOpenPortState" );
  m_StateMachine.AddState( m_PortOpenState, "PortOpenState" );

  m_StateMachine.AddState( m_AttemptToSetUpDataBuffersState, "AttemptToSetUpDataBuffersState" );
  m_StateMachine.AddState( m_DataBuffersSetState, "DataBuffersSetState" );

  m_StateMachine.AddState( m_AttemptToSetUpDataTransferParametersState, "AttemptToSetUpDataTransferParametersState" );
  m_StateMachine.AddState( m_PortReadyForCommunicationState, "PortReadyForCommunication" );

  m_StateMachine.AddState( m_AttemptToClosePortState, "m_AttemptToClosePortState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_OpenPortInput, "OpenPort");
  m_StateMachine.AddInput( m_OpenPortSuccessInput, "OpenPortSuccessInput");
  m_StateMachine.AddInput( m_OpenPortFailureInput, "OpenPortFailureInput");

  m_StateMachine.AddInput( m_SetUpDataBuffersInput, "SetUpDataBuffersInput");
  m_StateMachine.AddInput( m_DataBuffersSetUpSuccessInput, "DataBuffersSetUpSuccessInput");
  m_StateMachine.AddInput( m_DataBuffersSetUpFailureInput, "DataBuffersSetUpFailureInput");
 
  m_StateMachine.AddInput( m_SetUpDataTransferParametersInput, "SetUpDataTransferParametersInput");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpSuccessInput, "DataTransferParametersSetUpSuccessInput");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpFailureInput, "DataTransferParametersSetUpFailureInput");

  m_StateMachine.AddInput( m_ClosePortInput, "ClosePortInput");
  m_StateMachine.AddInput( m_ClosePortSuccessInput, "ClosePortSuccessInput");
  m_StateMachine.AddInput( m_ClosePortFailureInput, "ClosePortFailureInput");

  m_StateMachine.AddInput( m_RestPortInput, "RestPortInput");

  m_StateMachine.AddInput( m_FlushOutputBufferInput, "FlushOutputBufferInput");
  m_StateMachine.AddInput( m_SendStringInput, "SendStringInput");
  m_StateMachine.AddInput( m_ReceiveStringInput, "ReceiveStringInput");

  const ActionType NoAction = 0;

  // Programming the state machine transitions
  m_StateMachine.AddTransition( m_IdleState, m_OpenPortInput, m_AttemptToOpenPortState, &SerialCommunication::OpenPortProcessing);
  m_StateMachine.AddTransition( m_AttemptToOpenPortState, m_OpenPortSuccessInput, m_PortOpenState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToOpenPortState, m_OpenPortFailureInput, m_IdleState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_SetUpDataBuffersInput, m_AttemptToSetUpDataBuffersState, &SerialCommunication::SetUpDataBuffersProcessing);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataBuffersState, m_DataBuffersSetUpSuccessInput, m_DataBuffersSetState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataBuffersState, m_DataBuffersSetUpFailureInput, m_PortOpenState, NoAction);

  m_StateMachine.AddTransition( m_DataBuffersSetState, m_SetUpDataTransferParametersInput, m_AttemptToSetUpDataTransferParametersState, &SerialCommunication::SetUpDataTransferParametersProcessing);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataTransferParametersState, m_DataTransferParametersSetUpSuccessInput, m_PortReadyForCommunicationState, NoAction);
  m_StateMachine.AddTransition( m_AttemptToSetUpDataTransferParametersState, m_DataTransferParametersSetUpFailureInput, m_DataBuffersSetState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::ClosePortProcessing);
  m_StateMachine.AddTransition( m_DataBuffersSetState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::ClearBuffersAndClosePortProcessing);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ClosePortInput, m_AttemptToClosePortState, &SerialCommunication::ClearBuffersAndClosePortProcessing);

  m_StateMachine.AddTransition( m_AttemptToClosePortState, m_ClosePortSuccessInput, m_IdleState, &SerialCommunication::ClosePortSuccessProcessing);
  m_StateMachine.AddTransition( m_AttemptToClosePortState, m_ClosePortFailureInput, m_PortOpenState, &SerialCommunication::ClosePortFailureProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_RestPortInput, m_PortReadyForCommunicationState, &SerialCommunication::RestPortProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_FlushOutputBufferInput, m_PortReadyForCommunicationState, &SerialCommunication::FlushOutputBufferProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_SendStringInput, m_PortReadyForCommunicationState, &SerialCommunication::SendStringProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ReceiveStringInput, m_PortReadyForCommunicationState, &SerialCommunication::ReceiveStringProcessing);

  m_StateMachine.SelectInitialState( m_IdleState );

  // Initialize result input pointers

  m_pOpenPortResultInput = &m_OpenPortSuccessInput;
  m_pDataBuffersSetUpResultInput = &m_DataBuffersSetUpSuccessInput;
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpSuccessInput;
  m_pClosePortResultInput = &m_ClosePortSuccessInput;

  // Create instance of serial command
  m_pCommand = SerialCommunicationCommand::New();// this );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}

bool SerialCommunication::OpenCommunication( void )
{
  // Attempt to open communication port
  this->m_StateMachine.ProcessInput( m_OpenPortInput );
  this->m_StateMachine.ProcessInput( *m_pOpenPortResultInput );

  // Attempt to set up data buffers
  this->m_StateMachine.ProcessInput( m_SetUpDataBuffersInput );
  this->m_StateMachine.ProcessInput( *m_pDataBuffersSetUpResultInput );
 
  // Attempt to set up communication parameters 
  this->m_StateMachine.ProcessInput( m_SetUpDataTransferParametersInput );
  this->m_StateMachine.ProcessInput( *m_pDataTransferParametersSetUpResultInput );

  // Return "true" if successful, else return "false"
  return(  (m_pOpenPortResultInput==&m_OpenPortSuccessInput)
         &&(m_pDataBuffersSetUpResultInput==&m_DataBuffersSetUpSuccessInput)
         &&(m_pDataTransferParametersSetUpResultInput==&m_DataTransferParametersSetUpSuccessInput) );
}


bool SerialCommunication::CloseCommunication( void )
{
  this->m_StateMachine.ProcessInput( m_ClosePortInput );
  this->m_StateMachine.ProcessInput( *m_pClosePortResultInput );
  return(m_pClosePortResultInput==&m_ClosePortSuccessInput);
}


bool SerialCommunication::RestCommunication( void )
{
  this->m_StateMachine.ProcessInput( m_RestPortInput );
  return true;
}


bool SerialCommunication::Flush( void )
{
  this->m_StateMachine.ProcessInput( m_FlushOutputBufferInput );
  return true;
}


bool SerialCommunication::SendString( const char *data )
{
  int strSize = (strlen(data)<m_WriteBufferSize) ? strlen(data) : m_WriteBufferSize;
  memcpy(m_OutputBuffer, data, sizeof(char)*strSize);
//  m_OutputBuffer[strSize+2] = '\r';
//  m_OutputBuffer[strSize+2] = '\0';
  igstkLogMacro( DEBUG, "Message length = " << strSize << std::endl );
  igstkLogMacro( DEBUG, "Message = " << m_OutputBuffer << std::endl );
  this->m_StateMachine.ProcessInput( m_SendStringInput );
  return true;
}

bool SerialCommunication::ReceiveString( char *data )
{
  this->m_StateMachine.ProcessInput( m_ReceiveStringInput );
  memcpy(data, m_InputBuffer, m_ReadDataSize);
  return true;
}

void SerialCommunication::SetLogger( LoggerType* logger )
{
    m_pLogger = logger;
    m_pCommand->SetLogger( logger );
}



SerialCommunication::LoggerType* SerialCommunication::GetLogger(  void )
{
    return m_pLogger;
}


void SerialCommunication::ClosePortSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::ClosePortSuccessProcessing called ...\n");
}

void SerialCommunication::ClosePortFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::ClosePortFailureProcessing called ...\n");
}

} // end namespace igstk

