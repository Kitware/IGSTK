/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTracker.cxx
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

#include "igstkMicronTracker.h"
#include "igstkTrackerPort.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
MicronTracker::MicronTracker(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    TrackerPortPointer tport = TrackerPortType::New();
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      MicronTrackerToolPointer tool = MicronTrackerToolType::New();
      tport->AddTool(tool);
      }
    this->AddPort(tport);
    }

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the tracker to the main thread.
  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
MicronTracker::~MicronTracker(void)
{
}

/** Helper function for reporting interpreter errors. */
MicronTracker::ResultType
MicronTracker::CheckError(CommandInterpreterType *interpreter)
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
    igstkLogMacro( WARNING, "MicronTracker Error " << os.str() << ": " <<
                   interpreter->ErrorString(errnum) << "\n");

    igstkLogMacro( WARNING, interpreter->ErrorString(errnum) << "\n");
    return FAILURE;
    }

  return SUCCESS;
}

/** Open communication with the tracking device. */
MicronTracker::ResultType MicronTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalOpen called ...\n");

  // Insert logic for opening communication with the MicronTracker,
  // and set m_Version and any other status information
  m_Version = "";

  // Return SUCCESS or FAILURE depending on whether communication
  // was successfully opened, without error
  return SUCCESS;
}

/** Close communication with the tracking device. */
MicronTracker::ResultType MicronTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalClose called ...\n");

  // Return SUCCESS or FAILURE depending on whether communication
  // was successfully opened, without error
  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
MicronTracker::ResultType MicronTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalActivateTools called ...\n");

  // Communicate with the tracker to get all tools on-line.
  // Set m_NumberOfTools to be the number of tools that were found.
  m_NumberOfTools = 0;

  // Return SUCCESS or FAILURE depending on whether communication
  // was successfully opened, without error
  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
MicronTracker::ResultType MicronTracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalDeactivateTools called ...\n");

  // Communicate with the device to deactivate all tools (depending
  // on the tracker, there might not be anything to do here).

  // Return SUCCESS or FAILURE depending on whether communication
  // was successfully opened, without error
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalStartTracking called ...\n");  

  // Send the command to start tracking
  m_CommandInterpreter->StartTracking();

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalStopTracking called ...\n");

  // Send the command to stop tracking.
  m_CommandInterpreter->StopTracking();

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
MicronTracker::ResultType MicronTracker::InternalReset( void )
{
  m_CommandInterpreter->Reset();

  ResultType result = this->CheckError(m_CommandInterpreter);

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return this->CheckError(m_CommandInterpreter);
}

/** Update the status and the transforms for all TrackerTools. */
MicronTracker::ResultType MicronTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  for (unsigned int port = 0; port < NumberOfPorts; port++) 
    {
    for (unsigned int channel = 0; channel < NumberOfChannels; channel++)
      {
      // Create the transform
      typedef TransformType::VectorType TranslationType;
      typedef TransformType::VersorType RotationType;
      typedef TransformType::ErrorType  ErrorType;
      TransformType transform;
      TranslationType translation;
      ErrorType errorValue;

      // Insert code here to copy the transformation out of the
      // shared memory buffer

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

/** Update the shared memory buffer and the transforms.  This function
 *  is called by the thread that communicates with the tracker while
 *  the tracker is in the Tracking state. */
MicronTracker::ResultType MicronTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // Send the commands to the device that will get the transforms
  // for all of the tools.


  // In case of failure, return FAILURE

  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  // Copy the transforms and any status information into the
  // buffer.

  // unlock the buffer
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Print Self function */
void MicronTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
}

} // end of namespace igstk
