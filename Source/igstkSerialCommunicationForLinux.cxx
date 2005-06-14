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

void SerialCommunicationForLinux::OpenPortProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::OpenPortProcessing called ...\n");
  char portName[20];
  sprintf(portName, "/dev/ttyS%.1d", this->GetPortNumber().Get() );
  std::cout << portName << std::endl;

  if (this->m_PortHandle != SerialCommunicationForLinux::INVALID_HANDLE_VALUE)
  {
    igstkLogMacro( DEBUG, "**** COM port already in use. Exiting .... ****\n");
    return;
  }

  this->m_PortHandle = open(portName, O_RDWR | O_NOCTTY); // | O_NDELAY);
  // O_RDWR (Read/Write), O_NOCTTY (not a controlling terminal)
  // O_NDELAY (do not care what state the DCD signal line is in)
  if( this->m_PortHandle < 0)
  {
    m_pOpenPortResultInput = &m_OpenPortFailureInput;
    this->InvokeEvent( OpenPortFailureEvent() );
  }
  else
  {
    m_pOpenPortResultInput = &m_OpenPortSuccessInput;
    igstkLogMacro( DEBUG, "COM port name: " << portName << " opened.\n");
  }
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
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SetUpDataTransferParametersProcessing called ...\n");
  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpFailureInput;

  // Control setting for a serial communications device
  struct termio portSettings;
  struct termios portOptions;
  unsigned int baudRate;

  if (!( (ioctl(this->m_PortHandle, TCGETA, &portSettings)>=0) &&
         (tcgetattr(this->m_PortHandle, &portOptions)>=0) ))
  {
       this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
       std::cout << "Setup Communication Failed.\n" << std::endl; 
       return;
  }

  // Some initial port settings
  //Input mode settings
  portSettings.c_iflag &= ~(IXON | IXOFF | IXANY); // Hardware handshake.
  portSettings.c_iflag &= ~IGNCR; // Allow carriage return.
  portSettings.c_iflag &= ~ICRNL; // Do not map CR to NL. 

  //Output mode settings
  portSettings.c_oflag &= ~OCRNL; // Do not map CR to NL.
  portSettings.c_oflag &= ~OPOST; // No post-processing of output. 
   
  //Local mode settings (line discipline)
  portSettings.c_lflag &= ~ICANON; // Non-canonical input.
  portSettings.c_lflag &= ~ECHO; // No echoing of received/transmitted chars.
  portSettings.c_lflag &= ~ISIG; // Disable signals (no input <cntr> char checking.

  //Control mode settings (Hardware control)
  portSettings.c_cflag |= CLOCAL; // Local line. Modem off.
  portSettings.c_cflag |= CREAD; // Enable read from port.

  // Baudrate parameter settings
  portSettings.c_cflag &= ~CBAUD; // Clear baud rate bits.
  switch( this->m_BaudRate.Get() )
  {
  case 2400:
          baudRate = B2400; break;
  case 9600:
          baudRate = B9600; break;
  case 19200:
          baudRate = B19200; break;
  case 38400:
          baudRate = B38400; break;
  case 57600:
          baudRate = B57600; break;
  case 115200:
          baudRate = B115200; break;
  default: 
          baudRate = B9600; 
  }
  portSettings.c_cflag |= baudRate;

  // Bytesize parameter settings
  switch(this->m_ByteSize.Get())
  {
  case 7: 
       portSettings.c_cflag |= CS7; break;
  case 8: 
       portSettings.c_cflag |= CS8; break;
  default: //return error; shouldn't come here in the first place.
       portSettings.c_cflag |= CS8; 
  }
  // Parity parameter settings
  switch(this->m_Parity.Get())
  {
  case 0:  // None
       portSettings.c_cflag &= ~PARENB; // Disable parity.
       break;
  case 1:  // Odd
       portSettings.c_cflag |= PARENB; // Enable parity.
       portSettings.c_cflag |= PARODD; // Odd parity.
       break;
  case 2:  // Even
       portSettings.c_cflag |= PARENB; // Enable parity.
       portSettings.c_cflag &= ~PARODD; // Even parity.
       break;
  default: //return error; shouldn't come here in the first place.
       portSettings.c_cflag &= ~PARENB; // Disable parity.
  }

  // Stop bit parameter settings
  switch(this->m_StopBits.Get())
  {
  case 1: 
       portSettings.c_cflag &= ~CSTOPB; break; 
  case 2:
       portSettings.c_cflag |= CSTOPB; break; 
  default: //return error; shouldn't come here in the first place.
       portSettings.c_cflag &= ~CSTOPB;  
  }

  //Control characters
  portSettings.c_cc[VMIN] = 0; //return immediately if no char
  portSettings.c_cc[VTIME] = 1; //read waits 100 miliseconds

  cfsetispeed(&portOptions, baudRate); // Set input baudrate.
  cfsetospeed(&portOptions, baudRate); // Set output baudrate.

  //Set up communication state using Linux services 
  if (!(  (ioctl(this->m_PortHandle, TCSETA, &portSettings)>=0)
      && (tcsetattr(this->m_PortHandle, TCSANOW, &portOptions)>=0) ))
  {
    this->InvokeEvent( SetupCommunicationParametersFailureEvent() );
    std::cout << "Setup Communication Failed.\n" << std::endl;
    return;
  }
  else
  {
    igstkLogMacro( DEBUG, "SetupCommunicationParameters succeeded.\n");
  }

  m_pDataTransferParametersSetUpResultInput = &m_DataTransferParametersSetUpSuccessInput;
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
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::ClosePortProcessing called ...\n");
  close(this->m_PortHandle);
  this->m_PortHandle = SerialCommunicationForLinux::INVALID_HANDLE_VALUE;
  igstkLogMacro( DEBUG, "Communication port closed.\n");
}

void SerialCommunicationForLinux::RestPortProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::RestPortProcessing called ...\n");
  // clear input/output buffers
  tcflush(this->m_PortHandle,TCIOFLUSH);
  // send the break
  tcsendbreak(this->m_PortHandle, 0);
}


void SerialCommunicationForLinux::FlushOutputBufferProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::FlushOutputBufferProcessing called ...\n");
  if (tcflush(this->m_PortHandle,TCIFLUSH)!=0)
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }
}


void SerialCommunicationForLinux::SendStringProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::SendStringProcessing called ...\n");
  //try writing to the hardware
  unsigned long   bytesToWrite = strlen(m_OutputBuffer);
  unsigned long   writtenBytes;

  writtenBytes = write(this->m_PortHandle, this->m_OutputBuffer, bytesToWrite);
 
  //check if writing event successful
  if (writtenBytes==bytesToWrite)
  {
    std::cout << "Written bytes = " << writtenBytes << std::endl;
    this->InvokeEvent( SendStringSuccessfulEvent());
  }
  else
  {
      this->InvokeEvent( SendStringFailureEvent() );
  }
}


void SerialCommunicationForLinux::ReceiveStringProcessing( void )
{
  igstkLogMacro( DEBUG, "SerialCommunicationForLinux::ReceiveStringProcessing called ...\n");
  unsigned long readBytes;
  readBytes = read(this->m_PortHandle, this->m_InputBuffer, this->m_ReadBufferSize);

  std::cout << "Read number of bytes = " << readBytes << ". String: " << m_InputBuffer << std::endl;
 
  
}
/*
  int bytesAvailable = -1;
  ioctl(this->m_PortHandle, FIONREAD, &bytesAvailable);
  std::cout << "Number bytes available for read = " << bytesAvailable << std::endl;
*/
} // end namespace igstk

