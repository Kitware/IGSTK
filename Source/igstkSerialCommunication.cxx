/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSerialCommunication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

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
m_ReadBufferSize(3000), m_WriteBufferSize(200), m_InputBuffer( NULL),
m_OutputBuffer( NULL), m_PortRestSpan(10), m_InvalidPortNumber( -1 )
{
  /** Default communication settings */
  this->m_BaudRate = BAUD9600;
  this->m_ByteSize = EIGHT_BITS;
  this->m_Parity = NO_PARITY;
  this->m_StopBits = ONE_STOP_BIT;
  this->m_HardwareHandshake = HANDSHAKE_OFF;

  /** Hardware Port Settings */
  this->m_PortNumber = m_InvalidPortNumber;

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
  m_StateMachine.AddState( m_AttemptingToOpenPortState, "AttemptingToOpenPortState" );
  m_StateMachine.AddState( m_PortOpenState, "PortOpenState" );

  m_StateMachine.AddState( m_AttemptingToSetUpDataBuffersState, "AttemptingToSetUpDataBuffersState" );
  m_StateMachine.AddState( m_DataBuffersSetState, "DataBuffersSetState" );

  m_StateMachine.AddState( m_AttemptingToSetUpDataTransferParametersState, "AttemptingToSetUpDataTransferParametersState" );
  m_StateMachine.AddState( m_PortReadyForCommunicationState, "PortReadyForCommunication" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_OpenPortInput, "OpenPort");
  m_StateMachine.AddInput( m_OpenPortSuccess, "OpenPortSuccess");
  m_StateMachine.AddInput( m_OpenPortFailure, "OpenPortFailure");

  m_StateMachine.AddInput( m_SetUpDataBuffersInput, "SetUpDataBuffersInput");
  m_StateMachine.AddInput( m_DataBufferSetUpSuccess, "DataBufferSetUpSuccess");
  m_StateMachine.AddInput( m_DataBufferSetUpFailure, "DataBufferSetUpFailure");
 
  m_StateMachine.AddInput( m_SetUpDataTransferParametersInput, "SetUpDataTransferParameters");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpSuccess, "DataTransferParametersSetUpSuccess");
  m_StateMachine.AddInput( m_DataTransferParametersSetUpFailure, "DataTransferParametersSetUpFailure");

  m_StateMachine.AddInput( m_ClosePortInput, "ClosePort");
  m_StateMachine.AddInput( m_RestCommunication, "RestCommunication");

  m_StateMachine.AddInput( m_FlushOutputBuffer, "FlushOutputBuffer");
  m_StateMachine.AddInput( m_SendString, "SendString");
  m_StateMachine.AddInput( m_ReceiveString, "ReceiveString");

  const ActionType NoAction = 0;

  // Programming the state machine transitions
  m_StateMachine.AddTransition( m_IdleState, m_OpenPortInput, m_AttemptingToOpenPortState, &SerialCommunication::OpenCommunicationPortProcessing);
  m_StateMachine.AddTransition( m_AttemptingToOpenPortState, m_OpenPortSuccess, m_PortOpenState, NoAction);
  m_StateMachine.AddTransition( m_AttemptingToOpenPortState, m_OpenPortFailure, m_IdleState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_SetUpDataBuffersInput, m_AttemptingToSetUpDataBuffersState, &SerialCommunication::SetUpDataBuffersProcessing);
  m_StateMachine.AddTransition( m_AttemptingToSetUpDataBuffersState, m_DataBufferSetUpSuccess, m_DataBuffersSetState, NoAction);
  m_StateMachine.AddTransition( m_AttemptingToSetUpDataBuffersState, m_DataBufferSetUpFailure, m_PortOpenState, NoAction);

  m_StateMachine.AddTransition( m_DataBuffersSetState, m_SetUpDataTransferParametersInput, m_AttemptingToSetUpDataTransferParametersState, &SerialCommunication::SetUpDataTransferParametersProcessing);
  m_StateMachine.AddTransition( m_AttemptingToSetUpDataTransferParametersState, m_DataTransferParametersSetUpSuccess, m_PortReadyForCommunicationState, NoAction);
  m_StateMachine.AddTransition( m_AttemptingToSetUpDataTransferParametersState, m_DataTransferParametersSetUpFailure, m_DataBuffersSetState, NoAction);

  m_StateMachine.AddTransition( m_PortOpenState, m_ClosePortInput, m_IdleState, &SerialCommunication::CloseCommunicationPortProcessing);
  m_StateMachine.AddTransition( m_DataBuffersSetState, m_ClosePortInput, m_IdleState, &SerialCommunication::ClearBuffersAndCloseCommunicationPortProcessing);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ClosePortInput, m_IdleState, &SerialCommunication::ClearBuffersAndCloseCommunicationPortProcessing);

  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_RestCommunication, m_PortReadyForCommunicationState, &SerialCommunication::RestCommunicationProcessing);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_FlushOutputBuffer, m_PortReadyForCommunicationState, &SerialCommunication::FlushOutputBufferProcessing);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_SendString, m_PortReadyForCommunicationState, &SerialCommunication::SendStringProcessing);
  m_StateMachine.AddTransition( m_PortReadyForCommunicationState, m_ReceiveString, m_PortReadyForCommunicationState, &SerialCommunication::ReceiveStringProcessing);

  m_StateMachine.SelectInitialState( m_IdleState );

  // Initialize booleans
  m_ResultOfOpenCommunicationPortProcessing = false;
  m_ResultOfSetUpDataBuffersProcessing = false;
  m_ResultOfSetUpCommunicationParametersProcessing = false;

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}

