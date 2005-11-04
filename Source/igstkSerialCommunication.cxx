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
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

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
  // default to 1 second timeout
  this->SetTimeoutPeriod(1000);

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

  m_CaptureFileName = "";
  m_Capture = false;
  m_CaptureMessageNumber = 0;
  
  m_ResultInputMap[SUCCESS] = m_SuccessInput;
  m_ResultInputMap[FAILURE] = m_FailureInput;
  m_ResultInputMap[TIMEOUT] = m_TimeoutInput;

  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState,
                           "IdleState" );

  m_StateMachine.AddState( m_AttemptingToOpenPortState,
                           "AttemptingToOpenPortState" );

  m_StateMachine.AddState( m_PortOpenState,
                           "PortOpenState" );

  m_StateMachine.AddState( m_AttemptingToUpdateParametersState,
                           "AttemptingToUpdateParametersState" );

  m_StateMachine.AddState( m_ReadyForCommunicationState,
                           "ReadyForCommunicationState" );

  m_StateMachine.AddState( m_AttemptingToClosePortState,
                           "AttemptingToClosePortState" );
                           
  m_StateMachine.AddState( m_AttemptingToReadState,
                           "AttemptingToReadState" );
                           
  m_StateMachine.AddState( m_AttemptingToWriteState,
                           "AttemptingToWriteState" );

  m_StateMachine.AddState( m_AttemptingToPurgeBuffersState,
                           "AttemptingToPurgeBuffersState" );

  m_StateMachine.AddState( m_AttemptingToSendBreakState,
                           "AttemptingToSendBreakState" );

  m_StateMachine.AddState( m_SleepState,
                           "SleepState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_SuccessInput,
                           "SuccessInput");

  m_StateMachine.AddInput( m_FailureInput,
                           "FailureInput");

  m_StateMachine.AddInput( m_TimeoutInput,
                           "TimeoutInput");

  m_StateMachine.AddInput( m_OpenPortInput,
                           "OpenPortInput");

  m_StateMachine.AddInput( m_UpdateParametersInput,
                           "UpdateParametersInput");

  m_StateMachine.AddInput( m_ClosePortInput,
                           "ClosePortInput");

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
                                m_UpdateParametersInput,
                                m_IdleState,
                                NoAction);

  // AttemptingToOpenPortState
  m_StateMachine.AddTransition( m_AttemptingToOpenPortState,
                                m_SuccessInput,
                                m_PortOpenState,
                                &SerialCommunication::OpenPortSuccessProcessing);

  m_StateMachine.AddTransition( m_AttemptingToOpenPortState,
                                m_FailureInput,
                                m_IdleState,
                                &SerialCommunication::OpenPortFailureProcessing);

  // PortOpenState
  m_StateMachine.AddTransition( m_PortOpenState,
                                m_UpdateParametersInput,
                                m_AttemptingToUpdateParametersState,
                                &SerialCommunication::AttemptToUpdateParameters);

  m_StateMachine.AddTransition( m_PortOpenState,
                                m_PurgeBuffersInput,
                                m_PortOpenState,
                                &SerialCommunication::AttemptToPurgeBuffers);

  m_StateMachine.AddTransition( m_PortOpenState,
                                m_ClosePortInput,
                                m_AttemptingToClosePortState,
                                &SerialCommunication::AttemptToClosePort);

  // AttemptingToUpdateParametersState
  m_StateMachine.AddTransition( m_AttemptingToUpdateParametersState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
                                NoAction);

  m_StateMachine.AddTransition( m_AttemptingToUpdateParametersState,
                                m_FailureInput,
                                m_PortOpenState,
                                NoAction);

  // ReadyForCommunicationState
  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_SendBreakInput,
                                m_AttemptingToSendBreakState,
                                &SerialCommunication::AttemptToSendBreak);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_PurgeBuffersInput,
                                m_AttemptingToPurgeBuffersState,
                                &SerialCommunication::AttemptToPurgeBuffers);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_SleepInput,
                                m_SleepState,
                                &SerialCommunication::SleepProcessing);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_WriteInput,
                                m_AttemptingToWriteState,
                                &SerialCommunication::AttemptToWrite);
                                
  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_ReadInput,
                                m_AttemptingToReadState,
                                &SerialCommunication::AttemptToRead);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_ClosePortInput,
                                m_AttemptingToClosePortState,
                                &SerialCommunication::AttemptToClosePort);

  m_StateMachine.AddTransition( m_ReadyForCommunicationState,
                                m_UpdateParametersInput,
                                m_AttemptingToUpdateParametersState,
                                &SerialCommunication::AttemptToUpdateParameters);

  // AttemptingToReadState
  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::SuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_FailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::FailureProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToReadState,
                                m_TimeoutInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::TimeoutProcessing);

  // AttemptingToWriteState
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::SuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_FailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::FailureProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToWriteState,
                                m_TimeoutInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::TimeoutProcessing);

  // AttemptingToSendBreakState
  m_StateMachine.AddTransition( m_AttemptingToSendBreakState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::SuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToSendBreakState,
                                m_FailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::FailureProcessing);
                                
  // AttemptingToPurgeBuffersState
  m_StateMachine.AddTransition( m_AttemptingToPurgeBuffersState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::SuccessProcessing);
                                
  m_StateMachine.AddTransition( m_AttemptingToPurgeBuffersState,
                                m_FailureInput,
                                m_ReadyForCommunicationState,
                                &SerialCommunication::FailureProcessing);

  // AttemptingToClosePortState
  m_StateMachine.AddTransition( m_AttemptingToClosePortState,
                                m_SuccessInput,
                                m_IdleState,
                                NoAction);

  m_StateMachine.AddTransition( m_AttemptingToClosePortState,
                                m_FailureInput,
                                m_PortOpenState,
                                &SerialCommunication::ClosePortFailureProcessing);

  // SleepState
  m_StateMachine.AddTransition( m_SleepState,
                                m_SuccessInput,
                                m_ReadyForCommunicationState,
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


SerialCommunication::ResultType
SerialCommunication::OpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::OpenCommunication called ...\n");

  // Attempt to open communication port
  m_StateMachine.PushInput( m_OpenPortInput );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
}


