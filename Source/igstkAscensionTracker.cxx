/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionTracker.cxx
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

#include "igstkAscensionTracker.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
AscensionTracker::AscensionTracker(void):m_StateMachine(this)
{

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
  m_Communication = 0;
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
}

/** Destructor */
AscensionTracker::~AscensionTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the FlockOfBirds */
void AscensionTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "AscensionTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );
  igstkLogMacro( DEBUG, "AscensionTracker:: Exiting SetCommunication ...\n");
}

/** Open communication with the tracking device. */
AscensionTracker::ResultType AscensionTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "AscensionTracker::InternalOpen called ...\n");
  m_CommandInterpreter->Open();
  m_CommandInterpreter->SetFormat(FB_POSITION_QUATERNION);
  m_CommandInterpreter->SetHemisphere(FB_FORWARD);
  m_CommandInterpreter->ChangeValue(FB_GROUP_MODE,0);

  return SUCCESS;
}

/** Close communication with the tracking device. */
AscensionTracker::ResultType AscensionTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "AscensionTracker::InternalClose called ...\n");

  m_CommandInterpreter->Close();  

  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
AscensionTracker::ResultType 
AscensionTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, 
               "AscensionTracker::InternalActivateTools called ...\n");

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
AscensionTracker::ResultType 
AscensionTracker::InternalDeactivateTools( void )
{
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
AscensionTracker::ResultType 
AscensionTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, 
                "AscensionTracker::InternalStartTracking called ...\n");

  if ( m_NumberOfTools > 1 )
    {
    m_CommandInterpreter->FBBReset();
    m_CommandInterpreter->FBBAutoConfig(m_NumberOfTools);
    }

  m_CommandInterpreter->Run();

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
AscensionTracker::ResultType 
AscensionTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
                "AscensionTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->Stop();

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
AscensionTracker::ResultType AscensionTracker::InternalReset( void )
{
  return SUCCESS;
}


/** Update the status and the transforms for all TrackerTools. */
AscensionTracker::ResultType AscensionTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, 
                 "AscensionTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  TrackerToolsContainerType trackerToolContainer =
      this->GetTrackerToolContainer();

  while( inputItr != inputEnd )
    {

    // only report tools that are in view
    // this is actually not working because the ascension system is always
    // reporting some data.
    // TODO:
    // decide "tool visibility" in terms of signal fidelity. There should be
    // some parameter that gives that value. I saw it in the pciCubes 
    // application
    if (! this->m_ToolStatusContainer[inputItr->first])
      {
      igstkLogMacro( INFO, 
            "igstk::FlockOfBirdTracker::InternalUpdateStatus: " <<
            "tool " << inputItr->first << " is not in view\n");

       // report to the tracker tool that the tracker is not available
       this->ReportTrackingToolNotAvailable(
            trackerToolContainer[inputItr->first]);

       ++inputItr;
       continue;
      }

    // report to the tracker tool that the tracker is Visible
    this->ReportTrackingToolVisible(trackerToolContainer[inputItr->first]);

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = (inputItr->second)[0];
    translation[1] = (inputItr->second)[1];
    translation[2] = (inputItr->second)[2];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;

    const double normsquared =
        (inputItr->second)[3]*(inputItr->second)[3] +
        (inputItr->second)[4]*(inputItr->second)[4] +
        (inputItr->second)[5]*(inputItr->second)[5] +
        (inputItr->second)[6]*(inputItr->second)[6];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
        rotation.Set(0.0, 0.0, 0.0, 1.0);
        igstkLogMacro( WARNING, 
            "igstk::AscensionTracker::InternUpdateStatus: bad "
            "quaternion, norm = " << sqrt(normsquared) << "\n");
      }
    else
      {
        rotation.Set((inputItr->second)[3],
                     (inputItr->second)[4],
                     (inputItr->second)[5],
                     (inputItr->second)[6]);
      }

    // report error value
    // Get error value from the tracker. TODO
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.0;

    transform.SetToIdentity(this->GetValidityTime());
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
        this->GetValidityTime());

    // set the raw transform
    this->SetTrackerToolRawTransform(
        trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate(
        trackerToolContainer[inputItr->first], true );

    ++inputItr;

    }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
AscensionTracker::ResultType 
AscensionTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "AscensionTracker::InternalThreadedUpdateStatus "
          "called ...\n");

  // sebastian ordas comments:
  // cannot fix a 1-2 sec delay present when using more than one bird
  // I tried everything: point/stream, group mode, different positions
  // of RS232ToFBB, etc. It's only a delay. It seems we are actually 
  // not loosing samples (after the delay, the sensor movement is 
  // reproduced without jumps). It's like the serial port buffer is not flushed

  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;
  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  while( inputItr != inputEnd )
  {
    this->m_ToolStatusContainer[inputItr->first] = 0;

    if ( inputItr->first == "1" )
       m_CommandInterpreter->RS232ToFBB(1);
    if ( inputItr->first == "2" )
       m_CommandInterpreter->RS232ToFBB(2);
    if ( inputItr->first == "3" )
       m_CommandInterpreter->RS232ToFBB(3);
    if ( inputItr->first == "4" )
       m_CommandInterpreter->RS232ToFBB(4);

    m_CommandInterpreter->Point();
    m_CommandInterpreter->Update();

    float offset[3];
    m_CommandInterpreter->GetPosition(offset);

    float quaternion[4];
    m_CommandInterpreter->GetQuaternion(quaternion);

    std::vector< double > transform;

    transform.push_back( offset[0] ); 
    transform.push_back( offset[1] );
    transform.push_back( offset[2] );

    // fob quaternion q0, q1, q2, and q3 where q0
    // is the scaler component
    // itk versor: void Set( T x, T y, T z, T w );
    transform.push_back( quaternion[0] );
    transform.push_back( quaternion[3] );
    transform.push_back( quaternion[2] );
    transform.push_back( quaternion[1] );

    
    this->m_ToolTransformBuffer[ inputItr->first ] = transform;
    this->m_ToolStatusContainer[ inputItr->first ] = 1;

    inputItr++;
    } 

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Enable all tool ports that are occupied. */
void AscensionTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "AscensionTracker::EnableToolPorts called...\n");
}

/** Disable all tool ports. */
void AscensionTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "AscensionTracker::DisableToolPorts called...\n");
}

/** Verify tracker tool information */
AscensionTracker::ResultType
AscensionTracker
::VerifyTrackerToolInformation(const TrackerToolType * itkNotUsed(trackerTool))
{
  igstkLogMacro( DEBUG, 
    "AscensionTracker::VerifyTrackerToolInformation called...\n");
  return SUCCESS;
}

/** Remove tracker tool from internal containers */
AscensionTracker::ResultType
AscensionTracker
::RemoveTrackerToolFromInternalDataContainers( 
  const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "AscensionTracker::RemoveTrackerToolFromInternalDataContainers"
    " called...\n");

  const std::string trackerToolIdentifier =
       trackerTool->GetTrackerToolIdentifier();

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

AscensionTracker::ResultType
AscensionTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG,
      "igstk::AscensionTracker::AddTrackerToolToInternalDataContainers "
      "called ...\n");

  if ( trackerTool == NULL )
    {
    return FAILURE;
    }

  const std::string trackerToolIdentifier =
      trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );

  this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;
  this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

  m_NumberOfTools ++;

  return SUCCESS;
}

/** Print Self function */
void AscensionTracker::PrintSelf( std::ostream& os, 
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end of namespace igstk
