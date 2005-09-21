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

#include "igstkBinaryData.h"
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

  m_CaptureFileName = "";
  m_Capture = false;
  m_CaptureMessageNumber = 0;
  
  m_WriteResultInputMap[SUCCESS] = m_WriteSuccessInput;
  m_WriteResultInputMap[FAILURE] = m_WriteFailureInput;
  m_WriteResultInputMap[TIMEOUT] = m_WriteTimeoutInput;

  m_ReadResultInputMap[SUCCESS] = m_ReadSuccessInput;
  m_ReadResultInputMap[FAILURE] = m_ReadFailureInput;
  m_ReadResultInputMap[TIMEOUT] = m_ReadTimeoutInput;

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
                           "ReadyForCommunicationState" );

  m_StateMachine.AddState( m_AttemptingToClosePortState,
                           "m_AttemptingToClosePortState" );
                           
  m_StateMachine.AddState( m_AttemptingToReadState,
                           "m_AttemptingToReadState" );
                           
  m_StateMachine.AddState( m_AttemptingToWriteState,
                           "m_AttemptingToWriteState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_OpenPortInput,
                           "OpenPortInput");

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
                           
  m_StateMachine.AddInput( m_WriteSuccessInput,
                           "m_WriteSuccessInput" );
                           
  m_StateMachine.AddInput( m_WriteFailureInput,
                           "m_WriteFailureInput" );
                           
  m_StateMachine.AddInput( m_WriteTimeoutInput,
                           "m_WriteTimeoutInput" );
                           
  m_StateMachine.AddInput( m_ReadSuccessInput,
                           "m_ReadSuccessInput" );
                           
  m_StateMachine.AddInput( m_ReadFailureInput,
                           "m_ReadFailureInput" );
                           
  m_StateMachine.AddInput( m_ReadTimeoutInput,
                           "m_ReadTimeoutInput" );
                           
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
                                m_AttemptingToWriteState,
                                &SerialCommunication::AttemptToWrite);
                                
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_WriteSuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::WriteSuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_WriteFailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::WriteFailureProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_WriteTimeoutInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::WriteTimeoutProcessing);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_ReadInput,
                                m_AttemptingToReadState,
                                &SerialCommunication::AttemptToRead);

  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_ReadSuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::ReadSuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_ReadFailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::ReadFailureProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_ReadTimeoutInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::ReadTimeoutProcessing);

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

} 


// Destructor 
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}


void SerialCommunication::SetCaptureFileName(const char* filename)
{
  m_CaptureFileName = filename;
}


const char *SerialCommunication::GetCaptureFileName() const
{
  return m_CaptureFileName.c_str();
}


void SerialCommunication::OpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenCommunication called ...\n");

  // Attempt to open communication port
  m_StateMachine.PushInput( m_OpenPortInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::CloseCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::CloseCommunication"
                 " called ...\n" );

  m_StateMachine.PushInput( m_ClosePortInput );
  m_StateMachine.ProcessInputs();

  m_CaptureFileStream.close();
}


void SerialCommunication::SetBaudRate( BaudRateType baudRate )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetBaudRate(" << baudRate
                 << ") called ...\n" );

  m_BaudRate = baudRate;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetDataBits( DataBitsType bits)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetDataBits(" << bits
                 << ") called ...\n" );

  m_DataBits = bits;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetParity( ParityType parity)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetParity(" << parity
                 << ") called ...\n" );

  m_Parity = parity;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetStopBits( StopBitsType bits)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetStopBits(" << bits
                 << ") called ...\n" );

  m_StopBits = bits;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetHardwareHandshake( HandshakeType handshake)
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetHardwareHandshake("
                 << handshake << ") called ...\n" );

  m_HardwareHandshake = handshake;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SetTimeoutPeriod( unsigned int milliseconds )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SetTimeoutPeriod("
                 << milliseconds << ") called ...\n" );

  m_TimeoutPeriod = milliseconds;

  m_StateMachine.PushInput( m_SetTransferParametersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::SendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SendBreak called ...\n" );

  m_StateMachine.PushInput( m_SendBreakInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::PurgeBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::PurgeBuffers called ...\n" );

  m_StateMachine.PushInput( m_PurgeBuffersInput );
  m_StateMachine.ProcessInputs();
}


void SerialCommunication::Sleep( unsigned int milliseconds )
{
  igstkLogMacro( DEBUG, "SerialCommunication::Sleep(" << milliseconds
                 << ") called ...\n" );

  m_SleepPeriod = milliseconds;
  m_StateMachine.PushInput( m_SleepInput );
  m_StateMachine.ProcessInputs();
}


SerialCommunication::ResultType 
SerialCommunication::Write( const char *data, unsigned int numberOfBytes )
{
  // this call to the log macro assumes data is null terminated
  igstkLogMacro( DEBUG, "SerialCommunication::Write(" 
                 << data << ", " << numberOfBytes << ") called...\n" );

  m_OutputData = data;
  m_BytesToWrite = numberOfBytes;

  // Recording for data sent
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    m_CaptureMessageNumber++;
    std::string encodedString;
    BinaryData::Encode(encodedString, (const unsigned char*)m_OutputData, numberOfBytes);
    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                        << ". command[" << numberOfBytes << "] " << encodedString << std::endl );
    }

  m_StateMachine.PushInput( m_WriteInput );
  m_StateMachine.ProcessInputs();
  return m_WriteReturnValue;
}


