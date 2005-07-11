/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForLinux.cxx
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

#include "igstkSerialCommunicationForLinux.h"


namespace igstk
{ 
/** Constructor */
SerialCommunicationForLinux::SerialCommunicationForLinux() :  
SerialCommunication(), TIMEOUT_PERIOD(5000), NDI_INVALID_HANDLE(-1)
{
  this->m_PortHandle = SerialCommunicationForLinux::NDI_INVALID_HANDLE;
} 

SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalOpenCommunication( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalOpenPort called ...\n");
  static struct flock fl = { F_WRLCK, 0, 0, 0 }; /* for file locking */
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  struct termios t;
  int i;

  if( this->m_PortHandle != SerialCommunicationForLinux::NDI_INVALID_HANDLE )
  {
    igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalOpenPort : port is already open! ...\n");
  }

  char device[20];
  sprintf(device, "/dev/ttyS%.1d", this->GetPortNumber().Get() );
  std::cout << device << std::endl;

  /* port is readable/writable and is (for now) non-blocking */
  this->m_PortHandle = open(device,O_RDWR|O_NOCTTY|O_NDELAY);

  if (this->m_PortHandle == SerialCommunicationForLinux::NDI_INVALID_HANDLE) {
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;             /* bail out on error */
  }

  /* restore blocking now that the port is open (we just didn't want */
  /* the port to block while we were trying to open it) */
  fcntl(this->m_PortHandle, F_SETFL, 0);

#ifndef __APPLE__
  /* get exclusive lock on the serial port */
  /* on many unices, this has no effect for device files */
  if (fcntl(this->m_PortHandle, F_SETLK, &fl)) {
    close(this->m_PortHandle);
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;             /* bail out on error */
  }
#endif /* __APPLE__ */

  /* get I/O information */
  if (tcgetattr(this->m_PortHandle,&t) == -1) {
    fcntl(this->m_PortHandle, F_SETLK, &fu);
    close(this->m_PortHandle);
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;
  }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  tcgetattr(this->m_PortHandle,&m_SaveTermIOs);

  /* clear everything specific to terminals */
  t.c_lflag = 0;
  t.c_iflag = 0;
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    /* use constant, not interval timout */
  t.c_cc[VTIME] = SerialCommunicationForLinux::TIMEOUT_PERIOD/100;  /* wait for 5 secs max */

  if (tcsetattr(this->m_PortHandle,TCSANOW,&t) == -1) { /* set I/O information */
    fcntl(this->m_PortHandle, F_SETLK, &fu);
    close(this->m_PortHandle);
    this->InvokeEvent( OpenPortFailureEvent() );
    return FAILURE;
  }

  tcflush(this->m_PortHandle,TCIOFLUSH);         /* flush the buffers for good luck */
  igstkLogMacro( DEBUG, "COM port name: " << device << " opened.\n");

  return SUCCESS;
}


SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalSetUpDataBuffers( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalSetUpDataBuffers called ...\n");
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string

  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL)) 
  {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
    return FAILURE;
  }
  else
  {
    //Clear out buffers
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( DEBUG, "SetDataBufferSizeParameters with Read Buffer size = " << m_ReadBufferSize << " and Write Buffer Size = " << m_WriteBufferSize << " succeeded.\n");
    return SUCCESS;
  }
}


SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalSetTransferParameters( void )
{
  struct termios t;
  int newbaud;

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalSetTransferParameters called ...\n");

  unsigned int baud = this->m_BaudRate.Get();

#if defined(linux) || defined(__linux__)
  switch (baud)
    {
    case 9600:   newbaud = B9600;   break;
    case 14400:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:  newbaud = B57600;  break;
    case 115200: newbaud = B115200; break;
    default:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;

    }
#elif defined(__APPLE__)
  switch (baud)
    {
    case 9600:    newbaud = B9600;   break;
    case 14400:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    case 19200:   newbaud = B19200;  break;
    case 38400:   newbaud = B38400;  break;
    case 57600:   newbaud = B57600;  break;
    case 115200:  newbaud = B115200; break;
    default:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    }
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  switch (baud)
    {
    case 9600:    newbaud = 9600;   break;
    case 14400:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    case 19200:   newbaud = 19200;  break;
    case 38400:   newbaud = 38400;  break;
    case 57600:   newbaud = 57600;  break;
    case 115200:  newbaud = 115200; break;
    default:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    }
#else
  switch (baud)
    {
    case 9600:   newbaud = B9600;  break;
    case 14400:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    case 115200: 
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    default:
                 this->InvokeEvent( SetCommunicationParametersFailureEvent() );
                 std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
                 return FAILURE;
    }
#endif

  tcgetattr(this->m_PortHandle,&t);          /* get I/O information */
  t.c_cflag &= ~CSIZE;                /* clear flags */

#if defined(linux) || defined(__linux__)
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#elif defined(__APPLE__)
  cfsetispeed(&t, newbaud);
  cfsetospeed(&t, newbaud);
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  t.c_ospeed = newbaud;
#else
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#endif

  if (this->m_ByteSize.Get() == 8) {                 /* set data bits */
    t.c_cflag |= CS8; 
  }
  else if (this->m_ByteSize.Get() == 7) {
    t.c_cflag |= CS7;
  }
  else {
      this->InvokeEvent( SetCommunicationParametersFailureEvent() );
      std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
    return FAILURE;
  }

  if (this->m_Parity.Get() == 0) { // none                /* set parity */
    t.c_cflag &= ~PARENB;
    t.c_cflag &= ~PARODD;
  }
  else if (this->m_Parity.Get() == 1) { // odd
    t.c_cflag |= PARENB;
    t.c_cflag |= PARODD;
  }
  else if (this->m_Parity.Get() == 2) { // even
    t.c_cflag |= PARENB;
    t.c_cflag &= ~PARODD;
  }
  else {
      this->InvokeEvent( SetCommunicationParametersFailureEvent() );
      std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
    return FAILURE;
  }

  if (this->m_StopBits.Get() == 1) {                  /* set stop bits */
    t.c_cflag &= ~CSTOPB; 
  }
  else if (this->m_StopBits.Get() == 2) {
    t.c_cflag |= CSTOPB; 
  }
  else {
      this->InvokeEvent( SetCommunicationParametersFailureEvent() );
      std::cout << "Setting Communication Parameters Failed.\n" << std::endl;
    return FAILURE;
  }

  if (this->m_HardwareHandshake.Get()) {
#ifdef sgi
    t.c_cflag |= CNEW_RTSCTS;       /* enable hardware handshake */
#else
    t.c_cflag |= CRTSCTS;           
#endif
  }
  else {
#ifdef sgi
    t.c_cflag &= ~CNEW_RTSCTS;          /* turn off hardware handshake */
#else
    t.c_cflag &= ~CRTSCTS;
#endif     
  } 

  tcsetattr(this->m_PortHandle,TCSADRAIN,&t);  /* set I/O information */

  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n");
  return SUCCESS;
}

SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalClearBuffersAndClosePort( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalClearBuffersAndClosePort called ...\n");
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  return this->InternalClosePort();
}

SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalClosePort( void )
{
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int i;

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalClosePort called ...\n");
  /* restore the comm port state to from before it was opened */
  tcsetattr(this->m_PortHandle,TCSANOW,&m_SaveTermIOs);

  /* release our lock on the serial port */
  fcntl(this->m_PortHandle, F_SETLK, &fu);

  close(this->m_PortHandle);
  igstkLogMacro( DEBUG, "Communication port closed.\n");
  return SUCCESS;
}

