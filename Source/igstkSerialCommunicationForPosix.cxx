/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForPosix.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/* =========== standard includes */
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "igstkSerialCommunicationForPosix.h"


namespace igstk
{ 

// value for invalid handle
const int INVALID_HANDLE = -1;


SerialCommunicationForPosix::SerialCommunicationForPosix()
{
  m_PortHandle = INVALID_HANDLE;
} 


SerialCommunicationForPosix::~SerialCommunicationForPosix()
{
} 


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalOpenPort( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::InternalOpenPort"
                 " called ...\n" );

  struct termios t;
  int i;

  char device[20];
  // it would be really nice to support the devices for other OS
  sprintf(device, "/dev/ttyS%.1d", this->GetPortNumber() );

  // port is readable/writable and is (for now) non-blocking
  m_PortHandle = open(device,O_RDWR|O_NOCTTY|O_NDELAY);

  if (m_PortHandle == INVALID_HANDLE)
    {
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;             // bail out on error
    }

  // restore blocking now that the port is open (we just didn't want
  // the port to block while we were trying to open it)
  fcntl(m_PortHandle, F_SETFL, 0);

  // get I/O information
  if (tcgetattr(m_PortHandle,&t) == -1)
    {
    close(m_PortHandle);
    m_PortHandle = INVALID_HANDLE;
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;
    }

  // save the serial port state so that it can be restored when
  //   the serial port is closed in ndiSerialClose()
  tcgetattr(m_PortHandle,&m_SaveTermIOs);

  // clear everything specific to terminals
  t.c_lflag = 0;
  t.c_iflag = 0;
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    // use constant, not interval timout
  t.c_cc[VTIME] = m_TimeoutPeriod/100;

  if (tcsetattr(m_PortHandle,TCSANOW,&t) == -1)
    { // set I/O information
    close(m_PortHandle);
    m_PortHandle = INVALID_HANDLE;
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;
    }

  tcflush(m_PortHandle,TCIOFLUSH); // flush the buffers for good luck

  igstkLogMacro( DEBUG, "COM port name: " << device << " opened.\n" );

