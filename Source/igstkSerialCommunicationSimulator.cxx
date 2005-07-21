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

  std::string recvmsg, sentmsg;
  char buf[1024];
  char temp[5];
  unsigned sent, recv, len;
  sent = recv = 0;
  while( !m_File.eof() )
    {
    m_File.getline(buf, 1023, '\r');
    m_File.getline(temp, 4, '\n');
    len = strlen(buf);
    buf[len] = '\r';
    buf[len+1] = 0;

    memcpy(temp, buf, 4);
    temp[4] = 0;

    if( strncmp( temp, "sent", 4 ) == 0 )
      {
      memcpy(temp, &buf[5], 4);
      temp[4] = 0;
      sscanf(temp, "%04d", &sent);
      sentmsg.assign(&buf[12]);
      }
    else if( strncmp( temp, "recv", 4 ) == 0 )
      {
      memcpy(temp, &buf[5], 4);
      temp[4] = 0;
      sscanf(temp, "%04d", &recv);
      recvmsg.assign( &buf[12] );
      if( sent < recv )
        {
        m_ResponseTable[std::string("")] = recvmsg;
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
  strcpy( this->m_InputBuffer, this->m_ResponseTable[std::string("")].c_str() );
  this->m_ReadDataSize = strlen(this->m_InputBuffer);
  this->m_ReadBufferOffset = 0;

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

  strcpy( this->m_InputBuffer, this->m_ResponseTable[this->m_OutputBuffer].c_str() );
  igstkLogMacro( DEBUG, "Written bytes = " << bytesToWrite << std::endl);
  this->InvokeEvent( WriteSuccessEvent());

  return;  /* return the number of characters written */
}


void SerialCommunicationSimulator::InternalRead( void )
{
  int i = 0;
  unsigned long m;
  int n = this->m_ReadNumberOfBytes;
  
  while (n > 0) {
    m = strlen(this->m_InputBuffer);
    n -= m;  /* n is number of chars left to read */
    i += m;  /* i is the number of chars read */
    if ( this->m_UseReadTerminationCharacter )
      {
      if (this->m_InputBuffer[i-1] == this->m_ReadTerminationCharacter ) 
        {  /* done when ReadTerminationCharacter received */
        break;
        }
      }
  }

  this->m_ReadDataSize = i;
  this->m_ReadBufferOffset = 0;
  this->m_InputBuffer[i] = 0;
  igstkLogMacro( DEBUG, "Read number of bytes = " << i << ". String: " << this->m_InputBuffer << std::endl );
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

