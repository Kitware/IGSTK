/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>

#include "igstkSerialCommunicationSimulator.h"
#include "igstkPulseGenerator.h"


namespace igstk
{ 

SerialCommunicationSimulator::SerialCommunicationSimulator()
                                                        :m_StateMachine(this)
{
  m_ResponseTable.clear();
  m_CounterTable.clear();
  m_TimeTable.clear();
} 


SerialCommunicationSimulator::~SerialCommunicationSimulator()
{
} 


void SerialCommunicationSimulator::SetFileName(const char* filename)
{
  m_FileName.assign(filename);
} 

const char *SerialCommunicationSimulator::GetFileName() const
{
  return m_FileName.c_str();
} 

SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalOpenPort( void )
{
  igstkLogMacro( DEBUG, m_FileName << "\n" );

  m_File.open(m_FileName.c_str());

  if (!m_File.is_open()) 
    {
    m_File.clear();
    return FAILURE;
    }

  // read a command-to-response table from a file
  BinaryData recvmsg, sentmsg;
  unsigned char buf[64*1024];
  char numberString[20], temp[20], byte;
  unsigned int len, numberLength, number;
  std::string encodedString = "";
  std::string encodedString0;
  double timestamp0;
  double timestamp = 0.0;
  int sent = -1;
  int recv = 0;
  while( !m_File.eof() )
    {
    timestamp0 = timestamp; // save previous timestamp
    m_File >> timestamp;
    m_File >> byte;       // consuming #
    m_File >> temp;
    if( strncmp("(DEBUG)", temp, 7) == 0 )
      {
      m_File.getline((char*)buf, 64*1024);
      
      continue;
      }

    if( m_File.eof() )
      {
      break;
      }
    m_File.read(&byte, 1);  // consuming ' '(white space)
    numberLength = 0;
    do {
      m_File.read(&byte, 1);
      numberString[numberLength++] = byte;
      }
    while( byte != '.' );
    numberString[numberLength-1] = 0;
    
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
    numberString[numberLength-1] = 0;
    len = atoi(numberString);
    m_File.read(&byte, 1);  // to consume a space
    m_File.getline((char*)buf, 64*1024);
    len = strlen((const char*)buf);
    if( buf[len-1] == '\r' || buf[len-1] == '\n' )
      {
      buf[len-1] = '\0';
      }
    len = strlen((const char*)buf);
    encodedString0 = encodedString; // save the previous string
    encodedString.assign((const char*)buf);

    if( strncmp("command", temp, 7) == 0 )
      {
      sent = number;
      sentmsg.Decode(encodedString);
      }
    else if( strncmp("receive", temp, 7) == 0 )
      {
      recv = number;
      recvmsg.Decode(encodedString);
      if( sent < recv )
        {
        m_ResponseTable[BinaryData()].push_back(recvmsg);
        m_TimeTable[BinaryData()].push_back(timestamp - timestamp0);
        igstkLogMacro( DEBUG, "Sim File: sent " << " : "
                       << " <<SERIAL BREAK>>\n");
        igstkLogMacro( DEBUG, "Sim File: recv " << recv << " : "
                       << encodedString << "\n" );
        igstkLogMacro( DEBUG, "Sim File: time " << recv << " : "
                       << (timestamp - timestamp0) << " seconds\n" );
        }
      else if( sent == recv )
        {
        m_ResponseTable[sentmsg].push_back(recvmsg);
        m_TimeTable[sentmsg].push_back(timestamp - timestamp0);
        igstkLogMacro( DEBUG, "Sim File: sent " << sent << " : "
                       << encodedString0 << "\n" );
        igstkLogMacro( DEBUG, "Sim File: recv " << recv << " : "
                       << encodedString << "\n" );
        igstkLogMacro( DEBUG, "Sim File: time " << recv << " : "
                       << (timestamp - timestamp0) << " seconds\n" );
        }
      }
    }

  m_File.close();

  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalUpdateParameters( void )
{
  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalClosePort( void )
{
  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalSendBreak( void )
{
  // The response table might have a response for a serial break,
  //  which we signify with an empty string
  m_Command = BinaryData();
  return SUCCESS;
}

void SerialCommunicationSimulator::InternalSleep( unsigned int milliseconds )
{
  PulseGenerator::Sleep( milliseconds );
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalPurgeBuffers( void )
{
  // Simulator does not support purging buffers.
  return SUCCESS;
}

SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalWrite( const char *data,
                                             unsigned int bytesToWrite )
{
  igstkLogMacro( DEBUG, "InternalWrite called ...\n");

  // Just copy the data to m_Command for later use.
  m_Command.CopyFrom( (unsigned char*)&data[0], bytesToWrite );

  igstkLogMacro( DEBUG, "Written bytes = " << bytesToWrite << "\n");
  return SUCCESS;
}


SerialCommunicationSimulator::ResultType
SerialCommunicationSimulator::InternalRead( char *data,
                                            unsigned int bytesToRead,
                                            unsigned int &bytesRead )
{
  unsigned index = m_CounterTable[m_Command]++;
  char terminationCharacter = this->GetReadTerminationCharacter();
  bool useTerminationCharacter = this->GetUseReadTerminationCharacter();
  
  if( m_ResponseTable[m_Command].size() == 0 )
    {
    bytesRead = 0;
    igstkLogMacro( DEBUG, "InternalRead failed with timeout...\n");
    return TIMEOUT;
    }
  const BinaryData& response = m_ResponseTable[m_Command][index];
  double responseTime = m_TimeTable[m_Command][index];
  bytesRead = response.GetSize();

  if( index+1 >= m_ResponseTable[m_Command].size() )
    {
    m_CounterTable[m_Command] = 0;
    }

  // Number of bytes read can't be greater than number asked for.
  bytesRead = ((bytesToRead < bytesRead) ? bytesToRead : bytesRead);

  // Only copy the specified number of characters
  for (unsigned int j = 0; j < bytesRead; j++)
    {
    ((unsigned char *)data)[j] = response[j];
    } 

  // Check whether to simulate a timeout
  if (bytesRead == 0 ||
      (useTerminationCharacter && 
       data[bytesRead-1] != terminationCharacter) ||
      (!useTerminationCharacter &&
       bytesRead < bytesToRead))
    {
    // to be realistic, sleep for the timeout period before returning
    this->InternalSleep(this->GetTimeoutPeriod());
    igstkLogMacro( DEBUG, "InternalRead failed with timeout...\n");
    return TIMEOUT;
    }

  // to be realistic, sleep according to the response times in the file
  unsigned int sleepTime = 1 + bytesRead/10; // default value
  if (responseTime > 0.0 && responseTime < 10.0) // 10 secs max
    {
    // convert responseTime to milliseconds, and add 1 millisecond
    sleepTime = (unsigned int)(responseTime * 1000) + 1;
    }
  this->InternalSleep(sleepTime);

  igstkLogMacro( DEBUG, "Read number of bytes = " << bytesRead << "\n" );

  return SUCCESS;
}


/** Print Self function */
void SerialCommunicationSimulator::PrintSelf( std::ostream& os,
                                              itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "FileName: " << m_FileName << std::endl;
}

} // end namespace igstk