  return SUCCESS;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalSetTransferParameters( void )
{
  struct termios t;
  int newbaud;

  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalSetTransferParameters called ...\n" );

  unsigned int baud = m_BaudRate;

#if defined(sgi) && defined(__NEW_MAX_BAUD)
  switch (baud)
    {
    case 9600:    newbaud = 9600;   break;
    case 19200:   newbaud = 19200;  break;
    case 38400:   newbaud = 38400;  break;
    case 57600:   newbaud = 57600;  break;
    case 115200:  newbaud = 115200; break;
    }
#else
  switch (baud)
    {
    case 9600:   newbaud = B9600;   break;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:  newbaud = B57600;  break;
    case 115200: newbaud = B115200; break;
    }
#endif

  tcgetattr(m_PortHandle,&t);          // get I/O information
  t.c_cflag &= ~CSIZE;                // clear flags

  // set baud rate
#if defined(sgi) && defined(__NEW_MAX_BAUD)
  t.c_ospeed = newbaud;
#elif defined(__APPLE__)
  cfsetispeed(&t, newbaud);
  cfsetospeed(&t, newbaud);
#else
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                // set baud rate
#endif

  // set data bits
  if (m_DataBits == DataBits8)
    {                 
    t.c_cflag |= CS8; 
    }
  else if (m_DataBits == DataBits7)
    {
    t.c_cflag |= CS7;
    }

  // set parity
  if (m_Parity == NoParity)
    { // none            
    t.c_cflag &= ~PARENB;
    t.c_cflag &= ~PARODD;
    }
  else if (m_Parity == OddParity)
    { // odd
    t.c_cflag |= PARENB;
    t.c_cflag |= PARODD;
    }
  else if (m_Parity == EvenParity)
    { // even
    t.c_cflag |= PARENB;
    t.c_cflag &= ~PARODD;
    }

  // set stop bits
  if (m_StopBits == StopBits1)
    { 
    t.c_cflag &= ~CSTOPB; 
    }
  else if (m_StopBits == StopBits2)
    {
    t.c_cflag |= CSTOPB; 
    }

  // set handshaking
  if (m_HardwareHandshake == HandshakeOn)
    {
#ifdef sgi
    t.c_cflag |= CNEW_RTSCTS;       // enable hardware handshake
#else
    t.c_cflag |= CRTSCTS;           
#endif
    }
  else
    {
#ifdef sgi
    t.c_cflag &= ~CNEW_RTSCTS;       // turn off hardware handshake
#else
    t.c_cflag &= ~CRTSCTS;
#endif     
    } 

  // set timeout period
  t.c_cc[VMIN] = 0;                     // use constant, not interval timout
  t.c_cc[VTIME] = m_TimeoutPeriod/100;  // wait time is in 10ths of a second

  // set I/O information
  if (tcsetattr(m_PortHandle,TCSADRAIN,&t) == -1)
    {
    igstkLogMacro( DEBUG, "SetTransferParameters failed.\n" );

    this->InvokeEvent( SetTransferParametersFailureEvent() );

    return FAILURE;
    }

  igstkLogMacro( DEBUG, "SetTransferParameters succeeded.\n" );

  return SUCCESS;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalClosePort( void )
{
  int i;

  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalClosePort called ...\n" );
  // restore the comm port state to from before it was opened
  tcsetattr(m_PortHandle,TCSANOW,&m_SaveTermIOs);

  if (close(m_PortHandle) == -1)
    {
    this->InvokeEvent( ClosePortFailureEvent() );
    return FAILURE;
    }

  m_PortHandle = INVALID_HANDLE;

  igstkLogMacro( DEBUG, "Communication port closed.\n" );

  return SUCCESS;
}


void SerialCommunicationForPosix::InternalSendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalSendBreak called ...\n" );

  // send the break
  if (tcsendbreak(m_PortHandle,0) == -1)
    {
    this->InvokeEvent( SendBreakFailureEvent() );
    }
}


void SerialCommunicationForPosix::InternalSleep( void )
{
  int milliseconds = m_SleepPeriod;

  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalSleep called ...\n" );

  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = milliseconds/1000;
  sleep_time.tv_nsec = (milliseconds - sleep_time.tv_sec*1000)*1000000;
  nanosleep(&sleep_time,&dummy);
}


void SerialCommunicationForPosix::InternalPurgeBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalPurgeBuffers called ...\n" );

  tcflush(m_PortHandle, TCIOFLUSH);  // clear output buffers
}


void SerialCommunicationForPosix::InternalWrite( void )
{
  int i = 0;
  int m;
  int n = m_BytesToWrite;
  int writeError = 0;

  while (n > 0)
    { 
    if ((m = write(m_PortHandle, &m_OutputData[i], n)) == -1)
      {
      // if error is not EAGAIN, break
      m = 0;
      if (errno != EAGAIN) 
        {
        writeError = 1;
        break;
        }
      }

    n -= m;  // n is number of chars left to write
    i += m;  // i is the number of chars written
    }
  
  if (writeError)
    {
    this->InvokeEvent( WriteFailureEvent() );
    }
  else
    {
    this->InvokeEvent( WriteSuccessEvent() );
    }
}


void SerialCommunicationForPosix::InternalRead( void )
{
  int i = 0;
  int m;
  int n = m_BytesToRead;
  int readError = 0;

  // Read reply either until n bytes have been read,
  // or if UseReadTerminationCharacter is set then read
  // until the termination character is found.
  while (n > 0)
    {
    if ((m = read(m_PortHandle,&m_InputData[i], 1)) == -1)
      {
      // if error is not EAGAIN, break
      m = 0;
      if (errno != EAGAIN) 
        {
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

  // set the number of bytes that were read
  m_BytesRead = i;
  m_InputData[i] = '\0';

  // invoke the appropriate event
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
    this->InvokeEvent( ReadSuccessEvent() );
    }
}


void SerialCommunicationForPosix::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortHandle: " << m_PortHandle << std::endl;
}

} // end namespace igstk

