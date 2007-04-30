/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.cxx
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
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAuroraTracker.h"
#include "igstkTrackerPort.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
AuroraTracker::AuroraTracker(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    TrackerPortPointer tport = TrackerPortType::New();
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      AuroraTrackerToolPointer tool = AuroraTrackerToolType::New();
      tport->AddTool(tool);
      this->m_PortEnabled[port][channel] = 0;
      this->m_PortHandle[port][channel] = 0;
      }
    this->AddPort(tport);
    }

  this->SetThreadingEnabled( true );

  // lock for the thread buffer
  m_BufferLock = itk::MutexLock::New();

  // the baud rate is changed if SetCommunication is called
  m_BaudRate = CommunicationType::BaudRate9600;
}

/** Destructor */
AuroraTracker::~AuroraTracker(void)
{
}

/** Helper function for reporting interpreter errors. */
AuroraTracker::ResultType
AuroraTracker::CheckError(CommandInterpreterType *interpreter)
{
  const int errnum = interpreter->GetError();
  if (errnum)
    {
    // convert errnum to a hexadecimal string
    itk::OStringStream os;
    os << "0x";
    os.width(2);
    os.fill('0');
    os << std::hex << std::uppercase << errnum;
    igstkLogMacro( WARNING, "Aurora Error " << os.str() << ": " <<
                   interpreter->ErrorString(errnum) << "\n");

    igstkLogMacro( WARNING, interpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Set the communication object, it will be initialized as necessary
  * for use with the Aurora */
void AuroraTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "AuroraTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_BaudRate = communication->GetBaudRate();
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, "AuroraTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
AuroraTracker::ResultType AuroraTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalOpen called ...\n");

  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  ResultType result = this->CheckError(m_CommandInterpreter);

  if (result == SUCCESS)
    {
    // log information about the device
    m_CommandInterpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE);
    result = this->CheckError(m_CommandInterpreter);
    }

  if (result == SUCCESS)
    {
    // increase the baud rate to the initial baud rate that was set for
    // serial communication, since the baud rate is set to 9600 by 
    // NDICommandInterpreter::SetCommunication() in order to communicate
    // with the just-turned-on device which has a default baud rate of 9600
    CommandInterpreterType::COMMBaudType baudRateForCOMM = 
      CommandInterpreterType::NDI_9600;

    switch (m_BaudRate)
      {
      case CommunicationType::BaudRate9600:
        baudRateForCOMM = CommandInterpreterType::NDI_9600;
        break;
      case CommunicationType::BaudRate19200:
        baudRateForCOMM = CommandInterpreterType::NDI_19200;
        break;
      case CommunicationType::BaudRate38400:
        baudRateForCOMM = CommandInterpreterType::NDI_38400;
        break;
      case CommunicationType::BaudRate57600:
        baudRateForCOMM = CommandInterpreterType::NDI_57600;
        break;
      case CommunicationType::BaudRate115200:
        baudRateForCOMM = CommandInterpreterType::NDI_115200;
        break;
      }

    m_CommandInterpreter->COMM(baudRateForCOMM,
                               CommandInterpreterType::NDI_8N1,
                               CommandInterpreterType::NDI_NOHANDSHAKE);

    result = this->CheckError(m_CommandInterpreter);
    }

  return result;
}

/** Close communication with the tracking device. */
AuroraTracker::ResultType AuroraTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalClose called ...\n");

  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);

  return this->CheckError(m_CommandInterpreter);
}

/** Activate the tools attached to the tracking device. */
AuroraTracker::ResultType AuroraTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalActivateTools called ...\n");

  this->EnableToolPorts();

  m_NumberOfTools = 0;

  for(unsigned int port = 0; port < NumberOfPorts; port++)
    {
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      if( this->m_PortEnabled[port][channel] )
        {
        m_NumberOfTools++;
        }
      }
    }

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
AuroraTracker::ResultType AuroraTracker::InternalDeactivateTools( void )
{
  this->DisableToolPorts();

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
AuroraTracker::ResultType AuroraTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
AuroraTracker::ResultType AuroraTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();

  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
AuroraTracker::ResultType AuroraTracker::InternalReset( void )
{
  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  ResultType result = this->CheckError(m_CommandInterpreter);

  if (result == SUCCESS)
    {
    // log information about the device
    m_CommandInterpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE);
    result = this->CheckError(m_CommandInterpreter);
    }

  return result;
}

