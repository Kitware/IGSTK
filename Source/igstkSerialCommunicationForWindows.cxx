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
  SerialCommunication(), TIMEOUT_PERIOD(5000),
  NDI_INVALID_HANDLE(INVALID_HANDLE_VALUE)
{
  this->m_PortHandle = SerialCommunicationForWindows::NDI_INVALID_HANDLE;
} 

SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalOpenCommunication( void )
{
  static COMMTIMEOUTS default_ctmo = {
    MAXDWORD, MAXDWORD,
    TIMEOUT_PERIOD, 
    2, 
    TIMEOUT_PERIOD
  };
  HANDLE serial_port;
  DCB comm_settings;

  char device[20];
  sprintf(device, "COM%.1d:", this->GetPortNumber().Get()+1 );
  std::cout << device << std::endl;

  serial_port = CreateFile(device,
                           GENERIC_READ|GENERIC_WRITE,
                           0,  /* not allowed to share ports */
                           0,  /* child-processes don't inherit handle */
                           OPEN_EXISTING, 
                           FILE_ATTRIBUTE_NORMAL,
                           NULL); /* no template file */

  if (serial_port == INVALID_HANDLE_VALUE)
    {
    return FAILURE;
    }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  if (this->m_PortHandle == serial_port ||
      this->m_PortHandle == INVALID_HANDLE_VALUE)
    {
    this->m_PortHandle = serial_port;
    GetCommTimeouts(serial_port,&m_SaveTimeout);
    GetCommState(serial_port,&m_SaveDCB);
    }

  if (SetupComm(serial_port, this->m_ReadBufferSize, this->m_WriteBufferSize)
      == FALSE)
    { /* set buffer size */
    this->m_PortHandle = INVALID_HANDLE_VALUE;
    CloseHandle(serial_port);
    return FAILURE;
    }

  if (GetCommState(serial_port,&comm_settings) == FALSE)
    {
    this->m_PortHandle = INVALID_HANDLE_VALUE;
    CloseHandle(serial_port);
    return FAILURE;
    }

  comm_settings.fOutX = FALSE;             /* no S/W handshake */
  comm_settings.fInX = FALSE;
  comm_settings.fAbortOnError = FALSE;     /* don't need to clear errors */
  comm_settings.fOutxDsrFlow = FALSE;      /* no modem-style flow stuff*/
  comm_settings.fDtrControl = DTR_CONTROL_ENABLE;  

  if (SetCommState(serial_port,&comm_settings) == FALSE)
    {
    this->m_PortHandle = INVALID_HANDLE_VALUE;
    CloseHandle(serial_port);
    return FAILURE;
    }
  
  if (SetCommTimeouts(serial_port,&default_ctmo) == FALSE)
    {
    SetCommState(serial_port,&comm_settings);
    SetCommState(serial_port,&m_SaveDCB); 
    this->m_PortHandle = INVALID_HANDLE_VALUE;
    CloseHandle(serial_port);
    return FAILURE;
    }

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSetUpDataBuffers( void )
{
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  // one extra byte to store end of string
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ];
  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL) )
    {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
    return FAILURE;
    }
  else
    {
    //Clear out buffers
    PurgeComm(this->m_PortHandle,
              PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( DEBUG, "SetDataBufferSizeParameters with Read Buffer size = " << m_ReadBufferSize << " and Write Buffer Size = " << m_WriteBufferSize << " succeeded.\n");
    return SUCCESS;
    }
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSetTransferParameters( void )
{
  DCB comm_settings;
  int newbaud;

  unsigned int baud = this->m_BaudRate.Get();

  switch (baud)
    {
    case 9600:   newbaud = CBR_9600;   break;
    case 14400:  newbaud = CBR_14400;  break;
    case 19200:  newbaud = CBR_19200;  break;
    case 38400:  newbaud = CBR_38400;  break;
    case 57600:  newbaud = CBR_57600;  break;
    case 115200: newbaud = CBR_115200; break;
    default:     
      this->InvokeEvent( SetCommunicationParametersFailureEvent() );
      igstkLogMacro( DEBUG, "Setting Communication Parameters Failed.(invalid baud rate)\n" << std::endl );
      return FAILURE;
    }

  GetCommState(this->m_PortHandle,&comm_settings);

  comm_settings.BaudRate = newbaud;     // speed

  // set handshaking
  if (this->m_HardwareHandshake.Get())
    {
    comm_settings.fOutxCtsFlow = TRUE;       // on
    comm_settings.fRtsControl = RTS_CONTROL_HANDSHAKE;
    }
  else
    {
    comm_settings.fOutxCtsFlow = FALSE;       // off
    comm_settings.fRtsControl = RTS_CONTROL_DISABLE;
    }    

  // set data bits
  if (this->m_ByteSize.Get() == 8)
    {
    comm_settings.ByteSize = 8;
    }
  else if (this->m_ByteSize.Get() == 7)
    {
    comm_settings.ByteSize = 7;
    }
  else
    {
    this->InvokeEvent( SetCommunicationParametersFailureEvent() );
    igstkLogMacro( DEBUG, "Setting Communication Parameters Failed.(invalid data bits)\n" << std::endl );
    return FAILURE;
    }

  // set parity
  if (this->m_Parity.Get() == 0)
    { // none                
    comm_settings.Parity = NOPARITY;
    }
  else if (this->m_Parity.Get() == 1)
    { // odd
    comm_settings.Parity = ODDPARITY;
    }
  else if (this->m_Parity.Get() == 2)
    { // even
    comm_settings.Parity = EVENPARITY;
    }
  else
    {
    this->InvokeEvent( SetCommunicationParametersFailureEvent() );
    igstkLogMacro( DEBUG, "Setting Communication Parameters Failed.(invalid parity)\n" << std::endl );
    return FAILURE;
    }

  // set stop bits
  if (this->m_StopBits.Get() == 1)
    {
    comm_settings.StopBits = ONESTOPBIT;
    }
  else if (this->m_StopBits.Get() == 2)
    {
    comm_settings.StopBits = TWOSTOPBITS;
    }
  else
    {
    this->InvokeEvent( SetCommunicationParametersFailureEvent() );
    igstkLogMacro( DEBUG, "Setting Communication Parameters Failed.(invalid stop bits)\n" << std::endl );
    return FAILURE;
    }

  SetCommState(this->m_PortHandle,&comm_settings);

  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n");

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalClearBuffersAndClosePort( void )
{
  if (m_InputBuffer!= NULL) 
    { // This check not required, still keeping for safety
    delete m_InputBuffer;
    }
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL)
    { // This check not required, still keeping for safety
    delete m_OutputBuffer;
    }
  m_OutputBuffer = NULL;

  return this->InternalClosePort();
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalClosePort( void )
{
  // restore the comm port state to from before it was opened
  if ( this->m_PortHandle != INVALID_HANDLE_VALUE )
    {
    SetCommTimeouts(this->m_PortHandle,&m_SaveTimeout);
    SetCommState(this->m_PortHandle,&m_SaveDCB);
    this->m_PortHandle = INVALID_HANDLE_VALUE;      
    }

  CloseHandle(this->m_PortHandle);

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSetTimeoutPeriod( int milliseconds )
{
  COMMTIMEOUTS ctmo;
  
  if (GetCommTimeouts(this->m_PortHandle,&ctmo) == FALSE)
    {
    return FAILURE;
    }
  
  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ctmo.ReadTotalTimeoutConstant = milliseconds;
  ctmo.WriteTotalTimeoutConstant = milliseconds;

  if (SetCommTimeouts(this->m_PortHandle,&ctmo) == FALSE)
    {
    return FAILURE;
    }

  return SUCCESS;
}


void SerialCommunicationForWindows::InternalSendBreak( void )
{
  DWORD dumb;

  ClearCommError(this->m_PortHandle,&dumb,NULL);       // clear error
  PurgeComm(this->m_PortHandle,PURGE_TXCLEAR|PURGE_RXCLEAR); // clear buffers

  SetCommBreak(this->m_PortHandle);
  Sleep(300);                            // hold break for 0.3 seconds
  ClearCommBreak(this->m_PortHandle);

  return;
}


void SerialCommunicationForWindows::InternalFlushOutputBuffer( void )
{
  DWORD dumb;
  DWORD flushtype = PURGE_TXCLEAR | PURGE_RXCLEAR;

  ClearCommError(this->m_PortHandle,&dumb,NULL);       // clear error
  PurgeComm(this->m_PortHandle,flushtype);             // clear buffers

  return;
}


void SerialCommunicationForWindows::InternalWrite( void )
{
  DWORD m, dumb;
  int i = 0;
  unsigned long  bytesToWrite = m_WriteNumberOfBytes;

  igstkLogMacro( DEBUG, "InternalWrite called ...\n");
  while (bytesToWrite > 0)
    {
    if (WriteFile(this->m_PortHandle, &this->m_OutputBuffer[i],
                  bytesToWrite, &m, NULL) == FALSE)
      {
      if (GetLastError() == ERROR_OPERATION_ABORTED) 
        { // system cancelled us so clear error and retry
        ClearCommError(this->m_PortHandle,&dumb,NULL);
        }
      else
        {
        this->InvokeEvent( WriteFailureEvent() );
        igstkLogMacro( DEBUG, "InternalWrite failed ...\n");
        return;  // IO error occurred 
        }
      }
    else if (m == 0)
      { // no characters written, must have timed out
      this->InvokeEvent( WriteTimeoutEvent() );
      igstkLogMacro( DEBUG, "InternalWrite failed with timeout...\n");
      return;
      }

    bytesToWrite -= m;  // bytesToWrite is number of chars left to write
    i += m;  // i is the number of chars written
    }

  igstkLogMacro( DEBUG, "Written bytes = " << i << std::endl);
  this->InvokeEvent( WriteSuccessfulEvent() );

  return;  // return the number of characters written
}


void SerialCommunicationForWindows::InternalRead( void )
{
  int i = 0;
  DWORD m,dumb;
  int n = this->m_ReadNumberOfBytes;
  
  while (n > 0)
    {
    if (ReadFile(this->m_PortHandle, &this->m_InputBuffer[i],
                 n, &m, NULL) == FALSE)
      { 
      if (GetLastError() == ERROR_OPERATION_ABORTED)
        { // cancelled 
        ClearCommError(this->m_PortHandle,&dumb,NULL); // clear error and retry
        }
      else
        {
        igstkLogMacro( DEBUG, "InternalRead failed ...\n");
        this->InvokeEvent( ReadFailureEvent() );
        return;  // IO error occurred
        }
      }
    else if (m == 0)
      { // no characters read, must have timed out
      igstkLogMacro( DEBUG, "InternalRead failed with timeout...\n");
      this->InvokeEvent( ReadTimeoutEvent() );
      return;
      }
    n -= m;  // n is number of chars left to read
    i += m;  // i is the number of chars read
    if ( this->m_UseReadTerminationCharacter )
      {
      if (this->m_InputBuffer[i-1] == this->m_ReadTerminationCharacter )
        {  // done when ReadTerminationCharacter received
        break;
        }
      }
    }

  this->m_ReadDataSize = i;
  this->m_ReadBufferOffset = 0;
  this->m_InputBuffer[i] = 0;
  igstkLogMacro( DEBUG, "Read number of bytes = " << i << ". String: " << this->m_InputBuffer << std::endl );
  this->InvokeEvent( ReadSuccessfulEvent());

  return;
}


/** Print Self function */
void SerialCommunicationForWindows::PrintSelf( std::ostream& os, 
                                               itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Port Handle: " << m_PortHandle << std::endl;
}

} // end namespace igstk

