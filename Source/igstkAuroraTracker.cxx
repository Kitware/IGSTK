
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
  m_CommandInterpreter->SetCommunication( communication );
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
    m_CommandInterpreter->BEEP(2000);
  }
  else
  {
    m_CommandInterpreter->BEEP(200);
//  m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
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

  this->EnableToolPorts();
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

bool NDITracker::LoadVirtualSROM( const int tool, std::string SROMFileName) 
{
  FILE *file = fopen(SROMFileName.c_str(),"rb");
  if (file == NULL)
  {
    igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: couldn't find srom file ...\n");
    return false;
  }

  unsigned char data[1024];
  memset(data,0,1024);
  fread(data,1,1024,file);
  fclose(file);

  int errnum;
  int ph;
  int n, i;
  char hexbuffer[128];
  char location[14];

  if (tool < 3) // wired tools
  {
    m_CommandInterpreter->Command("PHSR:00");
    n = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (i = 0; i < n; i++)
    {
      if (m_CommandInterpreter->GetPHSRInformation(i) & NDI_TOOL_IN_PORT)
      {
        ph = m_CommandInterpreter->GetPHSRHandle(i);
        m_CommandInterpreter->Command("PHINF:%02X0021",ph);
        m_CommandInterpreter->GetPHINFPortLocation(location);
        if (tool == (location[10]-'0')*10 + (location[11]-'0') - 1)
        {
          break;
        }
      }
    }
    if (i == n)
    {
       igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: can't load SROM: no tool found in port ...\n");
      //vtkErrorMacro(<< "can't load SROM: no tool found in port " << tool);
      return false;
    }
  }
  else // wireless tools
  {
    m_CommandInterpreter->Command("PHRQ:**********0%c**", tool-3+'A');
    ph = m_CommandInterpreter->GetPHRQHandle();
  }
  errnum = m_CommandInterpreter->GetError();
  if (errnum)
  {
    igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
    igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    //vtkErrorMacro(<< ndiErrorString(errnum));
    return false;
  }

  for ( i = 0; i < 1024; i += 64)
  {
    m_CommandInterpreter->Command("PVWR:%02X%04X%.128s", ph, i, m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64));
  }  
  return true;
}



void NDITracker::EnableToolPorts()
{
  int errnum = 0;
  int tool;
  int ph;
  int port;
//  int info;
  int mode;
  int ntools;
  int status;
  char identity[34];
  char location[14];
//  char partNumber[24];

  // reset our information about the tool ports
  for (tool = 0; tool < NDI_MAX_TOOLS; tool++)
  {
    this->PortHandle[tool] = 0;
    this->PortEnabled[tool] = 0;
  }

  // stop tracking
  if (this->IsDeviceTracking)
  {
    m_CommandInterpreter->Command("TSTOP:");
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }    
  }

  // free ports that are waiting to be freed
  m_CommandInterpreter->Command("PHSR:01");
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    port = this->GetToolFromHandle(ph);
    m_CommandInterpreter->Command("PHF:%02X",ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }
  }

  // initialize ports waiting to be initialized
  do // repeat as necessary (in case multi-channel tools are used) 
  {
    m_CommandInterpreter->Command("PHSR:02");
    ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (tool = 0; tool < ntools; tool++)
    {
      ph = m_CommandInterpreter->GetPHSRHandle(tool);
      m_CommandInterpreter->Command("PINIT:%02X",ph);
      errnum = m_CommandInterpreter->GetError();
      if (errnum)
      { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
      }
    }
  }
  while (ntools > 0 && errnum == 0);

  // enable initialized tools
  m_CommandInterpreter->Command("PHSR:03");
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->Command("PHINF:%02X0001",ph);
    m_CommandInterpreter->GetPHINFToolInfo(identity);
    if (identity[1] == 0x03) // button-box
    {
      mode = 'B';
    }
    else if (identity[1] == 0x01) // reference
    {
      mode = 'S';
    }
    else // anything else
    {
      mode = 'D';
    }

    // enable the tool
    m_CommandInterpreter->Command("PENA:%02X%c",ph,mode);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    {
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }
  }

  // get information for all tools
  m_CommandInterpreter->Command("PHSR:00");
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->Command("PHINF:%02X0025",ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
      continue;
    }    
    // get the physical port identifier
    m_CommandInterpreter->GetPHINFPortLocation(location);
    if (location[11] >= 'A')
    {
      port = location[11] - 'A' + 3;
    }
    else
    {
      port = (location[10]-'0')*10 + (location[11]-'0') - 1;
    }
    if (port < NDI_MAX_TOOLS)
    {
      this->PortHandle[port] = ph;
    }

    // decompose identity string from end to front
    /*
    m_CommandInterpreter->GetPHINFToolInfo(identity);
    identity[31] = '\0';
    this->Tools[port]->SetToolSerialNumber(StripWhitespace(&identity[23]));
    identity[23] = '\0';
    this->Tools[port]->SetToolRevision(StripWhitespace(&identity[20]));
    identity[20] = '\0';
    this->Tools[port]->SetToolManufacturer(StripWhitespace(&identity[8]));
    identity[8] = '\0';
    this->Tools[port]->SetToolType(StripWhitespace(&identity[0]));

    m_CommandInterpreter->GetPHINFPartNumber(partNumber);
    partNumber[20] = '\0';
    this->Tools[port]->SetToolPartNumber(StripWhitespace(partNumber));
    */

    status = m_CommandInterpreter->GetPHINFPortStatus();
    this->PortEnabled[port] = ((status & NDI_ENABLED) != 0);
    
    /*
    if (this->Tools[port]->GetLED1())
    {
      this->InternalSetToolLED(tool,1,this->Tools[port]->GetLED1());
    }
    if (this->Tools[port]->GetLED2())
    {
      this->InternalSetToolLED(tool,2,this->Tools[port]->GetLED2());
    }
    if (this->Tools[port]->GetLED3())
    {
      this->InternalSetToolLED(tool,3,this->Tools[port]->GetLED3());
    }
    */
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    m_CommandInterpreter->Command("TSTART:");
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }
  }
}


int NDITracker::GetToolFromHandle(int handle)
{
  int tool;

  for (tool = 0; tool < NDI_MAX_TOOLS; tool++)
    {
    if (this->PortHandle[tool] == handle)
      {
      return tool;
      }
    }

  return -1;
}

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