SerialCommunicationForLinux::ResultType SerialCommunicationForLinux::InternalSetTimeoutPeriod( int milliseconds )
{
  struct termios t;

  if (tcgetattr(this->m_PortHandle,&t) == -1) {
    return FAILURE;
  }

  t.c_cc[VMIN] = 0;                  /* use constant, not interval timout */
  t.c_cc[VTIME] = milliseconds/100;  /* wait time is in 10ths of a second */

  if (tcsetattr(m_PortHandle,TCSANOW,&t) == -1) {
    return FAILURE;
  }

  return SUCCESS;
}

void SerialCommunicationForLinux::InternalSendBreak( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalSendBreak called ...\n");

  tcflush(this->m_PortHandle,TCIOFLUSH);     /* clear input/output buffers */
  tcsendbreak(this->m_PortHandle,0);         /* send the break */

  return;
}


void SerialCommunicationForLinux::InternalFlushOutputBuffer( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalFlushOutputBuffer called ...\n");
  int flushtype = TCIOFLUSH;
/*
  if (buffers == NDI_IFLUSH) {
    flushtype = TCIFLUSH;
  }
  else if (buffers == NDI_OFLUSH) {
*/
    flushtype = TCOFLUSH;
//  }    

  if( tcflush(this->m_PortHandle,flushtype) != 0)     /* clear input/output buffers */
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }

  return;
}


void SerialCommunicationForLinux::InternalWrite( void )
{
  int i = 0;
  int m;
  unsigned long  bytesToWrite = m_WriteNumberOfBytes;
//  unsigned long   bytesToWrite = strlen(m_OutputBuffer);

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalWrite called ...\n");
  while (bytesToWrite > 0) { 
    if ((m = write(this->m_PortHandle,&this->m_OutputBuffer[i], bytesToWrite)) == -1) {
      if (errno == EAGAIN) 
      { /* system cancelled us, retry */
        m = 0;
      }
      else {
        this->InvokeEvent( WriteFailureEvent() );
        igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalWrite failed ...\n");
        return;  /* IO error occurred */
      }
    }

    bytesToWrite -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  std::cout << "Written bytes = " << i << std::endl;
  this->InvokeEvent( WriteSuccessfulEvent());
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::InternalWrite succeeded ...\n");
  return;  /* return the number of characters written */
}


void SerialCommunicationForLinux::InternalRead( void )
{
  int i = 0;
  int m;
  int n = this->m_ReadNumberOfBytes;
//  int n = this->m_ReadBufferSize;

  while (n > 0) {                 /* read reply until ReadTerminationCharacter 
                                   if UseTerminationCharacter option is set */
    if ((m = read(this->m_PortHandle,&this->m_InputBuffer[i], n)) == -1) {
      if (errno == EAGAIN) 
      {      /* cancelled, so retry */
        m = 0;
      }
      else {
        this->InvokeEvent( ReadFailureEvent() );
        return;  /* IO error occurred */
      }
    }
    else if (m == 0) { /* no characters read, must have timed out */
      this->InvokeEvent( ReadTimeoutEvent() );
      return;
    }
    n -= m;  /* n is number of chars left to read */
    i += m;  /* i is the number of chars read */
    if ( this->m_UseReadTerminationCharacter )
      {
      if (this->m_InputBuffer[i-1] == this->m_ReadTerminationCharacter ) {  /* done when ReadTerminationCharacter received */
        break;
        }
      }
  }

  this->m_ReadDataSize = i;
  this->m_ReadBufferOffset = 0;
  this->m_InputBuffer[i] = 0; // terminate the string
  std::cout << "Read number of bytes = " << i << ". String: " << this->m_InputBuffer << std::endl;
  this->InvokeEvent( ReadSuccessfulEvent());
  return;
}
/*
  int bytesAvailable = -1;
  ioctl(this->m_PortHandle, FIONREAD, &bytesAvailable);
  std::cout << "Number bytes available for read = " << bytesAvailable << std::endl;
*/
} // end namespace igstk