SerialCommunication::ResultType
SerialCommunication::CloseCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::CloseCommunication"
                 " called ...\n" );

  m_StateMachine.PushInput( m_ClosePortInput );
  m_StateMachine.ProcessInputs();

  m_CaptureFileStream.close();

  return m_ReturnValue;
}


SerialCommunication::ResultType
SerialCommunication::UpdateParameters( void )
{
  igstkLogMacro( DEBUG, "UpdateParameters called ...\n" );

  m_StateMachine.PushInput( m_UpdateParametersInput );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
}


SerialCommunication::ResultType 
SerialCommunication::SendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::SendBreak called ...\n" );

  m_StateMachine.PushInput( m_SendBreakInput );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
}


SerialCommunication::ResultType 
SerialCommunication::PurgeBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunication::PurgeBuffers called ...\n" );

  m_StateMachine.PushInput( m_PurgeBuffersInput );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
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
  // In case the data contains nulls or non-graphical characters,
  // encode it before logging it
  std::string encodedString;
  igstk::BinaryData::Encode(encodedString, (unsigned char *)data,
                            numberOfBytes);
  igstkLogMacro( DEBUG, "SerialCommunication::Write(" 
                 << encodedString << ", " << numberOfBytes
                 << ") called...\n" );

  m_OutputData = data;
  m_BytesToWrite = numberOfBytes;

  // Recording for data sent
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    m_CaptureMessageNumber++;

    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                    << ". command[" << numberOfBytes << "] "
                    << encodedString << std::endl );
    }

  m_StateMachine.PushInput( m_WriteInput );
  m_StateMachine.ProcessInputs();

  return m_ReturnValue;
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


  // In case the data contains nulls or non-graphical characters,
  // encode it before logging it
  std::string encodedString;
  BinaryData::Encode(encodedString, (unsigned char*)data, bytesRead);

  // Recording for data received
  if( m_Capture && m_CaptureFileStream.is_open() )
    {
    igstkLogMacro2( m_Recorder, INFO, m_CaptureMessageNumber
                    << ". receive[" << bytesRead << "] "
                    << encodedString << std::endl );
    }

  igstkLogMacro( DEBUG, "SerialCommunication::Read(" << encodedString << ", "
                 << numberOfBytes << ", " << bytesRead << ") called...\n" );

  return m_ReturnValue;
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
  m_StateMachine.PushInput( m_UpdateParametersInput );
}

