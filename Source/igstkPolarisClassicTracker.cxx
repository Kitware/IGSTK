/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkPolarisClassicTracker.cxx,v $
  Language:  C++
  Date:      $Date: 2008/02/12 16:43:59 $
  Version:   $Revision: 1.26 $

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

#include "igstkNDICommandInterpreterClassic.h"
#include "igstkPolarisClassicTracker.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisClassicTracker::PolarisClassicTracker(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();

  this->SetThreadingEnabled( true );

  m_BaudRate = CommunicationType::BaudRate115200;
  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
PolarisClassicTracker::~PolarisClassicTracker(void)
{
}

/** Verify tracker tool information. */
PolarisClassicTracker::ResultType PolarisClassicTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  //  Verify that
  //
  //  1) the tracker tool information provided by the user matches with
  //  the information available in the SROM file such as tool part number.
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
    "igstk::PolarisClassicTracker::VerifyTrackerToolInformation called ...\n");

  typedef igstk::PolarisTrackerTool              PolarisClassicTrackerToolType;

  TrackerToolType * trackerToolNonConst =
                  const_cast<TrackerToolType*>(trackerTool);

  PolarisClassicTrackerToolType * polarisTrackerTool =
       dynamic_cast< PolarisClassicTrackerToolType * >( trackerToolNonConst );

  if ( polarisTrackerTool == NULL )
    {
    igstkLogMacro ( DEBUG, "polarisTrackerTool pointer is null\n" );
    return FAILURE;
    }

  CommandInterpreterType::Pointer commandInterpreter =
    this->GetCommandInterpreter();

  /* PSTAT example output after RESET, INIT
  * 0000000            0000000000001
  *                                UNOCCUPIED
  *                                         UNOCCUPIED6A13
  */

  commandInterpreter->PSTAT(CommandInterpreterType::NDI_BASIC);


  unsigned int tool=polarisTrackerTool->GetPortNumber();
  //query about plugged in tools
  if(commandInterpreter->GetPSTAToccupied(tool))
    {
    commandInterpreter->PINIT(tool);
    if (this->CheckError(commandInterpreter) == FAILURE)
    {
        igstkLogMacro( DEBUG,
      "PolarisClassicTracker::EnableToolPorts failed to enable...\n");
      }
    }

  /* PSTAT output after PINIT 1, PINIT 2, PINIT ...
  * 2000001ELEKTA      0013091040213
  *                               1010001NDI         0033144280611
  *                                                               UNOCCUPIED40AE
  */
  commandInterpreter->PSTAT(CommandInterpreterType::NDI_BASIC);
  //init occoupied tools
  //query about plugged in tools
  if(commandInterpreter->GetPSTAToccupied(tool))
    {
    // tool identity and type information
    char identity[512];
    commandInterpreter->GetPSTATToolInfo(tool, identity);
    // use tool type information to figure out mode for enabling
    int mode = CommandInterpreterType::NDI_DYNAMIC;

    char toolKindString[1] = {identity[0]};
    int toolKindInt = 0;
    sscanf(toolKindString, "%d", &toolKindInt);
    if (toolKindInt == CommandInterpreterType::NDI_TYPE_BUTTON)
      {
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }

    //NDI_TYPE_REFERENCE   = 0x01
    if (toolKindInt == CommandInterpreterType::NDI_TYPE_REFERENCE)
      {
      mode = CommandInterpreterType::NDI_STATIC;
      }

    // enable tool
    commandInterpreter->PENA(tool, mode);
    // tool is enabled
    if( this->CheckError(commandInterpreter) == SUCCESS)
      {
      // Set the port handle to be added
      this->SetPortHandleToBeAdded( tool + 1);
      }
    else
      {
      std::cout << "Not able to enable tool: " <<  tool + 1 << std::endl;
      }
    }

  const int status = commandInterpreter->GetPSTATPortStatus(tool);

  igstkLogMacro(INFO, "Port status information: " << status  << "\n" );

  // tool status
  igstkLogMacro(INFO,
    "Tool status: " <<  commandInterpreter->GetPSTATPortStatus(tool) << "\n");

  // tool type
  igstkLogMacro(INFO,
    "Tool type: " << commandInterpreter->GetPSTATToolType(tool) << "\n");

  // tool accessories
  igstkLogMacro(INFO,
    "Tool accessories: "
        << commandInterpreter->GetPSTATAccessories(tool) << "\n" );

  // tool marker type
  igstkLogMacro(INFO,
    "Marker type: " << commandInterpreter->GetPSTATMarkerType(tool) << "\n" );

  return SUCCESS;
}