/** Update the status and the transforms for all TrackerTools. */
AuroraTracker::ResultType AuroraTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalUpdateStatus called ...\n");


  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  m_BufferLock->Lock();

  for (unsigned int port = 0; port < NumberOfPorts; port++) 
    {
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      // convert m_StatusBuffer flags from NDI to vtkTracker format
      const int portStatus = m_StatusBuffer[port][channel];

      // only report tools that are enabled
      if ((portStatus & mflags) != mflags) 
        {
        continue;
        }

      // only report tools that are in view
      if (m_AbsentBuffer[port][channel])
        {
        // there should be a method to set that the tool is not in view
        igstkLogMacro( DEBUG, "AuroraTracker::InternalUpdateStatus: " <<
                       "tool " << port << " is not in view\n");
        continue;
        }

      // create the transform
      TransformType transform;
      const double *ndiTransform = m_TransformBuffer[port][channel];

      typedef TransformType::VectorType TranslationType;
      TranslationType translation;

      translation[0] = ndiTransform[4];
      translation[1] = ndiTransform[5];
      translation[2] = ndiTransform[6];

      typedef TransformType::VersorType RotationType;
      RotationType rotation;
      const double normsquared = (ndiTransform[0]*ndiTransform[0] +
                                  ndiTransform[1]*ndiTransform[1] +
                                  ndiTransform[2]*ndiTransform[2] +
                                  ndiTransform[3]*ndiTransform[3]);

      // don't allow null quaternions
      if (normsquared < 1e-6)
        {
        rotation.Set(0.0, 0.0, 0.0, 1.0);
        igstkLogMacro( WARNING, "AuroraTracker::InternUpdateStatus: bad "
                       "quaternion, norm=" << sqrt(normsquared) << "\n");
        }
      else
        {
        rotation.Set(ndiTransform[1], ndiTransform[2], ndiTransform[3],
                     ndiTransform[0]);
        }

      // report NDI error value
      typedef TransformType::ErrorType  ErrorType;
      ErrorType errorValue = ndiTransform[7];

      typedef TransformType::TimePeriodType TimePeriodType;
      const TimePeriodType validityTime = 100.0;

      transform.SetToIdentity(validityTime);
      transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                          validityTime);

      this->SetToolTransform(port, channel, transform);
      }
    }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
AuroraTracker::ResultType AuroraTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);

  ResultType result = this->CheckError(m_CommandInterpreter);

  // lock the buffer
  m_BufferLock->Lock();

  // Initialize transformations to identity.
  // The NDI transform is 8 values:
  // the first 4 values are a quaternion
  // the next 3 values are an x,y,z position
  // the final value is an error estimate in the range [0,1]
  unsigned int port;   // physical port number
  unsigned int channel; // channel on port
  for (port = 0; port < NumberOfPorts; port++)
    {
    for (channel = 0; channel < NumberOfChannels; channel++)
      {
      m_TransformBuffer[port][channel][0] = 1.0;
      m_TransformBuffer[port][channel][1] = 0.0;
      m_TransformBuffer[port][channel][2] = 0.0;
      m_TransformBuffer[port][channel][3] = 0.0;
      m_TransformBuffer[port][channel][4] = 0.0;
      m_TransformBuffer[port][channel][5] = 0.0;
      m_TransformBuffer[port][channel][6] = 0.0;
      m_TransformBuffer[port][channel][7] = 0.0;
      }
    }

  if (result == SUCCESS)
    {
    unsigned long frame[NumberOfPorts][NumberOfChannels];
    for (port = 0; port < NumberOfPorts; port++)
      {
      for (channel = 0; channel < NumberOfChannels; channel++)
        {
        const int ph = this->m_PortHandle[port][channel];
        m_AbsentBuffer[port][channel] = 0;
        m_StatusBuffer[port][channel] = 0;

        frame[port][channel] = 0;
        if (ph == 0)
          {
          continue;
          }

        double transform[8];
        const int tstatus = m_CommandInterpreter->GetTXTransform(ph,transform);
        const int absent = (tstatus != CommandInterpreterType::NDI_VALID);
        const int status = m_CommandInterpreter->GetTXPortStatus(ph);
        frame[port][channel] = m_CommandInterpreter->GetTXFrame(ph);

        if (!absent)
          {
          for(unsigned int i = 0; i < 8; i++ )
            {
            m_TransformBuffer[port][channel][i] = transform[i];
            }
          }

        m_AbsentBuffer[port][channel] = absent;
        m_StatusBuffer[port][channel] = status;
        }
      }
    }

  // In the original vtkNDITracker code, there was a check at this
  // point in the code to see if any new tools had been plugged in

  // unlock the buffer
  m_BufferLock->Unlock();

  return result;
}

/** Specify an SROM file to be used with a custom tool or splitter. */
void AuroraTracker::AttachSROMFileNameToPort( const unsigned int portNum,
                                              std::string fileName )
{
  igstkLogMacro( DEBUG, "AuroraTracker::AttachSROMFileNameToPort called..\n");

  if ( portNum < NumberOfPorts )
    {
    m_SROMFileNames[portNum][0] = fileName;
    }
}

