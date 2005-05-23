
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

NDITracker::NDITracker(void) : Tracker()
{
  this->m_CommandInterpreter = NDICommandInterpreter::New();
  this->IsDeviceTracking = 0;
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

  this->ClearPorts();

  for(int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
  { 
    if( this->PortEnabled[i] )
    {
      AuroraToolPointer tool = AuroraToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
    }
  }
}



void NDITracker::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "NDITracker::StartTrackingProcessing called ...\n");  
  m_CommandInterpreter->Command("TSTART:");
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
  {
    igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
    igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
  }
  else
  {
    this->IsDeviceTracking = 1;
  }
}



void NDITracker::UpdateStatusProcessing( void )
{
  this->InternalUpdate();
}


void NDITracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "NDITracker::StopTrackingProcessing called ...\n");  
  //m_pStopTrackingResultInput = &(m_StopTrackingFailureInput);

  m_CommandInterpreter->Command("TSTOP:");
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
  {
    igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
    igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    return;
  }
  //m_pStopTrackingResultInput = &(m_StopTrackingSuccessInput);
  this->IsDeviceTracking = 0;
}


void NDITracker::DisableToolsProcessing( void )
{
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
  { 
    if (!m_SROMFileNames[i].empty())
    {
      this->ClearVirtualSROM(i);
    }
  }

  this->DisableToolPorts();
}



void NDITracker::DisableCommunicationProcessing( void )
{
  // return to default comm settings
  m_CommandInterpreter->Command("COMM:00000");
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
  {
    igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
    igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
  }
  //ndiClose(this->Device); To be done by the application
}



void NDITracker::AttachSROMFileNameToPort( const int portNum, std::string fileName )
{
  if ((portNum>=0) && (portNum<=NDI_NUMBER_OF_PORTS))
  {
    m_SROMFileNames[portNum] = fileName;
  }
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
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
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
    if (port < NDI_NUMBER_OF_PORTS)
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


void NDITracker::DisableToolPorts( void )
{
  int errnum = 0;
  int ph;
  int tool;
  int ntools;

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

  // disable all enabled tools
  m_CommandInterpreter->Command("PHSR:04");
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->Command("PDIS:%02X",ph);
    //fprintf(stderr,"PDIS:%02X\n",ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
      igstkLogMacro( Logger::DEBUG, "NDITracker::LoadVirtualSROM: Error ...");
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }    
  }

  // disable the enabled ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
  {
    this->PortEnabled[tool] = 0;
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

  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    if (this->PortHandle[tool] == handle)
      {
      return tool;
      }
    }

  return -1;
}



void NDITracker::ClearVirtualSROM(int tool)
{
  int ph = this->PortHandle[tool];
  m_CommandInterpreter->Command("PHF:%02X", ph);
  this->PortEnabled[tool] = 0;
  this->PortHandle[tool] = 0;
}


