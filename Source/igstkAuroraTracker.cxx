
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
  m_CommandInterpreter = NDICommandInterpreter::New();
}


NDITracker::~NDITracker(void)
{
  //delete m_CommandInterpreter;
}


void NDITracker::SetCommunication( CommunicationType *communication )
{
  std::cout << "Entered ..." << std::endl;
  igstkLogMacro( Logger::DEBUG, "NDITracker:: Entered SetCommunication ...\n");
  Tracker::SetCommunication( communication );
  //m_CommandInterpreter->SetCommunication( communication );
  igstkLogMacro( Logger::DEBUG, "NDITracker:: Exiting SetCommunication ...\n"); 
}


void NDITracker::AttemptToSetUpCommunicationProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "NDITracker::AttemptToSetUpCommunicationProcessing called ...\n");
  // m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentFailureInput;

  // Initialize the device 
  const char *reply = m_CommandInterpreter->Command("INIT:");

  igstkLogMacro( Logger::DEBUG,  reply );
  igstkLogMacro( Logger::DEBUG, "...\n");

  if (m_CommandInterpreter->GetError())
  {
    m_CommandInterpreter->BEEP(200);
  }
  else
  {
//    m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
  }
}


void NDITracker::AttemptToSetUpToolsProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "NDITracker::AttemptToSetUpToolsProcessing called ...\n");
  //m_pActivateToolsResultInput = &(m_ToolsActivationFailureInput);

  // load any SROMS that are needed
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
  { 
    if (!m_SROMFileNames[i].empty())
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
  }
}



void NDITracker::AttachSROMFileNameToPort( const int portNum, std::string fileName )
{
  if ((portNum>=0) && (portNum<=NDI_NUMBER_OF_PORTS))
  {
    m_SROMFileNames[portNum] = fileName;
  }
}



void NDITracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "NDITracker::StopTrackingProcessing called ...\n");  
  //m_pStopTrackingResultInput = &(m_StopTrackingFailureInput);
/*
  m_VTKError = 0;
  m_VTKTracker->StopTracking();
  if (m_VTKError == 0)
    {
  //m_pStopTrackingResultInput = &(m_StopTrackingSuccessInput);
    }
*/
}


// Send a command to the tracking unit in TEXT MODE.
/*
void NDITracker::SendCommand(const char *command, bool addCRC)
{
  // FIXME: Mutex lock/unlock may be needed

  int commandLength = strlen(command);

  strncpy(m_CommandBuffer, command, commandLength);

  if (addCRC && (commandLength<=(NDI_COMMAND_MAX_LEN-6)))
  {
    unsigned int crc = 0;
    for( int m=0; m<commandLength; m++)
    {
      // replace space character with :
      if(command[m]==' ') command[m]=':';
      crc = m_CyclicRedundancy.ComputeCyclicRedundancyOfTextData(crc,command[m]);
    }
    sprintf(&command[commandLength],"%04X",crc);
  }

  m_Communication->SendString( command );
  m_Communication->Flush();
}
*/


//void NDITracker::ReadConfigurationFile( const char *file )
//{
//  NDIConfigurationReader  my_reader;
//}

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