void SerialCommunication::CloseCommunication( void )
{
  this->m_StateMachine.ProcessInput( m_ClosePortInput );
}


void SerialCommunication::OpenCommunication( const void *data )
{
  // Read data from XML file

  // Attempt to open communication port
  this->m_PortNumber = 0;
  m_ResultOfOpenCommunicationPortProcessing = false;
  this->m_StateMachine.ProcessInput( m_OpenPortInput );
  if (m_ResultOfOpenCommunicationPortProcessing)
  {
    this->m_StateMachine.ProcessInput( m_OpenPortSuccess );
  }
  else
  {
    this->m_StateMachine.ProcessInput( m_OpenPortFailure );
  }

  // Attempt to set up data buffers
  m_ResultOfSetUpDataBuffersProcessing = false;
  this->m_StateMachine.ProcessInput( m_SetUpDataBuffersInput );
  if (m_ResultOfSetUpDataBuffersProcessing)
  {
    this->m_StateMachine.ProcessInput( m_DataBufferSetUpSuccess );
  }
  else
  {
    this->m_StateMachine.ProcessInput( m_DataBufferSetUpFailure );
  }

  // Attempt to set up communication parameters 
  m_ResultOfSetUpCommunicationParametersProcessing = false;
  this->m_StateMachine.ProcessInput( m_SetUpDataTransferParametersInput );
  if (m_ResultOfSetUpCommunicationParametersProcessing)
  {
    this->m_StateMachine.ProcessInput( m_DataTransferParametersSetUpSuccess );
  }
  else
  {
    this->m_StateMachine.ProcessInput( m_DataTransferParametersSetUpFailure );
  }
}

void SerialCommunication::RestCommunication( void )
{
  this->m_StateMachine.ProcessInput( m_RestCommunication );
}


void SerialCommunication::FlushOutputBuffer( void )
{
  this->m_StateMachine.ProcessInput( m_FlushOutputBuffer );
}


void SerialCommunication::SendString( const CommunicationDataType& message )
{
  int strSize = (message.size()<m_WriteBufferSize) ? message.size() : m_WriteBufferSize;
  memcpy(m_OutputBuffer, message.c_str(), sizeof(char)*strSize);
//  m_OutputBuffer[strSize+2] = '\r';
//  m_OutputBuffer[strSize+2] = '\0';
  std::cout << "Message length = " << strSize << ", Message = " << m_OutputBuffer << std::endl;
  this->m_StateMachine.ProcessInput( m_SendString );
}

void SerialCommunication::ReceiveString( void )
{
  this->m_StateMachine.ProcessInput( m_ReceiveString );
}