/** Specify an SROM file to be used with a specific tool channel */
void AuroraTracker::AttachSROMFileNameToChannel( const unsigned int portNum,
                                                 const unsigned int channelNum,
                                                 std::string fileName )
{
  igstkLogMacro( DEBUG,
                 "AuroraTracker::AttachSROMFileNameToChannel called..\n");

  if ( portNum < NumberOfPorts && channelNum < NumberOfChannels )
    {
    m_SROMFileNames[portNum][channelNum] = fileName;
    }
}

/** Load the virtual SROMs onto tools that need them */
bool AuroraTracker::LoadVirtualSROMs( void )
{
  igstkLogMacro( DEBUG, "AuroraTracker::LoadVirtualSROMS called...\n");

  // if any SROMs were not successfully written, this method returns "false"
  bool returnValue = true;

  // loop over all open port handles
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);
  if (this->CheckError(m_CommandInterpreter) == SUCCESS)
    { 
    unsigned int numHandles = m_CommandInterpreter->GetPHSRNumberOfHandles();
    for (unsigned int handleIndex = 0;
         handleIndex < numHandles;
         handleIndex++)
      {
      int handleInfo = m_CommandInterpreter->GetPHSRInformation(handleIndex);

      // check whether there is an uninitialized tool plugged in
      if ((handleInfo & CommandInterpreterType::NDI_TOOL_IN_PORT) &&
          !(handleInfo & CommandInterpreterType::NDI_INITIALIZED))
        {
        int ph = m_CommandInterpreter->GetPHSRHandle(handleIndex);

        // check what the port and channel numbers are
        m_CommandInterpreter->PHINF(ph,
                                    CommandInterpreterType::NDI_BASIC |
                                    CommandInterpreterType::NDI_PORT_LOCATION);

        if (this->CheckError(m_CommandInterpreter) == SUCCESS)
          {
          // the port and channel are read in as text, so convert to
          // int, note that Aurora starts counting ports at "1"
          char location[256];
          m_CommandInterpreter->GetPHINFPortLocation(location);
          unsigned int port = (location[10]-'0')*10 + (location[11]-'0') - 1;
          unsigned int channel = (location[12]-'0')*10 + (location[13]-'0');

          // check if an SROM is attached for this port and channel
          if (port < NumberOfPorts && channel < NumberOfChannels &&
              !m_SROMFileNames[port][channel].empty())
            {
            std::string &sromFileName = m_SROMFileNames[port][channel];
            std::ifstream sromFile;
            sromFile.open(sromFileName.c_str(), std::ios::binary );
            
            if (!sromFile.is_open())
              {
              igstkLogMacro( WARNING, "AuroraTracker::LoadVirtualSROM: couldn't"
                             "find SROM file " << sromFileName << " ...\n");

              returnValue = false;
              }
            else
              {
              // most SROM files don't contain the whole 1024 bytes, they only
              // contain whatever is necessary, so fill the rest with zero
              char data[1024];
              memset( data, 0, 1024 );
              sromFile.read( data, 1024 );
              sromFile.close();

              // write the SROM contents to the AURORA
              int successfulWrite = 1;
              for (unsigned int address = 0;
                   address < 1024 && successfulWrite;
                   address += 64)
                {
                // holds hexadecimal data to be sent to device
                char hexbuffer[129];
    
                // convert data to hexadecimal and write to virtual SROM in
                // 64-byte chunks
                m_CommandInterpreter->HexEncode(hexbuffer, &data[address], 64);
                m_CommandInterpreter->PVWR(ph, address, hexbuffer);

                // set returnValue to false if write wasn't successfule
                returnValue = ((this->CheckError(m_CommandInterpreter)
                                != SUCCESS) && returnValue);
                }
              }
            }
          }
        }
      }
    }

  return returnValue;
}

/** Enable all tool ports that are occupied.
  * This method follows the "wired" flowchart in the
  * NDI Polaris and Aurora Combined Application Programmer's Interface
  * Guide */
