/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSerialCommunication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkSerialCommunication.h"


namespace igstk
{ 
/** Constructor */
SerialCommunication::SerialCommunication() : 
m_ReadBufferSize(3000), m_WriteBufferSize(200), m_InputBuffer( NULL) 
{
  /** Default communication settings */
  this->m_BaudRate = BAUD9600;
  this->m_ByteSize = EIGHT_BITS;
  this->m_Parity = NO_PARITY;
  this->m_StopBits = ONE_STOP_BIT;
  this->m_HardwareHandshake = HANDSHAKE_OFF;

  /** Hardware Port Settings */
  this->m_PortNumber = -1;
  this->m_PortHandle = (HandleType) INVALID_HANDLE_VALUE;

  /** Communication Time Out Settings */
  //ReadTimeout = m_ReadTotalTimeoutConstant + m_ReadTotalTimeoutMultiplier*Number_Of_Bytes_Read 
  m_ReadIntervalTimeout = 0xFFFF;
  m_ReadTotalTimeoutMultiplier = 0;
  m_ReadTotalTimeoutConstant = 0;
  //WriteTimeout = m_WriteTotalTimeoutConstant + m_WriteTotalTimeoutMultiplier*Number_Of_Bytes_Written 
  m_WriteTotalTimeoutMultiplier = 10;
  m_WriteTotalTimeoutConstant = 500;
} 

/** Destructor */
SerialCommunication::~SerialCommunication()  
{
  // Close communication, if any 
  this->CloseCommunication();
}

void SerialCommunication::CloseCommunication( void )
{
  this->CloseCommunicationPort();
}


void SerialCommunication::OpenCommunication( const void *data )
{
    // Read data from XML file
    this->OpenCommunicationPort( 1 );
    this->SetDataBufferSizeParameters();
    this->SetupCommunicationParameters();
    this->SetCommunicationTimeoutParameters();
}


void SerialCommunication::OpenCommunicationPort( const unsigned int portNum )
{
  char portName[25];
  sprintf(portName, "COM%d", portNum );
  if (this->m_PortHandle != (HandleType)INVALID_HANDLE_VALUE)
  {
    igstkLogMacro( igstk::Logger::DEBUG, "Object in use with port. Closing this port ...\n");
  }
  this->m_PortHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                                  0,   // opened with exclusive-access 
                                  0,    // no security attributes
                                  OPEN_EXISTING, 
                                  FILE_FLAG_OVERLAPPED, // For asynchronous write operations 
                                  NULL  
                                  );

  if( this->m_PortHandle == INVALID_HANDLE_VALUE)
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


void SerialCommunication::SetDataBufferSizeParameters( void )
{
  if (m_InputBuffer!=NULL) delete m_InputBuffer; 
  m_InputBuffer = new unsigned char[ m_ReadBufferSize ];
  if (!( (this->m_PortHandle!=INVALID_HANDLE_VALUE) && \
         (SetupComm(this->m_PortHandle, m_ReadBufferSize, m_WriteBufferSize)) ))
  {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
  }
  else
  {
    //Clear out buffers
    PurgeComm(this->m_PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    m_DataSize = 0;
    m_BufferOffset = 0;
    this->m_BufferOffset = 0;
    this->m_DataSize = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( igstk::Logger::DEBUG, "SetDataBufferSizeParameters with Read Buffer size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_ReadBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " and Write Buffer Size = ");
    igstkLogMacro( igstk::Logger::DEBUG, m_WriteBufferSize);
    igstkLogMacro( igstk::Logger::DEBUG, " succeeded.\n");
  }
}


void SerialCommunication::SetCommunicationTimeoutParameters( void )
{
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
}


void SerialCommunication::SetupCommunicationParameters( void )
{
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
}

void SerialCommunication::CloseCommunicationPort( void )
{
  if (this->m_PortHandle != (HandleType)INVALID_HANDLE_VALUE)
  {
    CloseHandle(this->m_PortHandle);
    this->m_PortHandle = (HandleType)INVALID_HANDLE_VALUE;
    igstkLogMacro( igstk::Logger::DEBUG, "Communication port closed.\n");
  }
  else
  {
    igstkLogMacro( igstk::Logger::DEBUG, "No port open for closing operation.\n");
  }
}


void SerialCommunication::SetLogger( LoggerType* logger )
{
    m_pLogger = logger;
}


SerialCommunication::LoggerType* SerialCommunication::GetLogger(  void )
{
    return m_pLogger;
}

} // end namespace igstk