/*
void SerialCommunication::OpenCommunicationPortProcessing( void )
{
  char portName[10];
  sprintf(portName, "COM%d", this->m_PortNumber );

  this->m_PortHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                                  0,   // opened with exclusive-access 
                                  0,    // no security attributes
                                  OPEN_EXISTING, 
                                  FILE_FLAG_OVERLAPPED, // For asynchronous write operations 
                                  NULL  
                                  );

  if( this->m_PortHandle == INVALID_HANDLE_VALUE)
  {
    this->InvokeEvent( OpenPortFailureEvent() );
  }
  else
  {
    igstkLogMacro( igstk::Logger::DEBUG, "COM port name: ");
    igstkLogMacro( igstk::Logger::DEBUG, portName);
    igstkLogMacro( igstk::Logger::DEBUG, " opened.\n");
  }
}


void SerialCommunication::SetDataBufferSizeProcessing( void )
{
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string
  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL) || 
      !SetupComm(this->m_PortHandle, this->m_ReadBufferSize, this->m_WriteBufferSize)) 
  {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
  {
    //Clear out buffers
    PurgeComm(this->m_PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( igstk::Logger::DEBUG, "SetDataBufferSizeParameters with Read Buffer size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_ReadBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " and Write Buffer Size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_WriteBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " succeeded.\n");
  }
}


void SerialCommunication::SetCommunicationTimeoutProcessing( void )
{
  COMMTIMEOUTS       CommunicationTimeouts;
  CommunicationTimeouts.ReadIntervalTimeout = MAXWORD; //m_ReadIntervalTimeout
  CommunicationTimeouts.ReadTotalTimeoutConstant = m_ReadTotalTimeoutConstant;
  CommunicationTimeouts.ReadTotalTimeoutMultiplier = m_ReadTotalTimeoutMultiplier;
  CommunicationTimeouts.WriteTotalTimeoutConstant = m_WriteTotalTimeoutConstant;
  CommunicationTimeouts.WriteTotalTimeoutMultiplier = m_WriteTotalTimeoutMultiplier;

  if (!SetCommTimeouts(this->m_PortHandle, &CommunicationTimeouts))
  {
    this->InvokeEvent( CommunicationTimeoutSetupFailureEvent() );
  }
  else
  {
    igstkLogMacro( igstk::Logger::DEBUG, "SetCommunicationTimeoutParameters succeeded.\n");
  }
}


void SerialCommunication::SetupCommunicationProcessing( void )
{
  // Control setting for a serial communications device
  DCB   dcb;
  if (!GetCommState(this->m_PortHandle, &dcb))
       return;

  // Baudrate parameter settings
  dcb.BaudRate = this->m_BaudRate;
  // Bytesize parameter settings
  switch(this->m_ByteSize)
  {
  case SEVEN_BITS: dcb.ByteSize = 7; break;
  case EIGHT_BITS: dcb.ByteSize = 8; break;
  default: ;//return error; shouldn't come here in the first place.
  }
  // Parity parameter settings
  dcb.fParity = (this->m_Parity==NO_PARITY) ? 0 : 1;
  switch(this->m_Parity)
  {
  case NO_PARITY: dcb.Parity = NOPARITY; break;
  case ODD_PARITY: dcb.Parity = ODDPARITY; break; 
  case EVEN_PARITY: dcb.Parity = EVENPARITY; break;  
  default: ;//return error; shouldn't come here in the first place.
  }
  // Stop bit parameter settings
  switch(this->m_StopBits)
  {
  case ONE_STOP_BIT: dcb.StopBits = ONESTOPBIT; break;
  case TWO_STOP_BITS: dcb.StopBits = TWOSTOPBITS; break;
  default: ;//return error; shouldn't come here in the first place.
  }
  //Hardware Handsake
  dcb.fOutxCtsFlow = m_HardwareHandshake ? 1 : 0;
  dcb.fRtsControl =  m_HardwareHandshake ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE;

  //Set up communication state using Windows API
  if (!SetCommState(this->m_PortHandle, &dcb))
  {
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
  }
  else
  {
    igstkLogMacro( igstk::Logger::DEBUG, "SetupCommunicationParameters succeeded.\n");
  }
}

void SerialCommunication::ClearBuffersAndCloseCommunicationPortProcessing( void )
{
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  this->CloseCommunicationPortProcessing();
}

void SerialCommunication::CloseCommunicationPortProcessing( void )
{
  CloseHandle(this->m_PortHandle);
  this->m_PortHandle = (HandleType)INVALID_HANDLE_VALUE;
  igstkLogMacro( igstk::Logger::DEBUG, "Communication port closed.\n");
}

void SerialCommunication::RestCommunicationProcessing( void )
{
  if (!SetCommBreak( this->m_PortHandle ))
  {
    this->InvokeEvent( RestCommunicationFailureEvent() );
  }

  Sleep( m_PortRestSpan );

  if (!ClearCommBreak( this->m_PortHandle ))
  {
    this->InvokeEvent( RestCommunicationFailureEvent() );
  }
}


void SerialCommunication::FlushOutputBufferProcessing( void )
{
  if (!FlushFileBuffers( this->m_PortHandle ))
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }
}


void SerialCommunication::SendStringProcessing( void )
{
  //OVERLAPPED structure contains information used in asynchronous input and output (I/O).
  OVERLAPPED    overlappedWrite = {0};

  //create an overlapped event for asynchronous write
  overlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if( overlappedWrite.hEvent == NULL )
  {
    this->InvokeEvent( OverlappedEventCreationFailureEvent() );
    return;
  }

  //try writing to the hardware
  unsigned long   bytesToWrite = strlen(m_OutputBuffer);
  unsigned long   writtenBytes;

  bool successfulWrite = WriteFile(this->m_PortHandle, this->m_OutputBuffer, bytesToWrite, &writtenBytes, &overlappedWrite);
 
  //check if writing event successful
  if (successfulWrite) 
  { // successfully wrote all data
    if (writtenBytes==bytesToWrite)
    {
      std::cout << "Bytes to write = " << bytesToWrite << ", Written buyes = " << writtenBytes << std::endl;
      this->InvokeEvent( SendStringSuccessfulEvent());
    }
    //write timeout occurred, and all data could not get written
    else
    {
      this->InvokeEvent( SendStringWriteTimeoutEvent() );
    }
  }
  //else check if writing operation is pending,
  else if (GetLastError()==ERROR_IO_PENDING)
  { // if so, wait for write to succeed or timeout.
    DWORD waitResult = WaitForSingleObject(overlappedWrite.hEvent, INFINITE); 
    switch(waitResult)
    {
    case WAIT_OBJECT_0:
      //check if writing succeeded 
      if( GetOverlappedResult(this->m_PortHandle, &overlappedWrite, &writtenBytes, FALSE)
              && (writtenBytes==bytesToWrite))
      {
        std::cout << "Bytes to write = " << bytesToWrite << ", Written buyes = " << writtenBytes << std::endl;
        this->InvokeEvent( SendStringSuccessfulEvent() );
      }
      //writing timeout occurred before all data could be written.
      else
      {
        this->InvokeEvent( SendStringWriteTimeoutEvent() );
      } 
      break;
    case WAIT_TIMEOUT:
      //wait timeout occurred
      this->InvokeEvent( SendStringWaitTimeoutEvent() );
      break;
    default:
      this->InvokeEvent( SendStringFailureEvent() );
    } 
  }
  //writing operation failed.
  else
  {
      this->InvokeEvent( SendStringFailureEvent() );
  }

  //Close the created overlapped event.
  CloseHandle(overlappedWrite.hEvent);
}


void SerialCommunication::ReceiveStringProcessing( void )
{
  std::cout << "Came to ReceiveStringProcessing" << std::endl;

  //Get status of the communication device.
  DWORD      communicationErrors = 0;
  COMSTAT    communicationStatus = {0};
  if( !ClearCommError( this->m_PortHandle, &communicationErrors, &communicationStatus ))
  {
      this->InvokeEvent( CommunicationStatusReportFailureEvent() );
      return;
  }

  int bytesToRead = (communicationStatus.cbInQue < m_ReadBufferSize) ? communicationStatus.cbInQue : m_ReadBufferSize;

  std::cout << "Bytes to read = " << bytesToRead << std::endl;

  //OVERLAPPED structure contains information used in asynchronous input/output (I/O).
  OVERLAPPED    overlappedRead = {0};
  //create an overlapped event for asynchronous read
  overlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if( overlappedRead.hEvent == NULL )
  {
    this->InvokeEvent( OverlappedEventCreationFailureEvent() );
    return;
  }

  //try reading from the hardware
  unsigned long   readBytes;

  bool successfulRead = ReadFile(this->m_PortHandle, this->m_InputBuffer, \
    bytesToRead, &readBytes, &overlappedRead);

  //check if reading event successful
  if (successfulRead) 
  { // successfully read all data
    if (readBytes==bytesToRead)
    {
      this->InvokeEvent( ReceiveStringSuccessfulEvent());
      this->m_ReadDataSize = readBytes;
      this->m_ReadBufferOffset = 0;
   }
    //read timeout occurred, and all data could not get read
    else
    {
      this->InvokeEvent( ReceiveStringReadTimeoutEvent() );
    }
  }
  //else check if reading operation is pending,
  else if (GetLastError()==ERROR_IO_PENDING)
  { // if so, wait for read to succeed or timeout.
    DWORD waitResult = WaitForSingleObject(overlappedRead.hEvent, 1000); 
    switch(waitResult)
    {
    case WAIT_OBJECT_0:
      //check if reading succeeded 
      if( GetOverlappedResult(this->m_PortHandle, &overlappedRead, &readBytes, FALSE)
              && (readBytes==bytesToRead))
      {
        this->InvokeEvent( ReceiveStringSuccessfulEvent() );
      }
      //reading timeout occurred before all data could be read.
      else
      {
        this->InvokeEvent( ReceiveStringReadTimeoutEvent() );
      } 
      break;
    case WAIT_TIMEOUT:
      //wait timeout occurred
      this->InvokeEvent( ReceiveStringWaitTimeoutEvent() );
      break;
    default:
      this->InvokeEvent( ReceiveStringFailureEvent() );
    } 
  }
  //writing operation failed.
  else
  {
      this->InvokeEvent( ReceiveStringFailureEvent() );
  }

  std::cout << "String: " << this->m_InputBuffer << std::endl;
 
  //Close the created overlapped event.
  CloseHandle(overlappedRead.hEvent);
}
*/

void SerialCommunication::SetLogger( LoggerType* logger )
{
    m_pLogger = logger;
}


SerialCommunication::LoggerType* SerialCommunication::GetLogger(  void )
{
    return m_pLogger;
}

} // end namespace igstk