void AuroraTracker::EnableToolPorts()
{
  // reset our information about the tool ports
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      this->m_PortHandle[port][channel] = 0;
      this->m_PortEnabled[port][channel] = 0;
      }
    }
  
  // keep list of tools that fail to initialize, so we don't keep retrying,
  // the largest port handle possible is 0xFF, or 256
  int alreadyAttemptedPINIT[256];
  for (int ph = 0; ph < 256; ph++)
    {
    alreadyAttemptedPINIT[ph] = 0;
    }

  // this is the "outer loop" from the flow chart, loop 256 times maximum
  unsigned int count = 0;
  while (count < 256)
    {
    // free ports that are waiting to be freed
    m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
    unsigned int nHandles = m_CommandInterpreter->GetPHSRNumberOfHandles();
    unsigned int iHandle;
    for (iHandle = 0; iHandle < nHandles; iHandle++)
      {
      const int ph = m_CommandInterpreter->GetPHSRHandle(iHandle);
      m_CommandInterpreter->PHF(ph);
      
      // if an error occurs, print the error but don't abort
      this->CheckError(m_CommandInterpreter);

      // allow this port handle to be re-initialized
      alreadyAttemptedPINIT[ph] = 0;
      }

    // initialize ports waiting to be initialized,  
    // repeat as necessary (in case multi-channel tools are used) 
    while (count++ < 256)
      {
      m_CommandInterpreter->PHSR(
        CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
    
      if (this->CheckError(m_CommandInterpreter) == FAILURE)
        {
        break;
        }

      nHandles = m_CommandInterpreter->GetPHSRNumberOfHandles();
      int foundNewTool = 0;

      // try to initialize all port handles
      for (iHandle = 0; iHandle < nHandles; iHandle++)
        {
        const int ph = m_CommandInterpreter->GetPHSRHandle(iHandle);
        // only call PINIT on tools that didn't fail last time
        // (the &0xFF makes sure index is < 256)
        if (!alreadyAttemptedPINIT[(ph & 0xFF)])
          {
          alreadyAttemptedPINIT[(ph & 0xFF)] = 1;
          m_CommandInterpreter->PINIT(ph);
          if (this->CheckError(m_CommandInterpreter) == SUCCESS)
            {
            foundNewTool = 1;
            }
          }
        }

      // exit if no new tools were initialized this round
      if (!foundNewTool)
        {
        break;
        }

      // load the virtual SROMs, this is done here because the tools
      // on a splitter don't appear until PINIT is called on the splitter.
      this->LoadVirtualSROMs();
      }

    // enable all initialized tools
    m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);

    nHandles = m_CommandInterpreter->GetPHSRNumberOfHandles();
  
    for (iHandle = 0; iHandle < nHandles; iHandle++)
      {
      const int ph = m_CommandInterpreter->GetPHSRHandle(iHandle);
      m_CommandInterpreter->PHINF(ph, CommandInterpreterType::NDI_BASIC);

      // tool identity and type information
      char identity[512];
      m_CommandInterpreter->GetPHINFToolInfo(identity);

      // use tool type information to figure out mode for enabling
      int mode = CommandInterpreterType::NDI_DYNAMIC;

      if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
        { // button-box or foot pedal
        mode = CommandInterpreterType::NDI_BUTTON_BOX;
        }
      else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
        { // reference
        mode = CommandInterpreterType::NDI_STATIC;
        }

      // enable the tool
      m_CommandInterpreter->PENA(ph, mode);

      // print any warnings
      this->CheckError(m_CommandInterpreter);
      }
    }

  // get information for all tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);

  unsigned int nHandles = m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (unsigned int iHandle = 0; iHandle < nHandles; iHandle++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(iHandle);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );

    if (this->CheckError(m_CommandInterpreter) == FAILURE)
      {
      continue;
      }

    // get the physical port identifier
    char location[512];
    m_CommandInterpreter->GetPHINFPortLocation(location);

    unsigned int port = (location[10]-'0')*10 + (location[11]-'0') - 1;
    unsigned int channel = 0;

    if (port < NumberOfPorts)
      {
      // assign handle to the next available channel
      for (channel = 0; channel < NumberOfChannels; channel++)
        {
        if (this->m_PortHandle[port][channel] == 0)
          {
          this->m_PortHandle[port][channel] = ph;
          break;
          }
        }
      }

    const int status = m_CommandInterpreter->GetPHINFPortStatus();

    if (port < NumberOfPorts && channel < NumberOfChannels)
      {
      this->m_PortEnabled[port][channel] = 
        ((status & CommandInterpreterType::NDI_ENABLED) != 0);
      }
    }
}

/** Disable all tool ports. */
void AuroraTracker::DisableToolPorts( void )
{
  // disable all enabled tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ENABLED_HANDLES);
  unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (unsigned int tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PDIS( ph );

    // print warning if failed to disable
    this->CheckError(m_CommandInterpreter);
    }

  // disable the enabled ports
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      this->m_PortEnabled[port][channel] = 0;
      }
    }
}


/** Print Self function */
void AuroraTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  for( unsigned int i = 0; i < NumberOfPorts; i++ )
    {
    for ( unsigned int j = 0; j < NumberOfChannels; j++)
      {
      if (m_PortHandle[i][j] != 0 || j == 0)
        {
        os << indent << "Port " << i << " Channel " << j
           << " PortHandle: " << m_PortHandle[i][j] << std::endl;
        os << indent << "Port " << i << " Channel " << j
           << " PortEnabled: " << m_PortEnabled[i][j] << std::endl;
        }
      }
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
}

} // end of namespace igstk
