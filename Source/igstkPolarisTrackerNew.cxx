/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerNew.cxx
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

#include "igstkPolarisTrackerNew.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisTrackerNew::PolarisTrackerNew(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();

  this->SetThreadingEnabled( true );

  m_BaudRate = CommunicationType::BaudRate9600; 
  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
PolarisTrackerNew::~PolarisTrackerNew(void)
{
}


/** Helper function for reporting interpreter errors. */
PolarisTrackerNew::ResultType
PolarisTrackerNew::CheckError(CommandInterpreterType *interpreter)
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
void PolarisTrackerNew::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_BaudRate = communication->GetBaudRate();
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, "PolarisTrackerNew:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalOpen called ...\n");

  ResultType result = SUCCESS;

  if (!m_Communication)
    {
    igstkLogMacro( CRITICAL, "PolarisTrackerNew: AttemptToOpen before "
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

/** Verify tracker tool information*/
PolarisTrackerNew::ResultType PolarisTrackerNew
::VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  //Verify that 
  //
  //1) the tracker tool information provided by the user matches with
  //the information available in the SROM file.
  //
  //2) the tool is actually attached to the correct physical port
  //for a wired tool.
  //
  //To get the tool information
  // - If it is wireless
  //    == SROM file has to be loaded and port handle created.
  // - If it is wired
  //    == If SROM file is specified
  //        == Load the SROM file and create a port handle
  //    == Otherwise, create a port handle     
  //
  // 3) Get port handle
  //
  // Initialize 
  // repeat as necessary (in case multi-channel tools are used) 
  //
  //
  // keep list of tools that fail to initialize, so we don't keep retrying,
  // the largest port handle possible is 0xFF, or 256
  //
  //
  // If the tool is wireless type or wired with SROM file specified, load the SROM file and set the port handle
  //
  //
  //
  igstkLogMacro( DEBUG, "PolarisTrackerNew::VerifyTrackerToolInformation called ...\n");

  PolarisTrackerToolType * polarisTrackerTool = 
             dynamic_cast< PolarisTrackerToolType * > ( trackerTool );   

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
      igstkLogMacro( WARNING, "PolarisTrackerNew::Failing to open"
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
        std::cerr << "Error searching for uninitialized ports"  << std::endl;
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
      std::cerr << "Couldn't find uninitialized port" << std::endl;
      return FAILURE;
      }
    }  

  // Once we got the port handle, we can continue on with initializing
  // and enabling the port

  // initialize the port 
  m_CommandInterpreter->PINIT(ph);

  if (this->CheckError(m_CommandInterpreter) == SUCCESS)
    {
    std::cout << "Port handle initialized successfully " << std::endl;
    }
  else
    {
    std::cerr << "Failure initializing the port" << std::endl;
    }

  m_CommandInterpreter->PHINF(ph, CommandInterpreterType::NDI_BASIC);

  // tool identity and type information
  char identity[512];
  m_CommandInterpreter->GetPHINFToolInfo(identity);
  std::cout << "Tool Information: " << identity << std::endl; 

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

  //tool information
  m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );

  if (this->CheckError(m_CommandInterpreter) == FAILURE)
    {
    std::cerr  << "Error while trying to get tool information" << std::endl;
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
    if (ndiport > 0 && ndiport <= NumberOfPorts)
      {
      port = ndiport - 1;
      std::cout << "Port number: " << port << std::endl;
      // Verify port number specified 
      if ( port != polarisTrackerTool->GetPortNumber() )
        {
        std::cerr << "The tracker tool is probably inserted into the wrong port: " 
                  << "The port number specified for the tool doesn't match with " 
                     "what is detected from the hardware" << std::endl;
        return FAILURE;
        }
      }
    }
  else // wireless tool
    {
    std::cout<< "Tool is wireless" << std::endl;
    }

  const int status = m_CommandInterpreter->GetPHINFPortStatus();

  std::cout<< "Port status information: " << status << std::endl;

  // tool status
  std::cout << "Tool status: " <<  m_CommandInterpreter->GetPHINFPortStatus() << std::endl;

  // tool type
  std::cout<< "Tool type: " << m_CommandInterpreter->GetPHINFToolType() << std::endl;   

  // tool part number
  char partNumber[21];
  m_CommandInterpreter->GetPHINFPartNumber( partNumber ) ;
  std::cout<< "Part number: " << partNumber << std::endl; 

  // tool accessories
  std::cout<< "Tool accessories: " << m_CommandInterpreter->GetPHINFAccessories() << std::endl;

  // tool marker type
  std::cout << "Marker type: " << m_CommandInterpreter->GetPHINFMarkerType() << std::endl;

  std::string trackerToolIdentifier = polarisTrackerTool->GetTrackerToolIdentifier();

  // add it to the port handle container 
  this->m_PortHandleContainer[ trackerToolIdentifier ] = ph;

  // add it to the tool absent status 
  this->m_ToolAbsentStatusContainer[ trackerToolIdentifier ] = 0;

  // add it to the tool status container
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}
 
