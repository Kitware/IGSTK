
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForLinux.h"
#endif

#include "igstkAuroraTracker.h"



namespace igstk
{

NDITracker::NDITracker(void) : Tracker()
{
}

NDITracker::~NDITracker(void)
{
}

void NDITracker::AttemptToSetUpCommunicationProcessing( void )
{
  /*
  this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentFailureInput;
#ifdef WIN32
  m_pCommunication = igstk::SerialCommunicationForWindows::New();
#else
  m_pCommunication = igstk::SerialCommunicationForLinux::New();
#endif
  if (m_pCommunication)
    this->m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
    */
}


void NDITracker::ReadConfigurationFile( const char *file )
{
  NDIConfigurationReader  my_reader;
}

int NDIConfigurationReader::CanReadFile(const char* name) 
{
  return 1;
}

void NDIConfigurationReader::StartElement(const char * name,const char **atts) 
{
}

void NDIConfigurationReader::EndElement(const char *name) 
{
}

void NDIConfigurationReader::CharacterDataHandler(const char *inData, int inLength) 
{
}

}