/** Helper function for reporting interpreter errors. */
PolarisClassicTracker::ResultType
PolarisClassicTracker::CheckError(CommandInterpreterType *interpreter) const
{
  const int errnum = interpreter->GetError();

  if (errnum)
    {
    // convert errnum to a hexidecimal string
    std::stringstream os;
    os << "0x";
    os.width(2);
    os.fill('0');
    os << std::hex << std::uppercase << errnum;
    igstkLogMacro( WARNING, "NDI Error " << os.str() << ": " <<
                   interpreter->ErrorString(errnum) << "\n");

    return FAILURE;
    }

  return SUCCESS;
}

/** Get method for the command interpreter
 *  This will method will be used by the derived classes */
PolarisClassicTracker::CommandInterpreterType::Pointer
PolarisClassicTracker::GetCommandInterpreter() const
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker:: GetCommandInterpreter ...\n");

  return m_CommandInterpreter;
}

/** Set the communication object, it will be initialized as necessary
  * for use with the NDI */
void PolarisClassicTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker:: Entered SetCommunication ...\n");
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
    "igstk::PolarisClassicTracker:: Exiting SetCommunication ...\n");
}

/** Open communication with the tracking device. */
PolarisClassicTracker::ResultType PolarisClassicTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG,
          "igstk::PolarisClassicTracker::InternalOpen called ...\n");

  ResultType result = SUCCESS;

  if (!m_Communication)
    {
    igstkLogMacro( CRITICAL, "PolarisClassicTracker: AttemptToOpen before "
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
    // NDICommandInterpreterClassic::SetCommunication() in order to communicate
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
PolarisClassicTracker::ResultType PolarisClassicTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG,
            "igstk::PolarisClassicTracker::InternalClose called ...\n");

  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);

  return this->CheckError(m_CommandInterpreter);
}

/** Put the tracking device into tracking mode. */
PolarisClassicTracker::ResultType
PolarisClassicTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker::InternalStartTracking called ...\n");

  m_CommandInterpreter->TSTART();

  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
PolarisClassicTracker
::ResultType PolarisClassicTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();

  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
PolarisClassicTracker::ResultType PolarisClassicTracker::InternalReset( void )
{
  igstkLogMacro( DEBUG,
            "igstk::PolarisClassicTracker::InternalReset called ...\n");

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
PolarisClassicTracker::ResultType PolarisClassicTracker::InternalUpdateStatus()
{
   igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);
                                                                //
                                //0000 0000 0000 0001 NDI_TOOL_IN_PORT
                                //0000 0000 0001 0000 NDI_INITIALIZED
                                //0000 0000 0010 0000 NDI_ENABLED
                                //0000 0000 0011 0001 bit by bit OR
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
      igstkLogMacro( DEBUG,
        "igstk::PolarisClassicTracker::InternalUpdateStatus: " <<
                    "tool " << inputItr->first << " is not available \n");
      ++inputItr;
      continue;
      }

    // only report tools that are in view
    if (m_ToolAbsentStatusContainer[inputItr->first])
      {
      // there should be a method to set that the tool is not in view
      igstkLogMacro( DEBUG,
              "igstk::PolarisClassicTracker::InternalUpdateStatus: " <<
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
      igstkLogMacro( WARNING,
              "igstk::PolarisClassicTracker::InternUpdateStatus: bad "
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
PolarisClassicTracker
::ResultType PolarisClassicTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
      "igstk::PolarisClassicTracker::InternalThreadedUpdateStatus "
                "called ...\n");

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->GX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);

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
        m_CommandInterpreter->GetGXTransform(ph-1, transformRecorded);

      const int absent = (tstatus != CommandInterpreterType::NDI_VALID);
      const int status = m_CommandInterpreter->GetGXPortStatus(ph-1);

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

PolarisClassicTracker::ResultType
PolarisClassicTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker"
    "::AddTrackerToolToInternalDataContainers called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  std::string trackerToolIdentifier =
    trackerTool->GetTrackerToolIdentifier();
  // add it to the port handle container
  this->m_PortHandleContainer[ trackerToolIdentifier ] = m_PortHandleToBeAdded;

  // add it to the tool absent status
  this->m_ToolAbsentStatusContainer[ trackerToolIdentifier ] = 0;

  // add it to the tool status container
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  return SUCCESS;
}

PolarisClassicTracker::ResultType
PolarisClassicTracker::
RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisClassicTracker::RemoveTrackerToolFromInternalDataContainers "
    "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  std::string trackerToolIdentifier =
    trackerTool->GetTrackerToolIdentifier();

  //m_CommandInterpreter->PHF( m_PortHandleContainer[trackerToolIdentifier] );

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


/** Print Self function */
void PolarisClassicTracker::PrintSelf(
                  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end of namespace igstk
