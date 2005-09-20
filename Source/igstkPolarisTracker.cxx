/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTracker.cxx
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

#include "igstkPolarisTracker.h"
#include "igstkTrackerPort.h"

namespace igstk
{

/** Some internal values that are used to describe tool states. */
enum {
  TR_MISSING       = 0x0001,  // tool or tool port is not available
  TR_OUT_OF_VIEW   = 0x0002,  // cannot obtain transform for tool
  TR_OUT_OF_VOLUME = 0x0004   // tool is not within the sweet spot of system
};


/** Constructor: Initializes all internal variables. */
PolarisTracker::PolarisTracker(void)
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (int i = 0; i < NDI_NUMBER_OF_PORTS; i++)
    {
    this->m_PortEnabled[i] = 0;
    this->m_PortHandle[i] = 0;
    }

  this->SetThreadingEnabled( true );
}

/** Destructor */
PolarisTracker::~PolarisTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the Polaris */
void PolarisTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "PolarisTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, "PolarisTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalOpen called ...\n");

  // Initialize the device 
  m_CommandInterpreter->INIT();

  // Reset and try again if error
  if (m_CommandInterpreter->GetError())
    {
    m_CommandInterpreter->RESET();
    m_CommandInterpreter->INIT();
    }

  // If it still failed to initialize, fail
  if (m_CommandInterpreter->GetError())
    {
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}

/** Close communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalClose( void )
{
  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::InternalClose: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalActivateTools called ...\n");

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
      PolarisTrackerToolPointer tool = PolarisTrackerToolType::New();
      TrackerPortPointer port = TrackerPortType::New();
      port->AddTool(tool);
      this->AddPort(port);
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalDeactivateTools( void )
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

/** Put the tracking device into tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}

/** Take the tracking device out of tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();
  int errnum = m_CommandInterpreter->GetError();
  if (errnum) 
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
PolarisTracker::ResultType PolarisTracker::InternalReset( void )
{
  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  // If it still failed to initialize, fail
  if (m_CommandInterpreter->GetError())
    {
    return FAILURE;
    }
  else
    {
    return SUCCESS;
    }
}


/** Update the status and the transforms for all TrackerTools. */
PolarisTracker::ResultType PolarisTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  int port;
  long flags; // flags for the device
  m_TrackingThreadLock->Lock();
  for (port = 0; port < NDI_NUMBER_OF_PORTS; port++) 
    {
    // convert m_StatusBuffer flags from NDI to vtkTracker format
    int portStatus = m_StatusBuffer[port];
    flags = 0;
    if ((portStatus & mflags) != mflags) 
      {
      flags |= TR_MISSING;
      }
    else
      {
      if (m_AbsentBuffer[port]) { flags |= TR_OUT_OF_VIEW;  }
      if (portStatus & CommandInterpreterType::NDI_OUT_OF_VOLUME)
        {
        flags |= TR_OUT_OF_VOLUME;
        }
      }
    // Send the transform to the tool:

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;
    translation[0] = m_TransformBuffer[port][4];
    translation[1] = m_TransformBuffer[port][5];
    translation[2] = m_TransformBuffer[port][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    rotation.Set(m_TransformBuffer[port][0],m_TransformBuffer[port][1],m_TransformBuffer[port][2],m_TransformBuffer[port][3]);

    // report NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_TransformBuffer[port][7];

    typedef TransformType::TimePeriodType TimePeriodType;
    TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port,0,transform);
    }
  m_TrackingThreadLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
PolarisTracker::ResultType PolarisTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalThreadedUpdateStatus called ...\n");

  int errnum; // error value from device
  int port;   // physical port number
  int ph;     // port handle reported by device
  unsigned long frame[NDI_NUMBER_OF_PORTS];

  // Initialize transformations to identity.
  // The NDI transform is 8 values:
  // the first 4 values are a quaternion
  // the next 3 values are an x,y,z position
  // the final value is an error estimate in the range [0,1]
  m_TrackingThreadLock->Lock();
  for (port = 0; port < NDI_NUMBER_OF_PORTS; port++)
    {
    m_TransformBuffer[port][0] = 1.0;
    m_TransformBuffer[port][1] = 0.0;
    m_TransformBuffer[port][2] = 0.0;
    m_TransformBuffer[port][3] = 0.0;
    m_TransformBuffer[port][4] = 0.0;
    m_TransformBuffer[port][5] = 0.0;
    m_TransformBuffer[port][6] = 0.0;
    m_TransformBuffer[port][7] = 0.0;
    }
  m_TrackingThreadLock->Unlock();

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);
  errnum = m_CommandInterpreter->GetError();

  if (errnum)
    {
    if (errnum == CommandInterpreterType::NDI_BAD_CRC ||
        errnum == CommandInterpreterType::NDI_TIMEOUT) // common errors
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
    m_TrackingThreadLock->Lock();
    ph = this->m_PortHandle[port];
    m_AbsentBuffer[port] = 0;
    m_StatusBuffer[port] = 0;
    m_TrackingThreadLock->Unlock();

    frame[port] = 0;
    if (ph == 0)
      {
      continue;
      }

    double transform[8];
    int status, absent;
    int result = m_CommandInterpreter->GetTXTransform(ph, transform);
    
    absent = (result != CommandInterpreterType::NDI_VALID);
    status = m_CommandInterpreter->GetTXPortStatus(ph);
    frame[port] = m_CommandInterpreter->GetTXFrame(ph);

    m_TrackingThreadLock->Lock();
    int i;
    for( i = 0; i < 8; ++i )
      {
      m_TransformBuffer[port][i] = transform[i];
      }
    m_AbsentBuffer[port] = absent;
    m_StatusBuffer[port] = status;
    m_TrackingThreadLock->Unlock();
  }

  // In the original vtkNDITracker code, there was a check at this
  // point in the code to see if any new tools had been plugged in

  return SUCCESS;
}

