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

  this->m_PortHandle = open(portName, O_RDWR | O_NOCTTY); // | O_NDELAY);
  // O_RDWR (Read/Write), O_NOCTTY (not a controlling terminal)
  // O_NDELAY (do not care what state the DCD signal line is in)
  if( this->m_PortHandle < 0)
  {
    this->m_PortNumber = m_InvalidPortNumber;
    m_ResultOfOpenCommunicationPortProcessing = false;
    this->InvokeEvent( OpenPortFailureEvent() );
  }
  else
  {
    m_ResultOfOpenCommunicationPortProcessing = true;
    igstkLogMacro( igstk::Logger::DEBUG, "COM port name: ");
    igstkLogMacro( igstk::Logger::DEBUG, portName);
    igstkLogMacro( igstk::Logger::DEBUG, " opened.\n");
  }
}


void SerialCommunicationForLinux::SetUpDataBuffersProcessing( void )
{
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string

  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL)) 
  {
    m_ResultOfSetUpDataBuffersProcessing = false;
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
  {
    m_ResultOfSetUpDataBuffersProcessing = true;
    //Clear out buffers
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( igstk::Logger::DEBUG, "SetDataBufferSizeParameters ");
    igstkLogMacro( igstk::Logger::DEBUG, "with Read Buffer size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_ReadBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " and Write Buffer Size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_WriteBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " succeeded.\n");
  }
}


void SerialCommunicationForLinux::SetUpDataTransferParametersProcessing( void )
{
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
  switch(this->m_BaudRate)
  {
  case BAUD2400:
          baudRate = B2400; break;
  case BAUD9600:
          baudRate = B9600; break;
  case BAUD19200:
          baudRate = B19200; break;
  case BAUD38400:
          baudRate = B38400; break;
  case BAUD57600:
          baudRate = B57600; break;
  case BAUD115200:
          baudRate = B115200; break;
  default: 
          baudRate = B9600; 
  }
  portSettings.c_cflag |= baudRate;

  // Bytesize parameter settings
  switch(this->m_ByteSize)
  {
  case SEVEN_BITS: 
       portSettings.c_cflag |= CS7; break;
  case EIGHT_BITS: 
       portSettings.c_cflag |= CS8; break;
  default: //return error; shouldn't come here in the first place.
       portSettings.c_cflag |= CS8; 
  }
  // Parity parameter settings
  switch(this->m_Parity)
  {
  case NO_PARITY: 
       portSettings.c_cflag &= ~PARENB; // Disable parity.
       break;
  case ODD_PARITY: 
       portSettings.c_cflag |= PARENB; // Enable parity.
       portSettings.c_cflag |= PARODD; // Odd parity.
       break;
  case EVEN_PARITY: 
       portSettings.c_cflag |= PARENB; // Enable parity.
       portSettings.c_cflag &= ~PARODD; // Even parity.
       break;
  default: //return error; shouldn't come here in the first place.
       portSettings.c_cflag &= ~PARENB; // Disable parity.
  }

  // Stop bit parameter settings
  switch(this->m_StopBits)
  {
  case ONE_STOP_BIT: 
       portSettings.c_cflag &= ~CSTOPB; break; 
  case TWO_STOP_BITS: 
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
  }
  else
  {
    igstkLogMacro( igstk::Logger::DEBUG, "SetupCommunicationParameters succeeded.\n");
  }
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
  if (tcflush(this->m_PortHandle,TCIFLUSH)!=0)
  {
    this->InvokeEvent( FlushOutputBufferFailureEvent() );
  }
}


void SerialCommunicationForLinux::SendStringProcessing( void )
{
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

