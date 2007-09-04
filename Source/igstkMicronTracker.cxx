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

namespace igstk
{

/** Constructor: Initializes all internal variables. */
MicronTracker::MicronTracker(void):m_StateMachine(this)
{
  m_NumberOfTools = 0;

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

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Take the tracking device out of tracking mode. */
MicronTracker::ResultType MicronTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "MicronTracker::InternalStopTracking called ...\n");

  // Send the command to stop tracking.

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS; 
}

/** Reset the tracking device to put it back to its original state. */
MicronTracker::ResultType MicronTracker::InternalReset( void )
{
  // Send the command to reset.
  
  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine) 
  return SUCCESS;
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
