/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTracker.cxx
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

#include "igstkPolarisTracker.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisTracker::PolarisTracker(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();

  this->SetThreadingEnabled( true );

  m_BaudRate = CommunicationType::BaudRate115200; 
  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
PolarisTracker::~PolarisTracker(void)
{
}


/** Helper function for reporting interpreter errors. */
PolarisTracker::ResultType
PolarisTracker::CheckError(CommandInterpreterType *interpreter) const
{
  const int errnum = interpreter->GetError();

  if (errnum)
    {
    // convert errnum to a hexidecimal string
    itk::OStringStream os;
    os << "0x";
    os.width(2);
    os.fill('0');
    os << std::hex << std::uppercase << errnum;
    igstkLogMacro( WARNING, "Polaris Error " << os.str() << ": " <<
                   interpreter->ErrorString(errnum) << "\n");

    return FAILURE;
    }

  return SUCCESS;
}


/** Set the communication object, it will be initialized as necessary
  * for use with the Polaris */
void PolarisTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_BaudRate = communication->GetBaudRate();
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalOpen called ...\n");

  ResultType result = SUCCESS;

  if (!m_Communication)
    {
    igstkLogMacro( CRITICAL, "PolarisTracker: AttemptToOpen before "
                   "Communication is set.\n");
    result = FAILURE;
    }

  if (result == SUCCESS)
    {
    m_CommandInterpreter->RESET();
    m_CommandInterpreter->INIT();

    result = this->CheckError(m_CommandInterpreter);
    }

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
      CommandInterpreterType::NDI_115200;

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

