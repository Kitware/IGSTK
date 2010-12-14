/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForPosix.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "igstkConfigure.h"
#include "igstkSerialCommunicationForPosix.h"
#include "igstkPulseGenerator.h"

/** Standard includes */
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

/** Includes for serial communication */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if defined (HAVE_TERMIOS_H)
  #include <termios.h> 
#elif defined (HAVE_TERMIO_H)
  #include <termio.h>
#endif


namespace igstk
{ 


SerialCommunicationForPosix
::SerialCommunicationForPosix():m_StateMachine(this)
{
  m_OldTimeoutPeriod = 0;
  m_PortHandle = INVALID_HANDLE;
} 


SerialCommunicationForPosix
::~SerialCommunicationForPosix()
{
} 


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalOpenPort( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::InternalOpenPort"
                 " called ...\n" );

  unsigned int portNumber = this->GetPortNumber();
  const char *device = "";  

  if ( portNumber < 8 )
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

  // port is readable/writable and is (for now) non-blocking
  m_PortHandle = open(device,O_RDWR|O_NOCTTY|O_NDELAY);

  if (m_PortHandle != INVALID_HANDLE)
    {
    // restore blocking now that the port is open (we just didn't want
    // the port to block while we were trying to open it)
    fcntl(m_PortHandle, F_SETFL, 0);

    // get I/O information
    struct termios t;
    if (tcgetattr(m_PortHandle,&t) != -1)
      {
      // clear everything specific to terminals
      t.c_lflag = 0;
      t.c_iflag = 0;
      t.c_oflag = 0;

      // use constant, not interval timout
      t.c_cc[VMIN] = 0;
      t.c_cc[VTIME] = (this->GetTimeoutPeriod()+99)/100;
      m_OldTimeoutPeriod = this->GetTimeoutPeriod();

      // set initial I/O parameters
      if (tcsetattr(m_PortHandle,TCSANOW,&t) != -1)
        {
        // flush the buffers for good luck
        if (tcflush(m_PortHandle,TCIOFLUSH) != -1)
          {
          igstkLogMacro( DEBUG, "COM port name: " << device << " opened.\n" );
          return SUCCESS;
          }
        }
      }

    // if the code gets to here, an error occurred after the port opened
    close(m_PortHandle);
    }

  // if the code gets to here, an error occurred before the port opened
  m_PortHandle = INVALID_HANDLE;

  return FAILURE;
}

SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalUpdateParameters( void )
{

  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalUpdateParameters called ...\n" );

  unsigned int timeoutPeriod = this->GetTimeoutPeriod();
  unsigned int baud = this->GetBaudRate();
  DataBitsType dataBits = this->GetDataBits();
  ParityType parity = this->GetParity();
  StopBitsType stopBits = this->GetStopBits();
  HandshakeType handshake = this->GetHardwareHandshake();

  int newbaud;

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

  struct termios t;
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
  if (dataBits == DataBits8)
    {
    t.c_cflag |= CS8;
    }
  else if (dataBits == DataBits7)
    {
    t.c_cflag |= CS7;
    }

  // set parity
  if (parity == NoParity)
    { // none
    t.c_cflag &= ~PARENB;
    t.c_cflag &= ~PARODD;
    }
  else if (parity == EvenParity)
    { // even
    t.c_cflag |= PARENB;
    t.c_cflag &= ~PARODD;
    }

  // set stop bits
  if (stopBits == StopBits1)
    { 
    t.c_cflag &= ~CSTOPB; 
    }
  else if (stopBits == StopBits2)
    {
    t.c_cflag |= CSTOPB; 
    }

  // enable hardware handshake by default
#ifdef sgi
  t.c_cflag |= CNEW_RTSCTS;
#else
  t.c_cflag |= CRTSCTS;
#endif

  // turn off hardware handshake if requested
  if (handshake == HandshakeOff)
    {
#ifdef sgi
    t.c_cflag &= ~CNEW_RTSCTS;
#else
    t.c_cflag &= ~CRTSCTS;
#endif
    } 

  // set timeout period
  t.c_cc[VMIN] = 0;                     // use constant, not interval timout
  t.c_cc[VTIME] = (timeoutPeriod + 99)/100; // wait time in 10ths of a second
  m_OldTimeoutPeriod = timeoutPeriod;

  ResultType result = FAILURE;
  // set I/O information
  if (tcsetattr(m_PortHandle,TCSADRAIN,&t) != -1)
    {
    result = SUCCESS;
    }

  return result;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalClosePort( void )
{

  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalClosePort called ...\n" );

  ResultType result = FAILURE;
  if (close(m_PortHandle) != -1)
    {
    igstkLogMacro( DEBUG, "Communication port closed.\n" );
    result = SUCCESS;
    m_PortHandle = INVALID_HANDLE;
    }

  return result;
}


/** Set the RTS value 
 *  0 : Clear the RTS (request-to-send) signal 
 *  1 : Sends the RTS signal */
SerialCommunicationForPosix::ResultType 
SerialCommunicationForPosix::InternalSetRTS(unsigned int signal)
{
  int rs232bits = 0;

  ioctl(m_PortHandle, TIOCMGET, &rs232bits);
  if (signal)
    {
    rs232bits |= TIOCM_RTS;
    }
  else
    {
    rs232bits &= ~TIOCM_RTS;
    }
  int errval = ioctl(m_PortHandle, TIOCMSET, &rs232bits);

  if(errval)
    {
    igstkLogMacro( WARNING, "SetRTS failed.\n" );
    return FAILURE;
    }

  igstkLogMacro( DEBUG, "SetRTS succeeded...\n" );
  return SUCCESS;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalSendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalSendBreak called ...\n" );

  // send the break
  ResultType result = FAILURE;
  if (tcsendbreak(m_PortHandle,0) != -1)
    {
    result = SUCCESS;
    }

  return result;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalPurgeBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalPurgeBuffers called ...\n" );

  ResultType result = FAILURE;
  if (tcflush(m_PortHandle, TCIOFLUSH) != -1)
    {
    result = SUCCESS;
    }

  return result;
}


void SerialCommunicationForPosix::InternalSleep( unsigned int milliseconds )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForPosix::"
                 "InternalSleep called ...\n" );
  PulseGenerator::Sleep( milliseconds );
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalWrite( const char *data,
                                            unsigned int n )
{
  unsigned int i = 0;
  int m;
  ResultType writeError = SUCCESS;

  while (n > 0)
    { 
    if ((m = write(m_PortHandle, &data[i], n)) == -1)
      {
      // if error is not EAGAIN, break
      m = 0;
      if (errno != EAGAIN) 
        {
        writeError = FAILURE;
        break;
        }
      }

    n -= m;  // n is number of chars left to write
    i += m;  // i is the number of chars written
    }
  
  return writeError;
}


SerialCommunicationForPosix::ResultType
SerialCommunicationForPosix::InternalRead( char *data,
                                           unsigned int n,
                                           unsigned int &bytesRead )
{
  char terminationCharacter = this->GetReadTerminationCharacter();
  bool useTerminationCharacter = this->GetUseReadTerminationCharacter();
  unsigned int timeoutPeriod = this->GetTimeoutPeriod();

  unsigned int i = 0;
  int m;
  ResultType readError = SUCCESS;

  // check to see if timeout needs to be adjusted
  if (m_OldTimeoutPeriod != timeoutPeriod)
    {
    struct termios t;

    readError = FAILURE;
    if (tcgetattr(m_PortHandle, &t) != -1)
      {
      t.c_cc[VMIN] = 0;
      t.c_cc[VTIME] = (timeoutPeriod+99)/100;
      if (tcsetattr(m_PortHandle,TCSANOW,&t) != -1)
        {
        m_OldTimeoutPeriod = timeoutPeriod;
        readError = SUCCESS;
        }
      }
    }

  if (readError == SUCCESS)
    {
    // Read reply either until n bytes have been read,
    // or if UseReadTerminationCharacter is set then read
    // until the termination character is found.
    while (n > 0)
      {
      if ((m = read(m_PortHandle, &data[i], 1)) == -1)
        {
        // if error is not EAGAIN, break
        m = 0;
        if (errno != EAGAIN) 
          {
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

  // set the number of bytes that were read
  bytesRead = i;
  data[i] = '\0';

  return readError;
}


void SerialCommunicationForPosix::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortHandle: " << m_PortHandle << std::endl;
  os << indent << "OldTimeoutPeriod: " << m_OldTimeoutPeriod << std::endl;
}

} // end namespace igstk
