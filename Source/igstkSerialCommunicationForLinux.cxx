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
SerialCommunication() , INVALID_HANDLE_VALUE(-1), NDI_MAX_SAVE_STATE(4), TIMEOUT_PERIOD(5000)
{
  this->m_PortHandle = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
  for(int i=0; i < NDI_MAX_SAVE_STATE; ++i )
  {
    this->m_OpenHandles[i] = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
  }
} 

void SerialCommunicationForLinux::OpenPortProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::OpenPortProcessing called ...\n");
  static struct flock fl = { F_WRLCK, 0, 0, 0 }; /* for file locking */
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  struct termios t;
  int i;

  if( this->m_PortHandle != SerialCommunicationForLinux::INVALID_HANDLE_VALUE )
  {
    igstkLogMacro( DEBUG, "SerialCommunicationForLinux::OpenPortProcessing : port is already open! ...\n");
  }

  char device[20];
  sprintf(device, "/dev/ttyS%.1d", this->GetPortNumber().Get() );
  std::cout << device << std::endl;

  /* port is readable/writable and is (for now) non-blocking */
  this->m_PortHandle = open(device,O_RDWR|O_NOCTTY|O_NDELAY);

  if (this->m_PortHandle == SerialCommunicationForLinux::INVALID_HANDLE_VALUE) {
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
    return;             /* bail out on error */
  }

  /* restore blocking now that the port is open (we just didn't want */
  /* the port to block while we were trying to open it) */
  fcntl(this->m_PortHandle, F_SETFL, 0);

#ifndef __APPLE__
  /* get exclusive lock on the serial port */
  /* on many unices, this has no effect for device files */
  if (fcntl(this->m_PortHandle, F_SETLK, &fl)) {
    close(this->m_PortHandle);
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
    return;             /* bail out on error */
  }
#endif /* __APPLE__ */

  /* get I/O information */
  if (tcgetattr(this->m_PortHandle,&t) == -1) {
    fcntl(this->m_PortHandle, F_SETLK, &fu);
    close(this->m_PortHandle);
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
    return;
  }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (m_OpenHandles[i] == this->m_PortHandle || m_OpenHandles[i] == SerialCommunicationForLinux::INVALID_HANDLE_VALUE) {
      m_OpenHandles[i] = this->m_PortHandle;
      tcgetattr(this->m_PortHandle,&m_NDISaveTermIOs[i]);
      break;
    }
  }

  /* clear everything specific to terminals */
  t.c_lflag = 0;
  t.c_iflag = 0;
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    /* use constant, not interval timout */
  t.c_cc[VTIME] = SerialCommunicationForLinux::TIMEOUT_PERIOD/100;  /* wait for 5 secs max */

  if (tcsetattr(this->m_PortHandle,TCSANOW,&t) == -1) { /* set I/O information */
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      m_OpenHandles[i] = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
    }
    fcntl(this->m_PortHandle, F_SETLK, &fu);
    close(this->m_PortHandle);
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
    return;
  }

  tcflush(this->m_PortHandle,TCIOFLUSH);         /* flush the buffers for good luck */
  m_pOpenPortResultInput = &m_OpenPortSuccessInput;
  igstkLogMacro( DEBUG, "COM port name: " << device << " opened.\n");

  return;
}


void SerialCommunicationForLinux::SetUpDataBuffersProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SetUpDataBuffersProcessing called ...\n");
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string

  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL)) 
  {
    m_pDataBuffersSetUpResultInput = &m_DataBuffersSetUpFailureInput;
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
  {
    m_pDataBuffersSetUpResultInput = &m_DataBuffersSetUpSuccessInput;
    //Clear out buffers
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( DEBUG, "SetDataBufferSizeParameters with Read Buffer size = " << m_ReadBufferSize << " and Write Buffer Size = " << m_WriteBufferSize << " succeeded.\n");
  }
}