void NDITracker::InternalUpdate()
{
  int errnum, port, ph;
  int status[NDI_NUMBER_OF_PORTS];
  int absent[NDI_NUMBER_OF_PORTS];
  unsigned long frame[NDI_NUMBER_OF_PORTS];
  double NDItransform[NDI_NUMBER_OF_PORTS][8];
  long flags;
  const unsigned long mflags = NDI_TOOL_IN_PORT | NDI_INITIALIZED | NDI_ENABLED;

  // FIXME: TO Remove this check 

  if (!this->IsDeviceTracking)
  {
    igstkLogMacro( Logger::DEBUG, "called Update() when NDI was not tracking");
    return;
  }

  // initialize transformations to identity
  // FIXME: May be, a memset could be used below for initialization.
  for (int port = 0; port < NDI_NUMBER_OF_PORTS; port++)
  {
    NDItransform[port][0] = 1.0;
    NDItransform[port][1] = NDItransform[port][2] = NDItransform[port][3] = 0.0;
    NDItransform[port][4] = NDItransform[port][5] = NDItransform[port][6] = 0.0;
    NDItransform[port][7] = 0.0;
  }

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->Command("TX:0001");
  errnum = m_CommandInterpreter->GetError();

  if (errnum)
  {
    if (errnum == NDI_BAD_CRC || errnum == NDI_TIMEOUT) // common errors
    {
      igstkLogMacro( Logger::WARNING, m_CommandInterpreter->ErrorString(errnum));
    }
    else
    {
      igstkLogMacro( Logger::DEBUG, m_CommandInterpreter->ErrorString(errnum));
    }
    return;
  }

  // default to incrementing frame count by one (in case there are
  // no transforms for any tools)
  unsigned long nextcount = 0;

  for (port = 0; port < NDI_NUMBER_OF_PORTS; port++)
  {
    ph = this->PortHandle[port];
    if (ph == 0)
    {
      continue;
    }

    absent[port] = m_CommandInterpreter->GetTXTransform(ph, NDItransform[port]);
    status[port] = m_CommandInterpreter->GetTXPortStatus(ph);
    frame[port] = m_CommandInterpreter->GetTXFrame(ph);
    /*
    if (!absent[tool] && frame[tool] > nextcount)
    { // 'nextcount' is max frame number returned
      nextcount = frame[tool];
    }
    */
  }

  // if no transforms were returned, advance frame count by 1
  // (assume the NDI will be returning the empty records at
  // its maximum reporting rate of 60Hz)
  /*
  if (nextcount == 0)
  {
    nextcount = this->Timer->GetLastFrame() + 1;
  }

  // the timestamp is always created using the frame number of
  // the most recent transformation
  this->Timer->SetLastFrame(nextcount);
  double timestamp = this->Timer->GetTimeStampForFrame(nextcount);
  */

  /*
  // check to see if any tools have been plugged in
  if (m_CommandInterpreter->GetTXSystemStatus() & NDI_PORT_OCCUPIED)
  { // re-configure, a new tool has been plugged in
    this->EnableToolPorts();
  }
  else
  {
    for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
      this->PortEnabled[tool] = ((status[tool] & mflags) == mflags);
    }
  }
  */

  /*
  if (this->ReferenceTool >= 0)
  { // copy reference tool transform
    referenceTransform = transform[this->ReferenceTool];
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

    /*
    // if tracking relative to another tool
    if (this->ReferenceTool >= 0 && tool != this->ReferenceTool)
    {
      if (!absent[tool])
      {
        if (absent[this->ReferenceTool])
        {
          flags |= TR_OUT_OF_VIEW;
        }
        if (status[this->ReferenceTool] & NDI_OUT_OF_VOLUME)
        {
          flags |= TR_OUT_OF_VOLUME;
        }
      }
      // pre-multiply transform by inverse of relative tool transform
      m_CommandInterpreter->RelativeTransform(transform[tool],referenceTransform,transform[tool]);
    }

    m_CommandInterpreter->TransformToMatrixd(transform[tool],*this->SendMatrix->Element);
    this->SendMatrix->Transpose();
    */

    // by default (if there is no camera frame number associated with
    // the tool transformation) the most recent timestamp is used.
    /*
    double tooltimestamp = timestamp;
    if (!absent[tool] && frame[tool])
    {
      // this will create a timestamp from the frame number      
      tooltimestamp = this->Timer->GetTimeStampForFrame(frame[tool]);
    }
    */
    // send the matrix and flags to the tool

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;
    translation[0] = NDItransform[port][4];
    translation[1] = NDItransform[port][5];
    translation[2] = NDItransform[port][6]; // + 1900; // correction for origin of Polaris in mm.

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    rotation.Set(NDItransform[port][0],NDItransform[port][1],NDItransform[port][2],NDItransform[port][3]);

    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.5; // actually it varies

    typedef TransformType::TimePeriodType TimePeriodType;
    TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port,0,transform);

 //   this->ToolUpdate(tool,this->SendMatrix,flags,tooltimestamp);
  }
}


}