SerialCommunication::ResultType 
SerialCommunication::Read( char *data, unsigned int numberOfBytes,
                                unsigned int &bytesRead )
{
  m_BytesRead = 0;
  m_BytesToRead = numberOfBytes;
  m_InputData = data; 

  m_StateMachine.PushInput( m_ReadInput );
  m_StateMachine.ProcessInputs();

  bytesRead = m_BytesRead;

  // terminate the string
  data[bytesRead] = '\0';

  // Recording for data received
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    std::string encodedString;
    BinaryData::Encode(encodedString, (const unsigned char*)m_InputData, bytesRead);
    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                        << ". receive[" << bytesRead << "] " << encodedString << std::endl );
    }

  igstkLogMacro( DEBUG, "SerialCommunication::Read(" << data << ", "
                 << numberOfBytes << ", " << bytesRead << ") called...\n" );
  return m_ReadReturnValue;
}


void SerialCommunication::OpenPortSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenPortSuccessProcessing"
                 " called ...\n");

  // Open a file for writing data stream.
  if( m_Capture )
    {
    time_t ti;
    time(&ti);

    igstkLogMacro( DEBUG, "Capture is on. Filename: "
                   << m_CaptureFileName << "\n" );

    if( m_Recorder.IsNull() )
      {
      m_Recorder = itk::Logger::New();
      m_Recorder->SetPriorityLevel(itk::Logger::DEBUG);
      m_Recorder->SetLevelForFlushing(itk::Logger::DEBUG);
      }
    if( m_CaptureFileOutput.IsNull() )
      {
      m_CaptureFileOutput = itk::StdStreamLogOutput::New();
      }
    m_CaptureFileStream.open(m_CaptureFileName.c_str());
    if( !m_CaptureFileStream.is_open() )
      {
      igstkLogMacro( CRITICAL,
                     "failed to open a file for writing data stream.\n" );
      }
    else
      {
      m_CaptureFileOutput->SetStream( m_CaptureFileStream );
      m_Recorder->AddLogOutput( m_CaptureFileOutput );
      igstkLogMacro2( m_Recorder, DEBUG, "# recorded " << asctime(localtime(&ti))
                          << "\r\n" );
      }
    }

   // if the port was opened successfully, then set transfer parameters next
  m_StateMachine.PushInput( m_SetTransferParametersInput );
}


void SerialCommunication::AttemptToWrite()
{
  int condition = this->InternalWrite();
  IntegerInputMapType& conditionInputMap = m_WriteResultInputMap;
  const InputType *input = &m_WriteFailureInput;
  IntegerInputMapType::const_iterator itr;

  itr = conditionInputMap.find(condition);
  if ( itr != conditionInputMap.end() )
    {
    input = & (itr->second);
    }
  else
    {
    std::cerr << "WARNING : Default Input is pushed because the condition value didn't have a corresponding input."
              << " Please make sure the condition value is correct. condition: " << condition << std::endl;
    }

  m_StateMachine.PushInput( *input );
}


void SerialCommunication::AttemptToRead()
{
  int condition = this->InternalRead();
  IntegerInputMapType& conditionInputMap = m_ReadResultInputMap;
  const InputType *input = & m_ReadFailureInput;
  IntegerInputMapType::const_iterator itr;

  itr = conditionInputMap.find(condition);
  if ( itr != conditionInputMap.end() )
    {
    input = & (itr->second);
    }
  else
    {
    std::cerr << "WARNING : Default Input is pushed because the condition value didn't have a corresponding input."
              << " Please make sure the condition value is correct. condition: " << condition << std::endl;
    }

  m_StateMachine.PushInput( *input );
}


void SerialCommunication::AttemptToOpenPort()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalOpenPort(), 
                                   m_OpenPortSuccessInput,
                                   m_OpenPortFailureInput );
}


void SerialCommunication::AttemptToSetTransferParameters()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalSetTransferParameters(),
                                   m_SetTransferParametersSuccessInput,
                                   m_SetTransferParametersFailureInput );
}


void SerialCommunication::AttemptToClosePort()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalClosePort(),
                                   m_ClosePortSuccessInput,
                                   m_ClosePortFailureInput );
}


void SerialCommunication::WriteSuccessProcessing()
{
  m_WriteReturnValue = SUCCESS;
  this->InvokeEvent( WriteSuccessEvent() );
}


void SerialCommunication::WriteFailureProcessing()
{
  m_WriteReturnValue = FAILURE;
  this->InvokeEvent( WriteFailureEvent() );
}


void SerialCommunication::WriteTimeoutProcessing()
{
  m_WriteReturnValue = TIMEOUT;
  this->InvokeEvent( WriteTimeoutEvent() );
}


void SerialCommunication::ReadSuccessProcessing()
{
  m_ReadReturnValue = SUCCESS;
  this->InvokeEvent( ReadSuccessEvent());
}


void SerialCommunication::ReadFailureProcessing()
{
  m_ReadReturnValue = FAILURE;
  this->InvokeEvent( ReadFailureEvent() );
}


void SerialCommunication::ReadTimeoutProcessing()
{
  m_ReadReturnValue = TIMEOUT;
  this->InvokeEvent( ReadTimeoutEvent() );
}


void SerialCommunication::PrintSelf( std::ostream& os,
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortNumber: " << m_PortNumber << std::endl;
  os << indent << "BaudRate: " << m_BaudRate << std::endl;
  os << indent << "DataBits: " << m_DataBits << std::endl;
  os << indent << "Parity: " << m_Parity << std::endl;
  os << indent << "StopBits: " << m_StopBits << std::endl;
  os << indent << "HardwareHandshake: " << m_HardwareHandshake << std::endl;
  os << indent << "TimeoutPeriod: " << m_TimeoutPeriod << std::endl;

  os << indent << "Capture: " << m_Capture << std::endl;
  os << indent << "CaptureFileName: " << m_CaptureFileName << std::endl;
  os << indent << "CaptureFileStream: " << m_CaptureFileStream << std::endl;
  os << indent << "CaptureMessageNumber: " << m_CaptureMessageNumber
     << std::endl;
}


} // end namespace igstk

