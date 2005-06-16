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

#include "igstkAuroraTracker.h"
#include "igstkTrackerPort.h"

namespace igstk
{

AuroraTracker::AuroraTracker(void) : Tracker()
{
  m_CommandInterpreter = NDICommandInterpreter::New();
  m_NumberOfTools = 0;
}


AuroraTracker::~AuroraTracker(void)
{
}


void AuroraTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "AuroraTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );
  igstkLogMacro( DEBUG, "AuroraTracker:: Exiting SetCommunication ...\n"); 
}


AuroraTracker::ResultType AuroraTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalOpen called ...\n");

  // Initialize the device 
  const char *reply = m_CommandInterpreter->Command("INIT:");

  igstkLogMacro( DEBUG,  reply << "...\n");

  if (m_CommandInterpreter->GetError())
    {
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}


AuroraTracker::ResultType AuroraTracker::InternalClose( void )
{
  // return to default comm settings
  m_CommandInterpreter->Command("COMM:00000");
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
  {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
  }
  //ndiClose(this->Device); To be done by the application
  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalActivateTools called ...\n");

  // load any SROMS that are needed
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
      }
    }

  this->EnableToolPorts();

  this->ClearPorts();

  m_NumberOfTools = 0;

  for(int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      AuroraToolPointer tool = AuroraToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalDeactivateTools( void )
{
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->ClearVirtualSROM(i);
      }
    }

  this->DisableToolPorts();

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->Command("TSTART:");

  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}


AuroraTracker::ResultType AuroraTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->Command("TSTOP:");
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalReset( void )
{
  return SUCCESS;
}


AuroraTracker::ResultType AuroraTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalUpdateStatus called ...\n");

  int errnum, port, ph;
  int status[NDI_NUMBER_OF_PORTS];
  int absent[NDI_NUMBER_OF_PORTS];
  unsigned long frame[NDI_NUMBER_OF_PORTS];
  double transform8[NDI_NUMBER_OF_PORTS][8];
  long flags;
  const unsigned long mflags = NDI_TOOL_IN_PORT | NDI_INITIALIZED | NDI_ENABLED;

  // initialize transformations to identity
  for (int port = 0; port < NDI_NUMBER_OF_PORTS; port++)
    {
    transform8[port][0] = 1.0;
    transform8[port][1] = 0.0;
    transform8[port][2] = 0.0;
    transform8[port][3] = 0.0;
    transform8[port][4] = 0.0;
    transform8[port][5] = 0.0;
    transform8[port][6] = 0.0;
    transform8[port][7] = 0.0;
    }

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->Command("TX:0001");
  errnum = m_CommandInterpreter->GetError();

  if (errnum)
    {
    if (errnum == NDI_BAD_CRC || errnum == NDI_TIMEOUT) // common errors
      {
      igstkLogMacro( WARNING, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    else
      {
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
      }
    return FAILURE;
    }

  // default to incrementing frame count by one (in case there are
  // no transforms for any tools)
  unsigned long nextcount = 0;

  for (port = 0; port < NDI_NUMBER_OF_PORTS; port++)
    {
    ph = this->m_PortHandle[port];
    if (ph == 0)
      {
      continue;
      }

    absent[port] = m_CommandInterpreter->GetTXTransform(ph, transform8[port]);
    status[port] = m_CommandInterpreter->GetTXPortStatus(ph);
    frame[port] = m_CommandInterpreter->GetTXFrame(ph);
  }

  /*
  // check to see if any tools have been plugged in
  if (m_CommandInterpreter->GetTXSystemStatus() & NDI_PORT_OCCUPIED)
    { // re-configure, a new tool has been plugged in
    this->m_CommandInterpreter->Command("TSTOP:");
    this->EnableToolPorts();
    this->m_CommandInterpreter->Command("TSTART:");
    }
  else
    {
    for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
      {
      this->m_PortEnabled[tool] = ((status[tool] & mflags) == mflags);
      }
    }
  */

  for (int port = 0; port < NDI_NUMBER_OF_PORTS; port++) 
    {
    // convert status flags from NDI to vtkTracker format
    int port_status = status[port];
    flags = 0;
    if ((port_status & mflags) != mflags) 
      {
      flags |= TR_MISSING;
      }
    else
      {
      if (absent[port]) { flags |= TR_OUT_OF_VIEW;  }
      if (port_status & NDI_OUT_OF_VOLUME){ flags |= TR_OUT_OF_VOLUME; }
//      if (port_status & NDI_SWITCH_1_ON)  { flags |= TR_SWITCH1_IS_ON; }
//      if (port_status & NDI_SWITCH_2_ON)  { flags |= TR_SWITCH2_IS_ON; }
//      if (port_status & NDI_SWITCH_3_ON)  { flags |= TR_SWITCH3_IS_ON; }
      }

    // send the transform to the tool

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;
    translation[0] = transform8[port][4];
    translation[1] = transform8[port][5];
    translation[2] = transform8[port][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    rotation.Set(transform8[port][0],transform8[port][1],transform8[port][2],transform8[port][3]);

    // report NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = transform8[port][7];

    typedef TransformType::TimePeriodType TimePeriodType;
    TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port,0,transform);
    }

  return SUCCESS;
}


void AuroraTracker::AttachSROMFileNameToPort( const int portNum, std::string fileName )
{
  if ((portNum>=0) && (portNum<=NDI_NUMBER_OF_PORTS))
    {
    m_SROMFileNames[portNum] = fileName;
    }
}


bool AuroraTracker::LoadVirtualSROM( const int tool, std::string SROMFileName) 
{
  FILE *file = fopen(SROMFileName.c_str(),"rb");
  if (file == NULL)
  {
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: couldn't find srom file ...\n");
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
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: can't load SROM: no tool found in port ...\n");
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
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return false;
    }

  for ( i = 0; i < 1024; i += 64)
    {
    m_CommandInterpreter->Command("PVWR:%02X%04X%.128s", ph, i, m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64));
    }  
  return true;
}


void AuroraTracker::EnableToolPorts()
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
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
  {
    this->m_PortHandle[tool] = 0;
    this->m_PortEnabled[tool] = 0;
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
    igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
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
        igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
        igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
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
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
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
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
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
    if (port < NDI_NUMBER_OF_PORTS)
      {
      this->m_PortHandle[port] = ph;
      }

    status = m_CommandInterpreter->GetPHINFPortStatus();
    this->m_PortEnabled[port] = ((status & NDI_ENABLED) != 0);
    
    }
}


void AuroraTracker::DisableToolPorts( void )
{
  int errnum = 0;
  int ph;
  int tool;
  int ntools;

  // disable all enabled tools
  m_CommandInterpreter->Command("PHSR:04");
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->Command("PDIS:%02X",ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }    
    }

  // disable the enabled ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    this->m_PortEnabled[tool] = 0;
    }
}


int AuroraTracker::GetToolFromHandle(int handle)
{
  int tool;

  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    if (this->m_PortHandle[tool] == handle)
      {
      return tool;
      }
    }
  return -1;
}


void AuroraTracker::ClearVirtualSROM(int tool)
{
  int ph = this->m_PortHandle[tool];
  m_CommandInterpreter->Command("PHF:%02X", ph);
  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = 0;
}


/** Print Self function */
void AuroraTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  int i;
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] << std::endl;
    }
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "Port " << i << " Handle: " << m_PortHandle[i] << std::endl;
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
  for( i = 0; i < NDI_NUMBER_OF_PORTS; ++i )
    {
    os << indent << "SROM filename " << i << " : " << m_SROMFileNames[i] << std::endl;
    }
}


}