PolarisTrackerNew::ResultType 
PolarisTrackerNew::
RemoveTrackerToolFromInternalDataContainers( std::string trackerToolIdentifier ) 
{
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
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalClose( void )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalClose called ...\n");

  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);

  return this->CheckError(m_CommandInterpreter);
}

/** Activate the tools attached to the tracking device. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalActivateTools( void )
{
  //FIXME: this method seems like it wont be necessary any more as the tracker
  //tools get initialized when they get attached to the tracker
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalActivateTools called ...\n");

  ResultType result = SUCCESS;

  return result;
}

/** Deactivate the tools attached to the tracking device. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalDeactivateTools( void )
{
  //FIXME: add code to disable the handles and clean up the SROM file  
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();

  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalReset( void )
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
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  m_BufferLock->Lock();

  typedef std::map< std::string, int >::const_iterator  ConstIteratorType;

  ConstIteratorType inputItr = m_PortHandleContainer.begin();
  ConstIteratorType inputEnd = m_PortHandleContainer.end();

  while( inputItr != inputEnd )
    {
    std::cout << "Updating transform for tool: " << inputItr->first << std::endl;
    const int portStatus = m_ToolStatusContainer[inputItr->first];

    // only report tools that are enabled
    if ((portStatus & mflags) != mflags) 
      {
      igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not available \n");
      ++inputItr;
      continue;
      }

    // only report tools that are in view
    if (m_ToolAbsentStatusContainer[inputItr->first])
      {
      // there should be a method to set that the tool is not in view
      igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalUpdateStatus: " <<
                     "tool " << inputItr->first << " is not in view\n");
      ++inputItr;
      continue;
      }

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (m_ToolTransformBuffer[inputItr->first])[4];
    translation[1] = m_ToolTransformBuffer[inputItr->first][5];
    translation[2] = m_ToolTransformBuffer[inputItr->first][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    const double normsquared = 
      m_ToolTransformBuffer[inputItr->first][0]*m_ToolTransformBuffer[inputItr->first][0] +
      m_ToolTransformBuffer[inputItr->first][1]*m_ToolTransformBuffer[inputItr->first][1] +
      m_ToolTransformBuffer[inputItr->first][2]*m_ToolTransformBuffer[inputItr->first][2] +
      m_ToolTransformBuffer[inputItr->first][3]*m_ToolTransformBuffer[inputItr->first][3];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
      rotation.Set(0.0, 0.0, 0.0, 1.0);
      igstkLogMacro( WARNING, "PolarisTrackerNew::InternUpdateStatus: bad "
                     "quaternion, norm=" << sqrt(normsquared) << "\n");
      }
    else
      {
      // ITK quaternions are in xyzw order, not wxyz order
      rotation.Set(m_ToolTransformBuffer[inputItr->first][1],
                   m_ToolTransformBuffer[inputItr->first][2],
                   m_ToolTransformBuffer[inputItr->first][3],
                   m_ToolTransformBuffer[inputItr->first][0]);
      }

    // retool NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_ToolTransformBuffer[inputItr->first][7];

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        this->GetValidityTime());

    // FIXME: set the transform to the parent
    // FOR NOW, use SetToolTransform method
    //
    this->SetToolTransform( inputItr->first, transform ); 

    ++inputItr;
    }
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
PolarisTrackerNew::ResultType PolarisTrackerNew::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "PolarisTrackerNew::InternalThreadedUpdateStatus "
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

    typedef std::map< std::string, int >::iterator  IteratorType;

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
      const int tstatus = m_CommandInterpreter->GetTXTransform(ph, transformRecorded);
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
void PolarisTrackerNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

}


} // end of namespace igstk
