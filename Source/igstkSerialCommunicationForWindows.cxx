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

SerialCommunicationForWindows::SerialCommunicationForWindows()
{
  m_PortHandle = INVALID_HANDLE_VALUE;
} 


SerialCommunicationForWindows::~SerialCommunicationForWindows()
{
} 


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalOpenPort( void )
{
  COMMTIMEOUTS default_ctmo = {
    MAXDWORD, MAXDWORD,
    m_TimeoutPeriod, 
    2, 
    m_TimeoutPeriod
  };
  HANDLE serial_port;
  DCB comm_settings;

  char device[20];
  sprintf(device, "COM%.1d:", this->GetPortNumber()+1 );

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
  if (m_PortHandle == serial_port ||
      m_PortHandle == INVALID_HANDLE_VALUE)
    {
    m_PortHandle = serial_port;
    GetCommTimeouts(serial_port,&m_SaveTimeout);
    GetCommState(serial_port,&m_SaveDCB);
    }

  if (SetupComm(serial_port, 1600, 1600)
      == FALSE)
    { /* set buffer size */
    CloseHandle(serial_port);
    m_PortHandle = INVALID_HANDLE_VALUE;
    return FAILURE;
    }

  if (GetCommState(serial_port,&comm_settings) == FALSE)
    {
    CloseHandle(serial_port);
    m_PortHandle = INVALID_HANDLE_VALUE;
    return FAILURE;
    }

  comm_settings.fOutX = FALSE;             /* no S/W handshake */
  comm_settings.fInX = FALSE;
  comm_settings.fAbortOnError = FALSE;     /* don't need to clear errors */
  comm_settings.fOutxDsrFlow = FALSE;      /* no modem-style flow stuff*/
  comm_settings.fDtrControl = DTR_CONTROL_ENABLE;  

  if (SetCommState(serial_port,&comm_settings) == FALSE)
    {
    CloseHandle(serial_port);
    m_PortHandle = INVALID_HANDLE_VALUE;
    return FAILURE;
    }
  
  if (SetCommTimeouts(serial_port,&default_ctmo) == FALSE)
    {
    SetCommState(serial_port,&comm_settings);
    SetCommState(serial_port,&m_SaveDCB); 
    CloseHandle(serial_port);
    m_PortHandle = INVALID_HANDLE_VALUE;
    return FAILURE;
    }

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSetTransferParameters( void )
{
  DCB comm_settings;
  COMMTIMEOUTS ctmo;
  
  int newbaud = CBR_9600;
  unsigned int baud = m_BaudRate;

  switch (baud)
    {
    case 9600:   newbaud = CBR_9600;   break;
    case 19200:  newbaud = CBR_19200;  break;
    case 38400:  newbaud = CBR_38400;  break;
    case 57600:  newbaud = CBR_57600;  break;
    case 115200: newbaud = CBR_115200; break;
    }

  GetCommState(m_PortHandle,&comm_settings);
  GetCommTimeouts(m_PortHandle,&ctmo);

  comm_settings.BaudRate = newbaud;     // speed

  // set handshaking
  if (m_HardwareHandshake == HandshakeOn)
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
  if (m_DataBits == DataBits8)
    {
    comm_settings.ByteSize = 8;
    }
  else if (m_DataBits == DataBits7)
    {
    comm_settings.ByteSize = 7;
    }

  // set parity
  if (m_Parity == NoParity)
    { // none                
    comm_settings.Parity = NOPARITY;
    }
  else if (m_Parity == OddParity)
    { // odd
    comm_settings.Parity = ODDPARITY;
    }
  else if (m_Parity == EvenParity)
    { // even
    comm_settings.Parity = EVENPARITY;
    }

  // set stop bits
  if (m_StopBits == StopBits1)
    {
    comm_settings.StopBits = ONESTOPBIT;
    }
  else if (m_StopBits == StopBits2)
    {
    comm_settings.StopBits = TWOSTOPBITS;
    }

  // set timeout
  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ctmo.ReadTotalTimeoutConstant = m_TimeoutPeriod;
  ctmo.WriteTotalTimeoutConstant = m_TimeoutPeriod;

  if (SetCommState(m_PortHandle,&comm_settings) == FALSE ||
      SetCommTimeouts(m_PortHandle,&ctmo) == FALSE)
    {
    return FAILURE;
    }

  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n");

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalClosePort( void )
{
  // restore the comm port state to from before it was opened
  SetCommTimeouts(m_PortHandle,&m_SaveTimeout);
  SetCommState(m_PortHandle,&m_SaveDCB);

  CloseHandle(m_PortHandle);
  m_PortHandle = INVALID_HANDLE_VALUE;

  return SUCCESS;
}


void SerialCommunicationForWindows::InternalSendBreak( void )
{
  DWORD dumb;

  ClearCommError(m_PortHandle,&dumb,NULL);       // clear error

  SetCommBreak(m_PortHandle);
  ::Sleep(300);                            // hold break for 0.3 seconds
  ClearCommBreak(m_PortHandle);
}


void SerialCommunicationForWindows::InternalPurgeBuffers( void )
{
  DWORD dumb;
  DWORD flushtype = PURGE_TXCLEAR | PURGE_RXCLEAR;

  ClearCommError(m_PortHandle,&dumb,NULL);       // clear error
  PurgeComm(m_PortHandle,flushtype);             // clear buffers
}


void SerialCommunicationForWindows::InternalSleep( void )
{
  // use Windows sleep function
  ::Sleep(m_SleepPeriod);
}

void SerialCommunicationForWindows::InternalWrite( void )
{
  DWORD m, dumb;
  int i = 0;
  int bytesToWrite = m_BytesToWrite;
  int writeError = 0;

  igstkLogMacro( DEBUG, "InternalWrite called ...\n");
  while (bytesToWrite > 0)
    {
    if (WriteFile(m_PortHandle, &m_OutputData[i],
                  bytesToWrite, &m, NULL) == FALSE)
      {
      if (GetLastError() == ERROR_OPERATION_ABORTED) 
        { // system cancelled us so clear error and retry
        ClearCommError(m_PortHandle,&dumb,NULL);
        }
      else
        {  // IO error occurred 
        writeError = 1;
        break;
        }
      }
    else if (m == 0)
      { // no characters written, must have timed out
      writeError = 2;
      break;
      }

    bytesToWrite -= m;  // bytesToWrite is number of chars left to write
    i += m;  // i is the number of chars written
    }

  if (writeError == 1)
    {
    this->InvokeEvent( WriteFailureEvent() );
    }
  else if (writeError == 2)
    {
    this->InvokeEvent( WriteTimeoutEvent() );
    }
  else
    {
    this->InvokeEvent( WriteSuccessEvent() );
    }
}


void SerialCommunicationForWindows::InternalRead( void )
{
  int i = 0;
  DWORD m,dumb;
  int n = m_BytesToRead;
  int readError = 0;
  
  while (n > 0)
    {
    if (ReadFile(m_PortHandle, &m_InputData[i],
                 1, &m, NULL) == FALSE)
      { 
      if (GetLastError() == ERROR_OPERATION_ABORTED)
        { // cancelled 
        ClearCommError(m_PortHandle,&dumb,NULL); // clear error and retry
        }
      else
        { // IO error occurred
        readError = 1;
        break;
        }
      }
    else if (m == 0)
      { // no characters read, must have timed out
      readError = 2;
      break;
      }
    n -= m;  // n is number of chars left to read
    i += m;  // i is the number of chars read

    // done when ReadTerminationCharacter received
    if ( m_UseReadTerminationCharacter &&
         m_InputData[i-1] == m_ReadTerminationCharacter )
      {  
      break;
      }
    }

  m_BytesRead = i;
  m_InputData[i] = '\0';

  if (readError == 1)
    {
    this->InvokeEvent( ReadFailureEvent() );
    }
  else if (readError == 2)
    {
    this->InvokeEvent( ReadTimeoutEvent() );
    }
  else
    {
    this->InvokeEvent( ReadSuccessEvent());
    }
}


void SerialCommunicationForWindows::PrintSelf( std::ostream& os,
                                               itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