/** Verify tracker tool information. */
PolarisTracker::ResultType PolarisTracker
::VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  //  Verify that 
  //  
  //  1) the tracker tool information provided by the user matches with
  //  the information available in the SROM file.
  //  
  //  2) the tool is actually attached to the correct physical port
  //  for a wired tool.
  //  
  //  To get the tool information
  //   - If it is wireless
  //      == SROM file has to be loaded and port handle created.
  //   - If it is wired
  //      == If SROM file is specified
  //          == Load the SROM file and create a port handle
  //      == Otherwise, create a port handle
  //  

  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker::VerifyTrackerToolInformation called ...\n");

  typedef igstk::PolarisTrackerTool              PolarisTrackerToolType;

  PolarisTrackerToolType * polarisTrackerTool = 
             dynamic_cast< PolarisTrackerToolType * > ( trackerTool );   

  if ( polarisTrackerTool == NULL )
    {
    return FAILURE;
    } 

  bool wirelessTool = polarisTrackerTool->IsToolWirelessType();
  bool SROMFileSpecified  = polarisTrackerTool->IsSROMFileNameSpecified();

  // port handle
  int ph;

  if( wirelessTool || SROMFileSpecified ) 
    {
    std::ifstream sromFile; 
    std::string SROMFileName = polarisTrackerTool->GetSROMFileName();
    sromFile.open(SROMFileName.c_str(), std::ios::binary );

    if (!sromFile.is_open())
      {
      igstkLogMacro( WARNING, "igstk::PolarisTracker::Failing to open"
                     << SROMFileName << " ...\n");
      return FAILURE;
      }

    // most SROM files don't contain the whole 1024 bytes, they only
    // contain whatever is necessary, so the rest should be filled with zero
    char data[1024]; 
    memset( data, 0, 1024 );
    sromFile.read( data, 1024 );
    sromFile.close();

    // the "port" must be set to "**" to support the Vicra
    m_CommandInterpreter->PHRQ("********", // device number
                               "*",        // TIU or SCU
                               "1",        // wired or wireless
                               "**",       // port
                               "**");      // channel

    if (this->CheckError(m_CommandInterpreter) == FAILURE)
      {
      return FAILURE;
      }

    ph = m_CommandInterpreter->GetPHRQHandle();

    for ( unsigned int i = 0; i < 1024; i += 64)
      {
      // holds hexidecimal data to be sent to device
      char hexbuffer[129]; 
      
      // convert data to hexidecimal and write to virtual SROM in
      // 64-byte chunks
      m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64);
      m_CommandInterpreter->PVWR(ph, i, hexbuffer);
      }
    }
  else
    {
    //search ports with uninitialized handles
    // initialize ports waiting to be initialized,  
    // repeat as necessary (in case multi-channel tools are used) 

    bool foundNewTool = false;

    for (int safetyCount = 0; safetyCount < 256; safetyCount++)
      {
      m_CommandInterpreter->PHSR(
        CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
      
      if (this->CheckError(m_CommandInterpreter) == FAILURE)
        {
        igstkLogMacro( WARNING, 
           "igstk::PolarisTracker::Error searching for uninitialized ports ");
        return FAILURE;
        }

      unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();

      // Make sure there is one and only one uninitialized port
      if ( ntools == 0 )
        {
        continue;
        }

      // The toolnumber will be assigned to 0 by default
      unsigned int toolNumber = 0; 
      ph = m_CommandInterpreter->GetPHSRHandle( toolNumber );
      foundNewTool = true;
      break;
      }

    if( !foundNewTool )
      {
      igstkLogMacro(WARNING, "Uninitialized port not found");
      return FAILURE;
      }
    }  

  // Once we got the port handle, we can continue on with initializing
  // and enabling the port

  // initialize the port 
  m_CommandInterpreter->PINIT(ph);

  if (this->CheckError(m_CommandInterpreter) == SUCCESS)
    {
    igstkLogMacro(INFO, "Port handle initialized successfully ");
    }
  else
    {
    igstkLogMacro(CRITICAL, "Failure initializing the port");
    }

  m_CommandInterpreter->PHINF(ph, CommandInterpreterType::NDI_BASIC);

  // tool identity and type information
  char identity[512];
  m_CommandInterpreter->GetPHINFToolInfo(identity);
  igstkLogMacro(INFO, "Tool Information: " << identity); 

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
  if(this->CheckError(m_CommandInterpreter) == SUCCESS)
    {
    igstkLogMacro(INFO, "Port handle enabled successfully "); 
    }
  else
    {
    igstkLogMacro(CRITICAL, "Failure enabling the port handle");
    return FAILURE;
    }


  //tool information
  m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );

  if (this->CheckError(m_CommandInterpreter) == FAILURE)
    {
    igstkLogMacro(CRITICAL,"Error accessing the tool information");
    return FAILURE;
    }

  // get the physical port identifier
  char location[512];
  m_CommandInterpreter->GetPHINFPortLocation(location);

  // physical port number
  unsigned int port = 0;

  if (location[9] == '0') // wired tool
    {
    unsigned int ndiport = (location[10]-'0')*10 + (location[11]-'0');

    const unsigned int NumberOfPorts = 12;
    if (ndiport > 0 && ndiport <= NumberOfPorts)
      {
      port = ndiport - 1;
      // Verify port number specified 
      if ( port != polarisTrackerTool->GetPortNumber() )
        {
        igstkLogMacro(CRITICAL, 
          "The tracker tool is probably inserted into the wrong port: "
          "The port number specified for the tool doesn't match with "
          "what is detected from the hardware");
        return FAILURE;
        }
      }
    }

  const int status = m_CommandInterpreter->GetPHINFPortStatus();

  igstkLogMacro(INFO, "Port status information: " << status ); 

  // tool status
  igstkLogMacro(INFO, 
    "Tool status: " <<  m_CommandInterpreter->GetPHINFPortStatus());

  // tool type
  igstkLogMacro(INFO, 
    "Tool type: " << m_CommandInterpreter->GetPHINFToolType());

  // tool part number
  char partNumber[21];
  m_CommandInterpreter->GetPHINFPartNumber( partNumber );
  igstkLogMacro(INFO, "Part number: " << partNumber );

  // tool accessories
  igstkLogMacro(INFO, 
    "Tool accessories: " << m_CommandInterpreter->GetPHINFAccessories());

  // tool marker type
  igstkLogMacro(INFO, 
    "Marker type: " << m_CommandInterpreter->GetPHINFMarkerType());

  std::string trackerToolIdentifier = 
    polarisTrackerTool->GetTrackerToolIdentifier();

  // add it to the port handle container 
  this->m_PortHandleContainer[ trackerToolIdentifier ] = ph;

  // add it to the tool absent status 
  this->m_ToolAbsentStatusContainer[ trackerToolIdentifier ] = 0;

  // add it to the tool status container
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}
 
PolarisTracker::ResultType 
PolarisTracker::
RemoveTrackerToolFromInternalDataContainers( TrackerToolType * trackerTool ) 
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker::RemoveTrackerToolFromInternalDataContainers "
    "called ...\n");

  // if SROM file has been loaded for this tracker tool, clear it first.
  TrackerToolsContainerType trackerToolContainer = 
    this->GetTrackerToolContainer();

  typedef igstk::PolarisTrackerTool              PolarisTrackerToolType;

  PolarisTrackerToolType * polarisTrackerTool = 
             dynamic_cast< PolarisTrackerToolType * > ( trackerTool );   

  std::string trackerToolIdentifier = trackerTool->GetTrackerToolIdentifier();

  if ( polarisTrackerTool != NULL ) 
    {
    bool SROMFileSpecified  = polarisTrackerTool->IsSROMFileNameSpecified();

    if( SROMFileSpecified )
      {
      m_CommandInterpreter->PHF( m_PortHandleContainer[trackerToolIdentifier] );
      }
    }
    
  // disable the port handle
  m_CommandInterpreter->PDIS( m_PortHandleContainer[ trackerToolIdentifier] );  

  // print warning if failed to disable
  this->CheckError(m_CommandInterpreter);

  // remove the tool from port handle container
  this->m_PortHandleContainer.erase( trackerToolIdentifier );

  // remove the tool from absent status container 
  this->m_ToolAbsentStatusContainer.erase( trackerToolIdentifier );

  // remove the tool from  tool status container
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );

  return SUCCESS;
}


