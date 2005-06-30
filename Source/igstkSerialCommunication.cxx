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


namespace igstk
{ 
/** Constructor */
SerialCommunication::SerialCommunication() :  m_StateMachine( this ),
                                              m_ReadBufferSize(3000),
                                              m_WriteBufferSize(200),
                                              m_InputBuffer( NULL),
                                              m_OutputBuffer( NULL),
                                              m_ReadDataSize(0),
                                              m_PortRestSpan(10),
                                              m_PortNumber( PortNumber0() ),
                                              m_BaudRate( BaudRate9600() ),
                                              m_ByteSize( DataBits8() ),
                                              m_Parity( NoParity() ),
                                              m_StopBits( StopBits1() ),
                                              m_HardwareHandshake( HandshakeOff() )
{
/*
  std::cout << m_OpenPortInput.GetIdentifier() << std::endl;
  std::cout << m_OpenPortSuccessInput << std::endl;
  std::cout << m_OpenPortFailureInput << std::endl;

  std::cout << m_SetUpDataBuffersInput << std::endl;
  std::cout << m_DataBuffersSetUpSuccessInput << std::endl;
  std::cout << m_DataBuffersSetUpFailureInput << std::endl;

  std::cout << m_SetUpDataTransferParametersInput << std::endl;
  std::cout << m_DataTransferParametersSetUpSuccessInput << std::endl;
  std::cout << m_DataTransferParametersSetUpFailureInput << std::endl;

  std::cout << m_RestPortInput << std::endl;
  std::cout << m_FlushOutputBufferInput << std::endl;
  std::cout << m_ReceiveStringInput << std::endl;
  std::cout << m_SendStringInput << std::endl;

  std::cout << m_ClosePortInput << std::endl;
  std::cout << m_ClosePortSuccessInput << std::endl;
  std::cout << m_ClosePortFailureInput << std::endl;
*/


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
  m_StateMachine.AddTransition( m_IdleState, m_ClosePortInput, m_IdleState, NoAction);

  m_StateMachine.AddTransition( m_AttemptToClosePortState, m_ClosePortSuccessInput, m_IdleState, &SerialCommunication::ClosePortSuccessProcessing);
  m_StateMachine.AddTransition( m_IdleState, m_ClosePortSuccessInput, m_IdleState, NoAction);
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
  igstkLogMacro( DEBUG, "SerialCommunication::OpenCommunication called ...\n");
  // Attempt to open communication port
  this->m_StateMachine.PushInput( m_OpenPortInput );
  this->m_StateMachine.ProcessInputs();
  this->m_StateMachine.PushInput( *m_pOpenPortResultInput );
  this->m_StateMachine.ProcessInputs();

  if( m_pOpenPortResultInput == &m_OpenPortFailureInput )
  {
    return false;
  }

  // Attempt to set up data buffers
  this->m_StateMachine.PushInput( m_SetUpDataBuffersInput );
  this->m_StateMachine.ProcessInputs();
  this->m_StateMachine.PushInput( *m_pDataBuffersSetUpResultInput );
  this->m_StateMachine.ProcessInputs();

  if( m_pDataBuffersSetUpResultInput == &m_DataBuffersSetUpFailureInput )
  {
    return false;
  }
 
  // Attempt to set up communication parameters 
  this->m_StateMachine.PushInput( m_SetUpDataTransferParametersInput );
  this->m_StateMachine.ProcessInputs();
  this->m_StateMachine.PushInput( *m_pDataTransferParametersSetUpResultInput );
  this->m_StateMachine.ProcessInputs();

  // Return "true" if successful, else return "false"
  return(  (m_pOpenPortResultInput==&m_OpenPortSuccessInput)
         &&(m_pDataBuffersSetUpResultInput==&m_DataBuffersSetUpSuccessInput)
         &&(m_pDataTransferParametersSetUpResultInput==&m_DataTransferParametersSetUpSuccessInput) );
}


bool SerialCommunication::CloseCommunication( void )
{
  // igstkLogMacro( DEBUG, "SerialCommunication::CloseCommunication called ...\n");
  this->m_StateMachine.PushInput( m_ClosePortInput );
  this->m_StateMachine.ProcessInputs();
  this->m_StateMachine.PushInput( *m_pClosePortResultInput );
  this->m_StateMachine.ProcessInputs();
  return(m_pClosePortResultInput==&m_ClosePortSuccessInput);
}


bool SerialCommunication::RestCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::RestCommunication called ...\n");
  this->m_StateMachine.PushInput( m_RestPortInput );
  this->m_StateMachine.ProcessInputs();
  return true;
}


bool SerialCommunication::Flush( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Flush called ...\n");
  this->m_StateMachine.PushInput( m_FlushOutputBufferInput );
  this->m_StateMachine.ProcessInputs();
  return true;
}


bool SerialCommunication::SendString( const char *data )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SendString called ...\n");
  if( m_OutputBuffer != NULL )
  {
    strncpy(m_OutputBuffer, data, m_WriteBufferSize);
    igstkLogMacro( DEBUG, "Message = " << m_OutputBuffer << std::endl );
  }
  igstkLogMacro( DEBUG, "Message length = " << strlen(data) << std::endl );
  this->m_StateMachine.PushInput( m_SendStringInput );
  this->m_StateMachine.ProcessInputs();
  return true;
}

bool SerialCommunication::ReceiveString( char *data )
{
  igstkLogMacro( DEBUG, "SerialCommunication::ReceiveString called ...\n");
  this->m_StateMachine.PushInput( m_ReceiveStringInput );
  this->m_StateMachine.ProcessInputs();
  if( m_InputBuffer != NULL )
  {
    strncpy(data, m_InputBuffer, m_ReadDataSize);
  }
  data[m_ReadDataSize] = '\0'; // terminate the string
  igstkLogMacro( DEBUG, "SerialCommunication::ReceiveString : (" << m_ReadDataSize << ") " << data << "...\n");
  return true;
}

void SerialCommunication::SetLogger( LoggerType* logger )
{
    m_pLogger = logger;
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

/** Print Self function */
void SerialCommunication::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Port number: " << m_PortNumber.Get() << std::endl;
  os << indent << "Baud rate: " << m_BaudRate.Get() << std::endl;
  os << indent << "Number of bits per byte: " << m_ByteSize.Get() << std::endl;
  os << indent << "Parity: " << m_Parity.Get() << std::endl;
  os << indent << "StopBits: " << m_StopBits.Get() << std::endl;
  os << indent << "HardwareHandshake: " << m_HardwareHandshake.Get() << std::endl;
  os << indent << "ReadIntervalTimeout: " << m_ReadIntervalTimeout << std::endl;
  os << indent << "ReadTotalTimeoutMultiplier: " << m_ReadTotalTimeoutMultiplier << std::endl;
  os << indent << "ReadTotalTimeoutConstant: " << m_ReadTotalTimeoutConstant << std::endl;
  os << indent << "WriteTotalTimeoutMultiplier: " << m_WriteTotalTimeoutMultiplier << std::endl;
  os << indent << "WriteTotalTimeoutConstant: " << m_WriteTotalTimeoutConstant << std::endl;
  os << indent << "ReadBufferOffset: " << m_ReadBufferOffset << std::endl;
  os << indent << "ReadDataSize: " << m_ReadDataSize << std::endl;
}


} // end namespace igstk