const SerialCommunication::InputType &
SerialCommunication::MapResultToInput( int condition )
{  
  IntegerInputMapType& conditionInputMap = m_ResultInputMap;
  const InputType *input = &m_FailureInput;
  IntegerInputMapType::const_iterator itr;

  itr = conditionInputMap.find(condition);
  if ( itr != conditionInputMap.end() )
    {
    input = & (itr->second);
    }
  else
    {
    igstkLogMacro( WARNING, " Default Input is pushed because the"
      << " condition value didn't have a corresponding input."
      << " Please make sure the condition value is correct."
      << " Condition: \n" );
    }

  return *input;
}
  

void SerialCommunication::AttemptToWrite()
{
  int condition = this->InternalWrite( m_OutputData,
                                       m_BytesToWrite );

  m_StateMachine.PushInput( this->MapResultToInput(condition) );
}


void SerialCommunication::AttemptToRead()
{
  int condition = this->InternalRead( m_InputData,
                                      m_BytesToRead,
                                      m_BytesRead );

  m_StateMachine.PushInput( this->MapResultToInput(condition) );
}


void SerialCommunication::AttemptToOpenPort()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalOpenPort(), 
                                   m_SuccessInput,
                                   m_FailureInput );
}


void SerialCommunication::AttemptToUpdateParameters()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalUpdateParameters(),
                                   m_SuccessInput,
                                   m_FailureInput );
}


void SerialCommunication::AttemptToClosePort()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalClosePort(),
                                   m_SuccessInput,
                                   m_FailureInput );
}


void SerialCommunication::AttemptToSendBreak()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalSendBreak(),
                                   m_SuccessInput,
                                   m_FailureInput);
}


void SerialCommunication::AttemptToPurgeBuffers()
{
  m_StateMachine.PushInputBoolean( (bool)this->InternalPurgeBuffers(),
                                   m_SuccessInput,
                                   m_FailureInput);
}


void SerialCommunication::SleepProcessing()
{
  this->InternalSleep(m_SleepPeriod);

  m_StateMachine.PushInput( m_SuccessInput );
}


void SerialCommunication::SuccessProcessing()
{
  m_ReturnValue = SUCCESS;
  this->InvokeEvent( CompletedEvent() );
}


void SerialCommunication::FailureProcessing()
{
  m_ReturnValue = FAILURE;
  this->InvokeEvent( InputOutputErrorEvent() );
}


void SerialCommunication::TimeoutProcessing()
{
  m_ReturnValue = TIMEOUT;
  this->InvokeEvent( InputOutputTimeoutEvent() );
}


void SerialCommunication::OpenPortFailureProcessing()
{
  m_ReturnValue = FAILURE;
  this->InvokeEvent( OpenPortErrorEvent() );
}


void SerialCommunication::ClosePortFailureProcessing()
{
  m_ReturnValue = FAILURE;
  this->InvokeEvent( ClosePortErrorEvent() );
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

  os << indent << "Capture: " << m_Capture << std::endl;
  os << indent << "CaptureFileName: " << m_CaptureFileName << std::endl;
  os << indent << "CaptureFileStream: " << m_CaptureFileStream << std::endl;
  os << indent << "CaptureMessageNumber: " << m_CaptureMessageNumber
     << std::endl;
}


} // end namespace igstk

