/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSerialCommunicationForLinux.cxx
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

#include "igstkSerialCommunicationForLinux.h"


namespace igstk
{ 
/** Constructor */
SerialCommunicationForLinux::SerialCommunicationForLinux() :  
SerialCommunication() , INVALID_HANDLE_VALUE(-1)
{
  this->m_PortHandle = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
} 

void SerialCommunicationForLinux::OpenCommunicationPortProcessing( void )
{
  char portName[20];
  sprintf(portName, "/dev/ttyS%.1d", this->m_PortNumber );
  std::cout << portName << std::endl;

  if (this->m_PortHandle != SerialCommunicationForLinux::INVALID_HANDLE_VALUE)
  {
    igstkLogMacro( igstk::Logger::DEBUG, "**** COM port already in use. Exiting .... ****\n");
    return;
  }

  this->m_PortHandle = open(portName, O_RDWR );

  if( this->m_PortHandle < 0)
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


void SerialCommunicationForLinux::SetDataBufferSizeProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "In SetDataBufferSizeProcessing ...\n ");
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string
/*
  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL) || 
      !SetupComm(this->m_PortHandle, this->m_ReadBufferSize, this->m_WriteBufferSize)) 
  {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
*/
  {
    //Clear out buffers
//    PurgeComm(this->m_PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
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


void SerialCommunicationForLinux::SetCommunicationTimeoutProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "In SetCommunicationTimeoutProcessing ...\n ");
  /*
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
  */
}


void SerialCommunicationForLinux::SetupCommunicationProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "In SetupCommunicationProcessing ...\n ");
  /*
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
  */
}

void SerialCommunicationForLinux::ClearBuffersAndCloseCommunicationPortProcessing( void )
{
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  this->CloseCommunicationPortProcessing();
}

void SerialCommunicationForLinux::CloseCommunicationPortProcessing( void )
{
  igstkLogMacro( igstk::Logger::DEBUG, "Attempt to close communication port .... \n");
  close(this->m_PortHandle);
  this->m_PortHandle = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
  igstkLogMacro( igstk::Logger::DEBUG, "Communication port closed.\n");
}

void SerialCommunicationForLinux::RestCommunicationProcessing( void )
{
  /*
  if (!SetCommBreak( this->m_PortHandle ))
  {
    this->InvokeEvent( RestCommunicationFailureEvent() );
  }

  Sleep( m_PortRestSpan );

  if (!ClearCommBreak( this->m_PortHandle ))
  {
    this->InvokeEvent( RestCommunicationFailureEvent() );
  }
  */
}


void SerialCommunicationForLinux::FlushOutputBufferProcessing( void )
{
  /*
  if (!FlushFileBuffers( this->m_PortHandle ))
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }
  */
}


void SerialCommunicationForLinux::SendStringProcessing( void )
{
  /*
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
  */
}


void SerialCommunicationForLinux::ReceiveStringProcessing( void )
{
  /*
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

  */
}

} // end namespace igstk

