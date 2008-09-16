/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForWindows.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/* standard include files */
#include <iostream>
#include <stdio.h>

#include "igstkConfigure.h"
#include "igstkSerialCommunicationForWindows.h"
#include "igstkPulseGenerator.h"


namespace igstk
{ 

SerialCommunicationForWindows::SerialCommunicationForWindows()
                                                   :m_StateMachine(this)
{
  m_PortHandle = INVALID_HANDLE_VALUE;
  m_OldTimeoutPeriod = 0;
} 


SerialCommunicationForWindows::~SerialCommunicationForWindows()
{
} 


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalOpenPort( void )
{
  unsigned int timeoutPeriod = this->GetTimeoutPeriod();

  COMMTIMEOUTS default_ctmo = 
    {
    MAXDWORD, MAXDWORD,
    timeoutPeriod, 
    2, 
    timeoutPeriod,
    };

  HANDLE portHandle;
  DCB commSettings;

  unsigned int portNumber = this->GetPortNumber();
  const char *device = "";  

  if (portNumber >= 0 && portNumber < 8)
    {
    const char *deviceNames[] = { IGSTK_SERIAL_PORT_0,
                                  IGSTK_SERIAL_PORT_1,
                                  IGSTK_SERIAL_PORT_2,
                                  IGSTK_SERIAL_PORT_3,
                                  IGSTK_SERIAL_PORT_4,
                                  IGSTK_SERIAL_PORT_5,
                                  IGSTK_SERIAL_PORT_6,
                                  IGSTK_SERIAL_PORT_7  };

    device = deviceNames[portNumber];
    }

  igstkLogMacro( DEBUG, "InternalOpenPort on " << device << "\n" );

  portHandle = CreateFile(device,
                          GENERIC_READ|GENERIC_WRITE,
                          0,  /* not allowed to share ports */
                          0,  /* child-processes don't inherit handle */
                          OPEN_EXISTING, 
                          FILE_ATTRIBUTE_NORMAL,
                          NULL); /* no template file */

  if (portHandle == INVALID_HANDLE_VALUE)
    {
    igstkLogMacro( WARNING, "InternalOpenPort: CreateFile() failed.\n" );
    return FAILURE;
    }

  /* save the port handle. */
  m_PortHandle = portHandle;

  if (SetupComm(portHandle, 1600, 1600) == FALSE)
    { /* set buffer size */
    CloseHandle(portHandle);
    m_PortHandle = INVALID_HANDLE_VALUE;
    igstkLogMacro( WARNING, "InternalOpenPort: SetupComm() failed.\n" );
    return FAILURE;
    }

  if (GetCommState(portHandle,&commSettings) == FALSE)
    {
    CloseHandle(portHandle);
    m_PortHandle = INVALID_HANDLE_VALUE;
    igstkLogMacro( WARNING, "InternalOpenPort: GetCommState() failed.\n" );
    return FAILURE;
    }

  commSettings.fOutX = FALSE;             /* no S/W handshake */
  commSettings.fInX = FALSE;
  commSettings.fAbortOnError = FALSE;     /* don't need to clear errors */
  commSettings.fOutxDsrFlow = FALSE;      /* no modem-style flow stuff*/
  commSettings.fDtrControl = DTR_CONTROL_ENABLE;  

  if (SetCommState(portHandle,&commSettings) == FALSE)
    {
    CloseHandle(portHandle);
    m_PortHandle = INVALID_HANDLE_VALUE;
    igstkLogMacro( WARNING, "InternalOpenPort: SetCommState() failed.\n" );
    return FAILURE;
    }
  
  if (SetCommTimeouts(portHandle,&default_ctmo) == FALSE)
    {
    CloseHandle(portHandle);
    m_PortHandle = INVALID_HANDLE_VALUE;
    igstkLogMacro( WARNING, "InternalOpenPort: SetCommTimeouts() failed.\n" );
    return FAILURE;
    }

  m_OldTimeoutPeriod = timeoutPeriod;

  igstkLogMacro( DEBUG, "InternalOpenPort succeeded...\n" );

  return SUCCESS;
}


/** Set the RTS value 
 *  0 : Clear the RTS (request-to-send) signal 
    1 : Sends the RTS signal */
SerialCommunicationForWindows::ResultType 
SerialCommunicationForWindows::InternalSetRTS(unsigned int signal)
{
  if(signal == 1)
    {
    if (EscapeCommFunction(m_PortHandle,SETRTS) == FALSE)
      {
      igstkLogMacro( WARNING, "SetRTS failed.\n" );
      return FAILURE;
      }
    }
  else
    {
    if (EscapeCommFunction(m_PortHandle,CLRRTS) == FALSE)
      {
      igstkLogMacro( WARNING, "SetRTS failed.\n" );
      return FAILURE;
      }
    }

  igstkLogMacro( DEBUG, "SetRTS succeeded...\n" );
  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalUpdateParameters( void )
{
  DCB commSettings;
  COMMTIMEOUTS ctmo;
  
  unsigned int timeoutPeriod = this->GetTimeoutPeriod();
  int newbaud = CBR_9600;
  unsigned int baud = this->GetBaudRate();
  DataBitsType dataBits = this->GetDataBits();
  ParityType parity = this->GetParity();
  StopBitsType stopBits = this->GetStopBits();
  HandshakeType handshake = this->GetHardwareHandshake();

  switch (baud)
    {
    case 9600:   newbaud = CBR_9600;   break;
    case 19200:  newbaud = CBR_19200;  break;
    case 38400:  newbaud = CBR_38400;  break;
    case 57600:  newbaud = CBR_57600;  break;
    case 115200: newbaud = CBR_115200; break;
    }

  GetCommState(m_PortHandle,&commSettings);
  GetCommTimeouts(m_PortHandle,&ctmo);

  commSettings.BaudRate = newbaud;     // speed

  // set handshaking
  if (handshake == HandshakeOn)
    {
    commSettings.fOutxCtsFlow = TRUE;       // on
    commSettings.fRtsControl = RTS_CONTROL_HANDSHAKE;
    }
  else
    {
    commSettings.fOutxCtsFlow = FALSE;       // off
    commSettings.fRtsControl = RTS_CONTROL_DISABLE;
    }

  // set data bits
  if (dataBits == DataBits8)
    {
    commSettings.ByteSize = 8;
    }
  else if (dataBits == DataBits7)
    {
    commSettings.ByteSize = 7;
    }

  // set parity
  if (parity == NoParity)
    { // none
    commSettings.Parity = NOPARITY;
    }
  else if (parity == OddParity)
    { // odd
    commSettings.Parity = ODDPARITY;
    }
  else if (parity == EvenParity)
    { // even
    commSettings.Parity = EVENPARITY;
    }

  // set stop bits
  if (stopBits == StopBits1)
    {
    commSettings.StopBits = ONESTOPBIT;
    }
  else if (stopBits == StopBits2)
    {
    commSettings.StopBits = TWOSTOPBITS;
    }

  // set timeout
  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ctmo.ReadTotalTimeoutConstant = timeoutPeriod;
  ctmo.WriteTotalTimeoutConstant = timeoutPeriod;

  if (SetCommState(m_PortHandle,&commSettings) == FALSE ||
      SetCommTimeouts(m_PortHandle,&ctmo) == FALSE)
    {
    igstkLogMacro( WARNING, "SetCommunicationParameters: SetCommState() "
                   "or SetCommTimeouts() failed\n" );
    return FAILURE;
    }

  m_OldTimeoutPeriod = timeoutPeriod;

  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n" );

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalClosePort( void )
{
  CloseHandle(m_PortHandle);
  m_PortHandle = INVALID_HANDLE_VALUE;

  return SUCCESS;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSendBreak( void )
{
  DWORD dumb;
  ResultType result = FAILURE;

  ClearCommError(m_PortHandle,&dumb,NULL);  // clear error

  if (SetCommBreak(m_PortHandle))
    {
    PulseGenerator::Sleep(300);   // hold break for 0.3 seconds

    if (ClearCommBreak(m_PortHandle))
      {
      result = SUCCESS;
      }
    }

  return result;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalPurgeBuffers( void )
{
  DWORD dumb;
  DWORD flushtype = PURGE_TXCLEAR | PURGE_RXCLEAR;
  ResultType result = FAILURE;

  ClearCommError(m_PortHandle,&dumb,NULL);       // clear error
  if (PurgeComm(m_PortHandle,flushtype))             // clear buffers
    {
    result = SUCCESS;
    }

  return result;
}


void SerialCommunicationForWindows::InternalSleep( int milliseconds )
{
  PulseGenerator::Sleep(milliseconds);
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalSetTimeout( unsigned int timeoutPeriod )
{
  ResultType result = SUCCESS;

  if (timeoutPeriod != m_OldTimeoutPeriod)
    {
    COMMTIMEOUTS ctmo;
    GetCommTimeouts(m_PortHandle,&ctmo);
    // set timeout
    ctmo.ReadIntervalTimeout = MAXDWORD;
    ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
    ctmo.ReadTotalTimeoutConstant = timeoutPeriod;
    ctmo.WriteTotalTimeoutConstant = timeoutPeriod;
    result = FAILURE;
    if (SetCommTimeouts(m_PortHandle,&ctmo) != FALSE)
      {
      result = SUCCESS;
      m_OldTimeoutPeriod = timeoutPeriod;
      }
    }

  return result;
}
  

SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalWrite( const char *data,
                                              unsigned int n )
{
  DWORD m, dumb;
  unsigned int i = 0;
  ResultType writeError = SUCCESS;
  
  igstkLogMacro( DEBUG, "InternalWrite called ...\n" );

  writeError = this->InternalSetTimeout(this->GetTimeoutPeriod());

  if (writeError == SUCCESS)
    {
    while (n > 0)
      {
      if (WriteFile(m_PortHandle, &data[i],
                    n, &m, NULL) == FALSE)
        {
        if (GetLastError() == ERROR_OPERATION_ABORTED) 
          { // system cancelled us so clear error and retry
          ClearCommError(m_PortHandle,&dumb,NULL);
          }
        else
          {  // IO error occurred 
          writeError = FAILURE;
          break;
          }
        }
      else if (m == 0)
        { // no characters written, must have timed out
        writeError = TIMEOUT;
        break;
        }

      n -= m;  // n is number of chars left to write
      i += m;  // i is the number of chars written
      }
    }

  return writeError;
}


SerialCommunicationForWindows::ResultType
SerialCommunicationForWindows::InternalRead( char *data,
                                             unsigned int n,
                                             unsigned int &bytesRead )
{
  unsigned int i = 0;
  DWORD m,dumb;
  ResultType readError = SUCCESS;
  char terminationCharacter = this->GetReadTerminationCharacter();
  bool useTerminationCharacter = this->GetUseReadTerminationCharacter();

  readError = this->InternalSetTimeout(this->GetTimeoutPeriod());

  if (readError == SUCCESS)
    {
    while (n > 0)
      {
      if (ReadFile(m_PortHandle, &data[i],
                   1, &m, NULL) == FALSE)
        { 
        if (GetLastError() == ERROR_OPERATION_ABORTED)
          { // cancelled 
          ClearCommError(m_PortHandle,&dumb,NULL); // clear error and retry
          }
        else
          { // IO error occurred
          readError = FAILURE;
          break;
          }
        }
      else if (m == 0)
        { // no characters read, must have timed out
        readError = TIMEOUT;
        break;
        }
      n -= m;  // n is number of chars left to read
      i += m;  // i is the number of chars read

      // done when ReadTerminationCharacter received
      if ( useTerminationCharacter &&
           data[i-1] == terminationCharacter )
        {  
        break;
        }
      }
    }

  bytesRead = i;
  data[i] = '\0';

  return readError;
}


void SerialCommunicationForWindows::PrintSelf( std::ostream& os,
                                               itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortHandle: " << m_PortHandle << std::endl;
  os << indent << "OldTimeoutPeriod: " << m_OldTimeoutPeriod << std::endl;
}

} // end namespace igstk
