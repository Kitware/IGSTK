/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <algorithm>
#include <string.h>

#include "igstkSerialCommunicationSimulator.h"


namespace igstk
{ 
/** Constructor */
SerialCommunicationSimulator::SerialCommunicationSimulator() : 
SerialCommunication()
{
  m_ResponseTable.clear();
} 


SerialCommunicationSimulator::~SerialCommunicationSimulator()
{
  if( m_File.is_open() )
    {
    m_File.close();
    }
} 


void
SerialCommunicationSimulator::SetFileName(const char* filename)
{
  m_FileName.assign(filename);
} 


SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalOpenCommunication( void )
{
  igstkLogMacro( DEBUG, m_FileName << std::endl );

  m_File.open(m_FileName.c_str(), std::ios::binary);

  if (!m_File.is_open()) 
    {
    return FAILURE;
    }

  BinaryData recvmsg, sentmsg;
  unsigned char buf[64*1024];
  char numberString[20], temp[20], byte;
  int sent, recv, len, numberLength, number;
  sent = recv = 0;
  while( !m_File.eof() )
    {
    m_File.read(&byte, 1);
    if( byte == '#' )
      {
      m_File.getline((char*)buf, 64*1024);
      continue;
      }

    if( m_File.eof() )
      {
      break;
      }
    numberLength = 0;
    numberString[numberLength++] = byte;
    while( byte != '.' )
      {
      m_File.read(&byte, 1);
      numberString[numberLength++] = byte;
      }
    numberString[numberLength] = 0;
    
    number = atoi(numberString);

    m_File.read(&byte, 1);  // to consume a space
    m_File.read(temp, 7);
    temp[7] = 0;

    m_File.read(&byte, 1);  // to consume [
    numberLength = 0;
    while( byte != ']' )
      {
      m_File.read(&byte, 1);
      numberString[numberLength++] = byte;
      }
    numberString[numberLength] = 0;
    len = atoi(numberString);
    m_File.read(&byte, 1);  // to consume a space
    m_File.read((char*)buf, len);
    buf[len] = 0;
    m_File.read(&byte, 1);  // to consume a newline

    if( strncmp("command", temp, 7) == 0 )
      {
      sent = number;
      sentmsg.CopyFrom(&buf[0], len);
      }
    else if( strncmp("receive", temp, 7) == 0 )
      {
      recv = number;
      recvmsg.CopyFrom(&buf[0], len);
      if( sent < recv )
        {
        m_ResponseTable[BinaryData()] = recvmsg;
        igstkLogMacro( DEBUG, "SERIAL BREAK ::: " << recvmsg << std::endl );
        }
      else if( sent == recv )
        {
        m_ResponseTable[sentmsg] = recvmsg;
        igstkLogMacro( DEBUG, "sent " << sent << " : " << sentmsg << std::endl );
        igstkLogMacro( DEBUG, "recv " << recv << " : " << recvmsg << std::endl );
        }
      }
    }

  m_File.close();
  return SUCCESS;
}


SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalSetUpDataBuffers( void )
{
  if (this->m_InputBuffer!=NULL) delete (this->m_InputBuffer); 
  this->m_InputBuffer = new char[ this->m_ReadBufferSize ];
  if (this->m_OutputBuffer!=NULL) delete this->m_OutputBuffer; 
  this->m_OutputBuffer = new char[ this->m_WriteBufferSize + 1 ]; // one extra byte to store end of string
  if ((this->m_InputBuffer==NULL) || (this->m_OutputBuffer==NULL) )
  {
    this->InvokeEvent( SetDataBufferSizeFailureEvent() );
    return FAILURE;
  }
  else
  {
    this->m_ReadDataSize = 0;
    this->m_ReadBufferOffset = 0;
    memset(this->m_InputBuffer, '\0', sizeof(this->m_InputBuffer));

    igstkLogMacro( DEBUG, "SetDataBufferSizeParameters with Read Buffer size = " << m_ReadBufferSize << " and Write Buffer Size = " << m_WriteBufferSize << " succeeded.\n");
    return SUCCESS;
  }
}


SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalSetTransferParameters( void )
{
  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n");
  return SUCCESS;
}

SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalClearBuffersAndClosePort( void )
{
  if (m_InputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_InputBuffer;
  m_InputBuffer = NULL;
  if (m_OutputBuffer!= NULL) // This check not required, still keeping for safety
    delete m_OutputBuffer;
  m_OutputBuffer = NULL;
  return this->InternalClosePort();
}

SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalClosePort( void )
{
  if ( m_File.is_open() )
    {
    m_File.close();
    }
  return SUCCESS;
}

SerialCommunicationSimulator::ResultType SerialCommunicationSimulator::InternalSetTimeoutPeriod( int milliseconds )
{
  return SUCCESS;
}

void SerialCommunicationSimulator::InternalSendBreak( void )
{
  const BinaryData& response = this->m_ResponseTable[BinaryData()];
  response.CopyTo( (unsigned char*)&this->m_InputBuffer[0] );

  return;
}


void SerialCommunicationSimulator::InternalFlushOutputBuffer( void )
{
  return;
}


void SerialCommunicationSimulator::InternalWrite( void )
{
  unsigned long  bytesToWrite = m_WriteNumberOfBytes;

  igstkLogMacro( DEBUG, "InternalWrite called ...\n");

  BinaryData output;
  output.CopyFrom( (unsigned char*)&this->m_OutputBuffer[0], bytesToWrite );
  const BinaryData& response = this->m_ResponseTable[output];
  response.CopyTo( (unsigned char*)&this->m_InputBuffer[0] );
  this->m_ReadDataSize = response.GetSize();
  igstkLogMacro( DEBUG, "Written bytes = " << bytesToWrite << std::endl);
  this->InvokeEvent( WriteSuccessEvent());

  return;  /* return the number of characters written */
}


void SerialCommunicationSimulator::InternalRead( void )
{
  int i = m_ReadDataSize;
  
  if (i == 0)
    { // no characters read, must have timed out
    igstkLogMacro( DEBUG, "InternalRead failed with timeout...\n");
    this->InvokeEvent( ReadTimeoutEvent() );
    return;
    }

  this->m_ReadDataSize = i;
  this->m_ReadBufferOffset = 0;
  this->m_InputBuffer[i] = 0;
  igstkLogMacro( DEBUG, "Read number of bytes = " << i << ". Data: " );
  for( i = 0; i < this->m_ReadDataSize; ++i )
    {
    std::cout << this->m_InputBuffer[i];
    }
  std::cout << std::endl;
  this->InvokeEvent( ReadSuccessEvent() );
  return;
}


/** Print Self function */
void SerialCommunicationSimulator::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "File: " << m_File << std::endl;
}

} // end namespace igstk