/** Specify an SROM file to be used with a passive or custom tool. */
void PolarisTracker::AttachSROMFileNameToPort( const int portNum,
                                              std::string fileName )
{
  if ((portNum>=0) && (portNum<=NDI_NUMBER_OF_PORTS))
    {
    m_SROMFileNames[portNum] = fileName;
    }
}

/** Load a virtual SROM, given the file name of the ROM file */
bool PolarisTracker::LoadVirtualSROM( const int tool, std::string SROMFileName) 
{
  FILE *file = fopen(SROMFileName.c_str(),"rb");
  if (file == NULL)
  {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: couldn't find srom file ...\n");
    return false;
  }

  unsigned char data[1024]; // holds contents of SROM file
  memset(data,0,1024);
  fread(data,1,1024,file);
  fclose(file);

  int errnum;          // NDI error code
  int ph;              // port handle
  int n, i;
  char hexbuffer[128]; // holds hexidecimal data to be sent to device
  char location[14];   // info about the location of a port

  if (tool < 3) // wired tools
    {
    m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);
    n = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (i = 0; i < n; i++)
      {
      if (m_CommandInterpreter->GetPHSRInformation(i) &
          CommandInterpreterType::NDI_TOOL_IN_PORT)
        {
        ph = m_CommandInterpreter->GetPHSRHandle(i);
        m_CommandInterpreter->PHINF(ph,
                                    CommandInterpreterType::NDI_BASIC |
                                    CommandInterpreterType::NDI_PORT_LOCATION);
        m_CommandInterpreter->GetPHINFPortLocation(location);
        if (tool == (location[10]-'0')*10 + (location[11]-'0') - 1)
          {
          break;
          }
        }
      }
    if (i == n)
      {
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: can't load SROM: no tool found in port ...\n");
      return false;
      }
    }
  else // wireless tools
    {
    // This is actually code for the POLARIS:
    // all tools past the first three assumed to be passive tools
    // identified by the letters "A", "B", "C", etc.
    char portIdentifier[3] = "00";
    portIdentifier[1] = tool-3+'A';

    m_CommandInterpreter->PHRQ("********", // device number
                               "*",        // TIU or SCU
                               "*",        // wired or wireless
                               portIdentifier,      // port
                               "**");      // channel
 
    ph = m_CommandInterpreter->GetPHRQHandle();
    }
  errnum = m_CommandInterpreter->GetError();
  if (errnum)
    {
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");

    return false;
    }

  for ( i = 0; i < 1024; i += 64)
    {
    // convert data to hexidecimal and write to virtual SROM in
    // 64-byte chunks
    m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64);
    m_CommandInterpreter->PVWR(ph, i, hexbuffer);
    }

  return true;
}

/** Clear a previously loaded SROM. */
void PolarisTracker::ClearVirtualSROM(int tool)
{
  int ph = this->m_PortHandle[tool];
  m_CommandInterpreter->PHF(ph);
  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = 0;
}

/** Enable all tool ports that are occupied. */
void PolarisTracker::EnableToolPorts()
{
  int errnum = 0;
  int tool;
  int ph;
  int port;
  int mode;
  int ntools;
  int status;
  char identity[34];
  char location[14];

  // reset our information about the tool ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
  {
    this->m_PortHandle[tool] = 0;
    this->m_PortEnabled[tool] = 0;
  }

  // free ports that are waiting to be freed
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
  {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    port = this->GetToolFromHandle(ph);
    m_CommandInterpreter->PHF(ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
    { 
    igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
    igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
    }
  }

  // initialize ports waiting to be initialized
  do // repeat as necessary (in case multi-channel tools are used) 
    {
    m_CommandInterpreter->PHSR(
      CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
    
    ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (tool = 0; tool < ntools; tool++)
      {
      ph = m_CommandInterpreter->GetPHSRHandle(tool);
      m_CommandInterpreter->PINIT(ph);
      errnum = m_CommandInterpreter->GetError();
      if (errnum)
        { 
        igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
        igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum)<<"\n");
        }
      }
    }
  while (ntools > 0 && errnum == 0);

  // enable initialized tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_BASIC);
    m_CommandInterpreter->GetPHINFToolInfo(identity);
    if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
      { // reference
      mode = CommandInterpreterType::NDI_STATIC;
      }
    else
      { // anything else
      mode = CommandInterpreterType::NDI_DYNAMIC;
      }

    // enable the tool
    m_CommandInterpreter->PENA(ph, mode);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      {
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }
    }

  // get information for all tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
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
    this->m_PortEnabled[port] = 
      ((status & CommandInterpreterType::NDI_ENABLED) != 0);
    }
}

/** Disable all tool ports. */
void PolarisTracker::DisableToolPorts( void )
{
  int errnum = 0;
  int ph;
  int tool;
  int ntools;

  // disable all enabled tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ENABLED_HANDLES);
  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  for (tool = 0; tool < ntools; tool++)
    {
    ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PDIS(ph);
    errnum = m_CommandInterpreter->GetError();
    if (errnum)
      { 
      igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM: Error ...\n");
      igstkLogMacro( DEBUG, m_CommandInterpreter->ErrorString(errnum) << "\n");
      }    
    }

  // disable the enabled ports
  for (tool = 0; tool < NDI_NUMBER_OF_PORTS; tool++)
    {
    this->m_PortEnabled[tool] = 0;
    }
}

/** Given an NDI tool handle, return the physical port number. */
int PolarisTracker::GetToolFromHandle(int handle)
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


/** Print Self function */
void PolarisTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
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
