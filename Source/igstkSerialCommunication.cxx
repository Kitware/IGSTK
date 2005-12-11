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
  igstkAddStateMacro( IdleState ); 
  igstkAddStateMacro( AttemptingToOpenPortState ); 
  igstkAddStateMacro( PortOpenState ); 
  igstkAddStateMacro( AttemptingToUpdateParametersState ); 
  igstkAddStateMacro( ReadyForCommunicationState ); 
  igstkAddStateMacro( AttemptingToClosePortState ); 
  igstkAddStateMacro( AttemptingToReadState ); 
  igstkAddStateMacro( AttemptingToWriteState ); 
  igstkAddStateMacro( AttemptingToPurgeBuffersState ); 
  igstkAddStateMacro( AttemptingToSendBreakState ); 
  igstkAddStateMacro( SleepState );

  // Set the input descriptors
  igstkAddInputMacro( SuccessInput ); 
  igstkAddInputMacro( FailureInput ); 
  igstkAddInputMacro( TimeoutInput ); 
  igstkAddInputMacro( OpenPortInput ); 
  igstkAddInputMacro( UpdateParametersInput ); 
  igstkAddInputMacro( ClosePortInput ); 
  igstkAddInputMacro( SendBreakInput ); 
  igstkAddInputMacro( PurgeBuffersInput ); 
  igstkAddInputMacro( SleepInput ); 
  igstkAddInputMacro( WriteInput ); 
  igstkAddInputMacro( ReadInput ); 

  // ----------------------------------------
  // Programming the state machine transitions


  // IdleState
  igstkAddTransitionMacro( IdleState,
                           OpenPortInput,
                           AttemptingToOpenPortState,
                           AttemptToOpenPort);

  igstkAddTransitionMacro( IdleState,
                           ClosePortInput,
                           IdleState,
                           NoAction);

  igstkAddTransitionMacro( IdleState,
                           UpdateParametersInput,
                           IdleState,
                           NoAction);

  // AttemptingToOpenPortState
  igstkAddTransitionMacro( AttemptingToOpenPortState,
                           SuccessInput,
                           PortOpenState,
                           OpenPortSuccessProcessing);

  igstkAddTransitionMacro( AttemptingToOpenPortState,
                           FailureInput,
                           IdleState,
                           OpenPortFailureProcessing);

  // PortOpenState
  igstkAddTransitionMacro( PortOpenState,
                           UpdateParametersInput,
                           AttemptingToUpdateParametersState,
                           AttemptToUpdateParameters);

  igstkAddTransitionMacro( PortOpenState,
                           PurgeBuffersInput,
                           PortOpenState,
                           AttemptToPurgeBuffers);

  igstkAddTransitionMacro( PortOpenState,
                           ClosePortInput,
                           AttemptingToClosePortState,
                           AttemptToClosePort);

  // AttemptingToUpdateParametersState
  igstkAddTransitionMacro( AttemptingToUpdateParametersState,
                           SuccessInput,
                           ReadyForCommunicationState,
                           NoAction);

  igstkAddTransitionMacro( AttemptingToUpdateParametersState,
                           FailureInput,
                           PortOpenState,
                           NoAction);

  // ReadyForCommunicationState
  igstkAddTransitionMacro( ReadyForCommunicationState,
                           SendBreakInput,
                           AttemptingToSendBreakState,
                           AttemptToSendBreak);

  igstkAddTransitionMacro( ReadyForCommunicationState,
                           PurgeBuffersInput,
                           AttemptingToPurgeBuffersState,
                           AttemptToPurgeBuffers);

  igstkAddTransitionMacro( ReadyForCommunicationState,
                           SleepInput,
                           SleepState,
                           SleepProcessing);

  igstkAddTransitionMacro( ReadyForCommunicationState,
                           WriteInput,
                           AttemptingToWriteState,
                           AttemptToWrite);
                                
  igstkAddTransitionMacro( ReadyForCommunicationState,
                           ReadInput,
                           AttemptingToReadState,
                           AttemptToRead);

  igstkAddTransitionMacro( ReadyForCommunicationState,
                           ClosePortInput,
                           AttemptingToClosePortState,
                           AttemptToClosePort);

  igstkAddTransitionMacro( ReadyForCommunicationState,
                           UpdateParametersInput,
                           AttemptingToUpdateParametersState,
                           AttemptToUpdateParameters);

  // AttemptingToReadState
  igstkAddTransitionMacro( AttemptingToReadState,
                           SuccessInput,
                           ReadyForCommunicationState,
                           SuccessProcessing);
                                
  igstkAddTransitionMacro( AttemptingToReadState,
                           FailureInput,
                           ReadyForCommunicationState,
                           FailureProcessing);
                                
  igstkAddTransitionMacro( AttemptingToReadState,
                           TimeoutInput,
                           ReadyForCommunicationState,
                           TimeoutProcessing);

  // AttemptingToWriteState
  igstkAddTransitionMacro( AttemptingToWriteState,
                           SuccessInput,
                           ReadyForCommunicationState,
                           SuccessProcessing);
                                
  igstkAddTransitionMacro( AttemptingToWriteState,
                           FailureInput,
                           ReadyForCommunicationState,
                           FailureProcessing);
                                
  igstkAddTransitionMacro( AttemptingToWriteState,
                           TimeoutInput,
                           ReadyForCommunicationState,
                           TimeoutProcessing);

  // AttemptingToSendBreakState
  igstkAddTransitionMacro( AttemptingToSendBreakState,
                           SuccessInput,
                           ReadyForCommunicationState,
                           SuccessProcessing);
                                
  igstkAddTransitionMacro( AttemptingToSendBreakState,
                           FailureInput,
                           ReadyForCommunicationState,
                           FailureProcessing);
                                
  // AttemptingToPurgeBuffersState
  igstkAddTransitionMacro( AttemptingToPurgeBuffersState,
                           SuccessInput,
                           ReadyForCommunicationState,
                           SuccessProcessing);
                                
  igstkAddTransitionMacro( AttemptingToPurgeBuffersState,
                           FailureInput,
                           ReadyForCommunicationState,
                           FailureProcessing);

  // AttemptingToClosePortState
  igstkAddTransitionMacro( AttemptingToClosePortState,
                           SuccessInput,
                           IdleState,
                           NoAction);

  igstkAddTransitionMacro( AttemptingToClosePortState,
                           FailureInput,
                           PortOpenState,
                           ClosePortFailureProcessing);

  // SleepState
  igstkAddTransitionMacro( SleepState,
                           SuccessInput,
                           ReadyForCommunicationState,
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
  this->SetLogger(0); 
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
                          << "\n" );
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


void SerialCommunication::NoAction()
{
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

