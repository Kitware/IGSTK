/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkFlockOfBirdsTracker.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
FlockOfBirdsTracker::FlockOfBirdsTracker(void):m_StateMachine(this)
{
  m_NumberOfTools = 0;
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    {
    this->m_PortEnabled[i] = 0;
    }

  for (unsigned int j = 0; j < NumberOfPorts; j++)
    { 
    FlockOfBirdsTrackerToolPointer tool = FlockOfBirdsTrackerToolType::New();
    TrackerPortPointer port = TrackerPortType::New();
    port->AddTool(tool);
    this->AddPort(port);
    }

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
  m_Communication = 0;
  m_CommandInterpreter = CommandInterpreterType::New();
}

/** Destructor */
FlockOfBirdsTracker::~FlockOfBirdsTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the FlockOfBirds */
void FlockOfBirdsTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Exiting SetCommunication ...\n");
}

/** Open communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalOpen called ...\n");
  m_CommandInterpreter->Open();
  m_CommandInterpreter->SetFormat(FB_POSITION_QUATERNION);
  return SUCCESS;
}

/** Close communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalClose called ...\n");
  m_CommandInterpreter->Close();
  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, 
               "FlockOfBirdsTracker::InternalActivateTools called ...\n");
  
  this->EnableToolPorts();

  m_NumberOfTools = 0;

  for(unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      m_NumberOfTools++;
      }
    }
  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalDeactivateTools( void )
{
  this->DisableToolPorts();

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, 
                "FlockOfBirdsTracker::InternalStartTracking called ...\n");
  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
                "FlockOfBirdsTracker::InternalStopTracking called ...\n");
  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalReset( void )
{
  return SUCCESS;
}


/** Update the status and the transforms for all TrackerTools. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, 
                 "FlockOfBirdsTracker::InternalUpdateStatus called ...\n");
  m_CommandInterpreter->Point();
  m_CommandInterpreter->Update();

  // create the transform
  TransformType transform;

  typedef TransformType::VectorType TranslationType;
  TranslationType translation;
  
  m_BufferLock->Lock();

  float offset[3];
  m_CommandInterpreter->GetPosition(offset);

  translation[0] = offset[0];
  translation[1] = offset[1];
  translation[2] = offset[2];

  float quaternion[4];
  m_CommandInterpreter->GetQuaternion(quaternion);


  typedef TransformType::VersorType RotationType;
  RotationType rotation;
  rotation.Set(quaternion[0],-quaternion[3],quaternion[2],quaternion[1]);

  typedef TransformType::TimePeriodType TimePeriodType;
  const TimePeriodType validityTime = 100.0;

  transform.SetToIdentity(validityTime);
  transform.SetTranslationAndRotation(translation, rotation, 0,
                                      validityTime);

  this->SetToolTransform(0, 0, transform);

  m_BufferLock->Unlock(); 

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalThreadedUpdateStatus( void )
{
  //igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalThreadedUpdateStatus "
  //               "called ...\n");
  return SUCCESS;
}

/** Enable all tool ports that are occupied. */
void FlockOfBirdsTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::EnableToolPorts called...\n");

  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 1;
    }

}

/** Disable all tool ports. */
void FlockOfBirdsTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::DisableToolPorts called...\n");
  
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 0;
    }
}


/** Print Self function */
void FlockOfBirdsTracker::PrintSelf( std::ostream& os, 
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  unsigned int i;

  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] 
       << std::endl;
    }
}


} // end of namespace igstk