/** Close communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalClose called ...\n");

  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);

  return this->CheckError(m_CommandInterpreter);
}

/** Put the tracking device into tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();

  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
PolarisTracker::ResultType PolarisTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalReset called ...\n");

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
PolarisTracker::ResultType PolarisTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTracker::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  m_BufferLock->Lock();

  typedef PortHandleContainerType::const_iterator  ConstIteratorType;

  ConstIteratorType inputItr = m_PortHandleContainer.begin();
  ConstIteratorType inputEnd = m_PortHandleContainer.end();

  TrackerToolsContainerType trackerToolContainer = 
    this->GetTrackerToolContainer();

  while( inputItr != inputEnd )
    {
    const int portStatus = m_ToolStatusContainer[inputItr->first];

    // only report tools that are enabled
    if ((portStatus & mflags) != mflags) 
      {
      igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not available \n");
      ++inputItr;
      continue;
      }

    // only report tools that are in view
    if (m_ToolAbsentStatusContainer[inputItr->first])
      {
      // there should be a method to set that the tool is not in view
      igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not in view\n");

      // report to the tracker tool that the tracker is not available 
      this->ReportTrackingToolNotAvailable( 
        trackerToolContainer[inputItr->first] );

      ++inputItr;
      continue;
      }

    const PortIdentifierType portId = inputItr->first;

    // report to the tracker tool that the tracker is Visible 
    this->ReportTrackingToolVisible( trackerToolContainer[portId] );
 
    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (m_ToolTransformBuffer[portId])[4];
    translation[1] = (m_ToolTransformBuffer[portId])[5];
    translation[2] = (m_ToolTransformBuffer[portId])[6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    const double normsquared = 
      m_ToolTransformBuffer[portId][0]*m_ToolTransformBuffer[portId][0] +
      m_ToolTransformBuffer[portId][1]*m_ToolTransformBuffer[portId][1] +
      m_ToolTransformBuffer[portId][2]*m_ToolTransformBuffer[portId][2] +
      m_ToolTransformBuffer[portId][3]*m_ToolTransformBuffer[portId][3];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
      rotation.Set(0.0, 0.0, 0.0, 1.0);
      igstkLogMacro( WARNING, "igstk::PolarisTracker::InternUpdateStatus: bad "
                     "quaternion, norm=" << sqrt(normsquared) << "\n");
      }
    else
      {
      // ITK quaternions are in xyzw order, not wxyz order
      rotation.Set(m_ToolTransformBuffer[portId][1],
                   m_ToolTransformBuffer[portId][2],
                   m_ToolTransformBuffer[portId][3],
                   m_ToolTransformBuffer[portId][0]);
      }

    // retool NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_ToolTransformBuffer[portId][7];

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());
  
    // set the raw transform
    this->SetTrackerToolRawTransform( trackerToolContainer[portId], transform );
    this->SetTrackerToolTransformUpdate( trackerToolContainer[portId], true );

    ++inputItr;
    }
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
PolarisTracker::ResultType PolarisTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTracker::InternalThreadedUpdateStatus "
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
  //
  std::vector < double > transform;
  transform.push_back ( 1.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );
  transform.push_back ( 0.0 );

  if (result == SUCCESS)
    {

    typedef PortHandleContainerType::iterator  IteratorType;

    IteratorType inputItr = m_PortHandleContainer.begin();
    IteratorType inputEnd = m_PortHandleContainer.end();

    while( inputItr != inputEnd )
      {
      m_ToolAbsentStatusContainer[inputItr->first] = 0;
      m_ToolStatusContainer[inputItr->first] = 0;
 
      unsigned int ph = inputItr->second;
      if ( ph == 0 )
        {
        ++inputItr;
        continue;
        }

      double transformRecorded[8];

      const int tstatus = 
        m_CommandInterpreter->GetTXTransform(ph, transformRecorded);

      const int absent = (tstatus != CommandInterpreterType::NDI_VALID);
      const int status = m_CommandInterpreter->GetTXPortStatus(ph);

      if (!absent)
        {
        for( unsigned int i = 0; i < 8; i++ )
          {
          transform[i] = transformRecorded[i];
          }
        }

      m_ToolAbsentStatusContainer[inputItr->first] = absent;
      m_ToolStatusContainer[inputItr->first] = status;
      m_ToolTransformBuffer[inputItr->first] = transform;

      ++inputItr;
      }
    }

  // In the original vtkNDITracker code, there was a check at this
  // point in the code to see if any new tools had been plugged in

  // unlock the buffer
  m_BufferLock->Unlock();

  return result;
}

/** Print Self function */
void PolarisTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

}


} // end of namespace igstk