void SerialCommunicationForLinux::SetUpDataTransferParametersProcessing( void )
{
  struct termios t;
  int newbaud;

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SetUpDataTransferParametersProcessing called ...\n");
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpFailureInput;

  unsigned int baud = this->m_BaudRate.Get();

#if defined(linux) || defined(__linux__)
  switch (baud)
    {
    case 9600:   newbaud = B9600;   break;
    case 14400:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:  newbaud = B57600;  break;
    case 115200: newbaud = B115200; break;
    default:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;

    }
#elif defined(__APPLE__)
  switch (baud)
    {
    case 9600:    newbaud = B9600;   break;
    case 14400:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    case 19200:   newbaud = B19200;  break;
    case 38400:   newbaud = B38400;  break;
    case 57600:   newbaud = B57600;  break;
    case 115200:  newbaud = B115200; break;
    default:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    }
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  switch (baud)
    {
    case 9600:    newbaud = 9600;   break;
    case 14400:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    case 19200:   newbaud = 19200;  break;
    case 38400:   newbaud = 38400;  break;
    case 57600:   newbaud = 57600;  break;
    case 115200:  newbaud = 115200; break;
    default:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    }
#else
  switch (baud)
    {
    case 9600:   newbaud = B9600;  break;
    case 14400:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    case 115200: 
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
    default:
                 this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
                 std::cout << "Setup Communication Failed.\n" << std::endl;
                 return;
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
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
    std::cout << "Setup Communication Failed.\n" << std::endl;
    return;
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
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
    std::cout << "Setup Communication Failed.\n" << std::endl;
    return;
  }

  if (this->m_StopBits.Get() == 1) {                  /* set stop bits */
    t.c_cflag &= ~CSTOPB; 
  }
  else if (this->m_StopBits.Get() == 2) {
    t.c_cflag |= CSTOPB; 
  }
  else {
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
    std::cout << "Setup Communication Failed.\n" << std::endl;
    return;
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

  igstkLogMacro( DEBUG, "SetupCommunicationParameters succeeded.\n");
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpSuccessInput;
  return;
}

void SerialCommunicationForLinux::ClearBuffersAndClosePortProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::ClearBuffersAndClosePortProcessing called ...\n");
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  this->ClosePortProcessing();
}

void SerialCommunicationForLinux::ClosePortProcessing( void )
{
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int i;

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::ClosePortProcessing called ...\n");
  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (m_OpenHandles[i] == this->m_PortHandle && m_OpenHandles[i] != SerialCommunicationForLinux::INVALID_HANDLE_VALUE) {
      tcsetattr(this->m_PortHandle,TCSANOW,&m_NDISaveTermIOs[i]);
      m_OpenHandles[i] = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
      break;
    }
  }

  /* release our lock on the serial port */
  fcntl(this->m_PortHandle, F_SETLK, &fu);

  close(this->m_PortHandle);
  igstkLogMacro( DEBUG, "Communication port closed.\n");
}

void SerialCommunicationForLinux::RestPortProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::RestPortProcessing called ...\n");

  tcflush(this->m_PortHandle,TCIOFLUSH);     /* clear input/output buffers */
  tcsendbreak(this->m_PortHandle,0);         /* send the break */

  return;
}


void SerialCommunicationForLinux::FlushOutputBufferProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::FlushOutputBufferProcessing called ...\n");
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


void SerialCommunicationForLinux::SendStringProcessing( void )
{
  int i = 0;
  int m;
  unsigned long   bytesToWrite = strlen(m_OutputBuffer);

  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SendStringProcessing called ...\n");
  while (bytesToWrite > 0) { 
    if ((m = write(this->m_PortHandle,&this->m_OutputBuffer[i], bytesToWrite)) == -1) {
      if (errno == EAGAIN) 
      { /* system cancelled us, retry */
        m = 0;
      }
      else {
        this->InvokeEvent( SendStringFailureEvent() );
        igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SendStringProcessing failed ...\n");
        return;  /* IO error occurred */
      }
    }

    bytesToWrite -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  std::cout << "Written bytes = " << i << std::endl;
  this->InvokeEvent( SendStringSuccessfulEvent());
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SendStringProcessing succeeded ...\n");
  return;  /* return the number of characters written */
}


void SerialCommunicationForLinux::ReceiveStringProcessing( void )
{
  int i = 0;
  int m;
  int n = this->m_ReadBufferSize;

  while (n > 0) {                        /* read reply until <CR> */
    if ((m = read(this->m_PortHandle,&this->m_InputBuffer[i], n)) == -1) {
      if (errno == EAGAIN) 
      {      /* cancelled, so retry */
        m = 0;
      }
      else {
        this->InvokeEvent( ReceiveStringFailureEvent() );
        return;  /* IO error occurred */
      }
    }
    else if (m == 0) { /* no characters read, must have timed out */
      this->InvokeEvent( ReceiveStringReadTimeoutEvent() );
      return;
    }
    n -= m;  /* n is number of chars left to read */
    i += m;  /* i is the number of chars read */
    if (this->m_InputBuffer[i-1] == '\r') {  /* done when carriage return received */
      break;
    }
  }

  this->m_ReadDataSize = i;
  this->m_ReadBufferOffset = 0;
  this->m_InputBuffer[i] = 0; // terminate the string
  std::cout << "Read number of bytes = " << i << ". String: " << this->m_InputBuffer << std::endl;
  this->InvokeEvent( ReceiveStringSuccessfulEvent());
  return;
}
/*
  int bytesAvailable = -1;
  ioctl(this->m_PortHandle, FIONREAD, &bytesAvailable);
  std::cout << "Number bytes available for read = " << bytesAvailable << std::endl;
*/
} // end namespace igstk

