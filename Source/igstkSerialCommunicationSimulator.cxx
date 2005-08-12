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

SerialCommunicationSimulator::SerialCommunicationSimulator()
{
  m_ResponseTable.clear();
  m_CounterTable.clear();
} 


SerialCommunicationSimulator::~SerialCommunicationSimulator()
{
} 


void SerialCommunicationSimulator::SetFileName(const char* filename)
{
  m_FileName.assign(filename);
} 


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalOpenPort( void )
{
  igstkLogMacro( DEBUG, m_FileName << std::endl );

  m_File.open(m_FileName.c_str(), std::ios::binary);

  if (!m_File.is_open()) 
    {
    m_File.clear();
    return FAILURE;
    }

  // read a command-to-response table from a file
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
        m_ResponseTable[BinaryData()].push_back(recvmsg);
        igstkLogMacro( DEBUG, "SERIAL BREAK ::: " << recvmsg << std::endl );
        }
      else if( sent == recv )
        {
        m_ResponseTable[sentmsg].push_back(recvmsg);
        igstkLogMacro( DEBUG, "sent " << sent << " : " << sentmsg << std::endl );
        igstkLogMacro( DEBUG, "recv " << recv << " : " << recvmsg << std::endl );
        }
      }
    }

  m_File.close();

  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalSetTransferParameters( void )
{
  igstkLogMacro( DEBUG, "SetCommunicationParameters succeeded.\n");
  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalClosePort( void )
{
  return SUCCESS;
}


void SerialCommunicationSimulator::InternalSendBreak( void )
{
  // The response table might have a response for a serial break,
  //  which we signify with an empty string
  m_Command = BinaryData();
}


void SerialCommunicationSimulator::InternalSleep( void )
{
  // Sleep isn't really needed during simulation, since
  //  responses are instantaneous
}


void SerialCommunicationSimulator::InternalPurgeBuffers( void )
{
  // Simulator does not support purging buffers.
}

void SerialCommunicationSimulator::InternalWrite( void )
{
  unsigned int bytesToWrite = m_BytesToWrite;

  igstkLogMacro( DEBUG, "InternalWrite called ...\n");

  // Just copy the data to m_Command for later use.
  m_Command.CopyFrom( (unsigned char*)&m_OutputData[0], bytesToWrite );

  igstkLogMacro( DEBUG, "Written bytes = " << bytesToWrite << std::endl);
  this->InvokeEvent( WriteSuccessEvent());
}


void SerialCommunicationSimulator::InternalRead( void )
{
  unsigned index = m_CounterTable[m_Command]++;
  const BinaryData& response = m_ResponseTable[m_Command][index];
  int bytesRead = response.GetSize();

  if( index+1 >= m_ResponseTable[m_Command].size() )
    {
    m_CounterTable[m_Command] = 0;
    }

  // Number of bytes read can't be greater than number asked for.
  bytesRead = ((m_BytesToRead < bytesRead) ? m_BytesToRead : bytesRead);

  // Only copy the specified number of characters
  for (int j = 0; j < bytesRead; j++)
    {
    ((unsigned char *)m_InputData)[j] = response[j];
    } 

  m_BytesRead = bytesRead;

  // Check whether to simulate a timeout
  if (bytesRead == 0 ||
      (m_UseReadTerminationCharacter && 
       m_InputData[bytesRead-1] != m_ReadTerminationCharacter) ||
      (!m_UseReadTerminationCharacter &&
       bytesRead < m_BytesToRead))
    {
    igstkLogMacro( DEBUG, "InternalRead failed with timeout...\n");
    this->InvokeEvent( ReadTimeoutEvent() );
    return;
    }

  igstkLogMacro( DEBUG, "Read number of bytes = " << bytesRead << ". Data: " );

  this->InvokeEvent( ReadSuccessEvent() );
}


/** Print Self function */
void SerialCommunicationSimulator::PrintSelf( std::ostream& os,
                                              itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "FileName: " << m_FileName << std::endl;
}

} // end namespace igstk

