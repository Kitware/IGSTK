/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForWindows.cxx
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

#include "igstkSerialCommunicationForWindows.h"


namespace igstk
{ 
/** Constructor */
SerialCommunicationForWindows::SerialCommunicationForWindows() :  
SerialCommunication()
{
} 

void SerialCommunicationForWindows::OpenPortProcessing( void )
{
  char portName[10];

  switch(this->m_PortNumber.Get())
  {
  case 0: sprintf(portName, "COM1" ); break;
  case 1: sprintf(portName, "COM2" ); break;
  case 2: sprintf(portName, "COM3" ); break;
  case 3: sprintf(portName, "COM4" ); break;
  }

  this->m_PortHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                                  0,   // opened with exclusive-access 
                                  0,    // no security attributes
                                  OPEN_EXISTING, 
                                  FILE_FLAG_OVERLAPPED, // For asynchronous write operations 
                                  NULL  
                                  );

  if( this->m_PortHandle == INVALID_HANDLE_VALUE)
  {
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
  }
  else
  {
    m_pOpenPortResultInput = &m_OpenPortSuccessInput;
    igstkLogMacro( DEBUG, "COM port name: " << portName << " opened\n" );
  }
}


void SerialCommunicationForWindows::SetUpDataBuffersProcessing( void )
{
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string
  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL) || 
      !SetupComm(this->m_PortHandle, this->m_ReadBufferSize, this->m_WriteBufferSize)) 
  {
    m_pDataBuffersSetUpResultInput = &m_DataBuffersSetUpFailureInput;
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
  {
    m_pDataBuffersSetUpResultInput = &m_DataBuffersSetUpSuccessInput;
    //Clear out buffers
    PurgeComm(this->m_PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( DEBUG, "SetDataBufferSizeParameters with Read Buffer size = " << m_ReadBufferSize << " and Write Buffer Size = " << m_WriteBufferSize << " succeeded.\n");
  }
}


void SerialCommunicationForWindows::SetUpDataTransferParametersProcessing( void )
{
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpFailureInput;

  // Control setting for a serial communications device
  DCB   dcb;
  if (!GetCommState(this->m_PortHandle, &dcb))
  {
       return;
  }

  // Baudrate parameter settings
  dcb.BaudRate = this->m_BaudRate.Get();
  // Bytesize parameter settings
  switch(this->m_ByteSize.Get())
  {
  case 7: dcb.ByteSize = 7; break;
  case 8: dcb.ByteSize = 8; break;
  default: ;//return error; shouldn't come here in the first place.
  }
  // Parity parameter settings
  dcb.fParity = (this->m_Parity==0) ? 0 : 1;
  switch(this->m_Parity.Get())
  {
  case 0: dcb.Parity = NOPARITY; break;
  case 1: dcb.Parity = ODDPARITY; break; 
  case 2: dcb.Parity = EVENPARITY; break;  
  default: ;//return error; shouldn't come here in the first place.
  }
  // Stop bit parameter settings
  switch(this->m_StopBits.Get())
  {
  case 1: dcb.StopBits = ONESTOPBIT; break;
  case 2: dcb.StopBits = TWOSTOPBITS; break;
  default: ;//return error; shouldn't come here in the first place.
  }
  //Hardware Handsake
  dcb.fOutxCtsFlow = m_HardwareHandshake.Get() ? 1 : 0;
  dcb.fRtsControl =  m_HardwareHandshake.Get() ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE;

  //Set up communication state using Windows API
  if (!SetCommState(this->m_PortHandle, &dcb))
  {
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
    return;
  }

  // Set up communication timeouts

  COMMTIMEOUTS       CommunicationTimeouts;
  CommunicationTimeouts.ReadIntervalTimeout = MAXWORD; //m_ReadIntervalTimeout
  CommunicationTimeouts.ReadTotalTimeoutConstant = m_ReadTotalTimeoutConstant;
  CommunicationTimeouts.ReadTotalTimeoutMultiplier = m_ReadTotalTimeoutMultiplier;
  CommunicationTimeouts.WriteTotalTimeoutConstant = m_WriteTotalTimeoutConstant;
  CommunicationTimeouts.WriteTotalTimeoutMultiplier = m_WriteTotalTimeoutMultiplier;

  if (!SetCommTimeouts(this->m_PortHandle, &CommunicationTimeouts))
  {
    this->InvokeEvent( CommunicationTimeoutSetupFailureEvent() );
    return;
  }
  else
  {
    igstkLogMacro( DEBUG, "SetupCommunicationParameters succeeded.\n");
  }
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpSuccessInput;
}

void SerialCommunicationForWindows::ClearBuffersAndClosePortProcessing( void )
{
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  this->ClosePortProcessing();
}

void SerialCommunicationForWindows::ClosePortProcessing( void )
{
  CloseHandle(this->m_PortHandle);
  this->m_PortHandle = (HandleType)INVALID_HANDLE_VALUE;
  igstkLogMacro( DEBUG, "Communication port closed.\n");
}

void SerialCommunicationForWindows::RestPortProcessing( void )
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


void SerialCommunicationForWindows::FlushOutputBufferProcessing( void )
{
  if (!FlushFileBuffers( this->m_PortHandle ))
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }
}


void SerialCommunicationForWindows::SendStringProcessing( void )
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
      std::cout << "Bytes to write = " << bytesToWrite << ", Written bytes = " << writtenBytes << std::endl;
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


void SerialCommunicationForWindows::ReceiveStringProcessing( void )
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

  int bytesToRead = communicationStatus.cbInQue;
  if (bytesToRead == 0)
    {
    bytesToRead = 1;
    }
  if (bytesToRead > m_ReadBufferSize)
    {
    bytesToRead = m_ReadBufferSize;
    }

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
  unsigned long   readBytes = 0;

  bool successfulRead = ReadFile(this->m_PortHandle, this->m_InputBuffer, \
    bytesToRead, &readBytes, &overlappedRead);
  this->m_InputBuffer[readBytes] = 0; // terminate the string

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
      successfulRead = GetOverlappedResult(this->m_PortHandle, &overlappedRead, &readBytes, FALSE);
      this->m_InputBuffer[readBytes] = 0; // terminate the string

      if (successfulRead && (readBytes==bytesToRead))
      {
        this->InvokeEvent( ReceiveStringSuccessfulEvent() );
        this->m_ReadDataSize = readBytes;
        this->m_ReadBufferOffset = 0;
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

  igstkLogMacro( DEBUG, "String: " << this->m_InputBuffer << std::endl );
 
  //Close the created overlapped event.
  CloseHandle(overlappedRead.hEvent);
}


/** Print Self function */
void SerialCommunicationForWindows::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Port Handle: " << m_PortHandle << std::endl;
}

} // end namespace igstk

