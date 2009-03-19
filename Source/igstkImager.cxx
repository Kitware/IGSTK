/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImager.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug
// information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkImager.h"

namespace igstk
{

/** Constructor */
Imager::Imager(void) :  m_StateMachine( this )
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToEstablishCommunication );
  igstkAddStateMacro( AttemptingToCloseCommunication);
  igstkAddStateMacro( AttemptingToAttachImagerTool );
  igstkAddStateMacro( ImagerToolAttached );
  igstkAddStateMacro( CommunicationEstablished );
  igstkAddStateMacro( AttemptingToTrack );
  igstkAddStateMacro( AttemptingToStopImaging);
  igstkAddStateMacro( Imaging );
  igstkAddStateMacro( AttemptingToUpdate );

  // Set the input descriptors
  igstkAddInputMacro( EstablishCommunication);
  igstkAddInputMacro( AttachImagerTool);
  igstkAddInputMacro( StartImaging);
  igstkAddInputMacro( UpdateStatus);
  igstkAddInputMacro( StopImaging);
  igstkAddInputMacro( Reset);
  igstkAddInputMacro( CloseCommunication);
  igstkAddInputMacro( Success);
  igstkAddInputMacro( Failure);
  igstkAddInputMacro( ValidFrequency);

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           EstablishCommunication,
                           AttemptingToEstablishCommunication,
                           AttemptToOpen );
  igstkAddTransitionMacro( Idle,
                           StartImaging,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           StopImaging,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           AttachImagerTool,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           UpdateStatus,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Reset,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           ValidFrequency,
                           Idle,
                           SetFrequency );

  // Transitions from the AttemptingToEstablishCommunication
  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Success,
                           CommunicationEstablished,
                           CommunicationEstablishmentSuccess );

  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Failure,
                           Idle,
                           CommunicationEstablishmentFailure );

  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           ValidFrequency,
                           AttemptingToEstablishCommunication,
                           ReportInvalidRequest );

  // Transitions from CommunicationEstablished
  igstkAddTransitionMacro( CommunicationEstablished,
                           AttachImagerTool,
                           AttemptingToAttachImagerTool,
                           AttemptToAttachImagerTool );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StartImaging,
                           AttemptingToTrack,
                           AttemptToStartImaging );

  igstkAddTransitionMacro( CommunicationEstablished,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           Reset,
                           CommunicationEstablished,
                           ResetFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StopImaging,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           EstablishCommunication,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           UpdateStatus,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           ValidFrequency,
                           CommunicationEstablished,
                           SetFrequency );

  // Transitions from AttemptingToAttachImagerTool
  igstkAddTransitionMacro( AttemptingToAttachImagerTool,
                           Success,
                           ImagerToolAttached,
                           AttachingImagerToolSuccess );

  igstkAddTransitionMacro( AttemptingToAttachImagerTool,
                           Failure,
                           CommunicationEstablished,
                           AttachingImagerToolFailure );

  igstkAddTransitionMacro( AttemptingToAttachImagerTool,
                           ValidFrequency,
                           AttemptingToAttachImagerTool,
                           ReportInvalidRequest );

  // Transitions from ImagerToolAttached
  igstkAddTransitionMacro( ImagerToolAttached,
                           StartImaging,
                           AttemptingToTrack,
                           AttemptToStartImaging );

  igstkAddTransitionMacro( ImagerToolAttached,
                           AttachImagerTool,
                           AttemptingToAttachImagerTool,
                           AttemptToAttachImagerTool );

  igstkAddTransitionMacro( ImagerToolAttached,
                           ValidFrequency,
                           ImagerToolAttached,
                           SetFrequency );

  igstkAddTransitionMacro( ImagerToolAttached,
                           StopImaging,
                           ImagerToolAttached,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImagerToolAttached,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  // Transitions from AttemptingToTrack
  igstkAddTransitionMacro( AttemptingToTrack,
                           Success,
                           Imaging,
                           StartImagingSuccess );

  igstkAddTransitionMacro( AttemptingToTrack,
                           Failure,
                           CommunicationEstablished,
                           StartImagingFailure );

  igstkAddTransitionMacro( AttemptingToTrack,
                           ValidFrequency,
                           AttemptingToTrack,
                           ReportInvalidRequest );

  // Transitions from Imaging
  igstkAddTransitionMacro( Imaging,
                           UpdateStatus,
                           AttemptingToUpdate,
                           AttemptToUpdateStatus );

  igstkAddTransitionMacro( Imaging,
                           StopImaging,
                           AttemptingToStopImaging,
                           AttemptToStopImaging );

  igstkAddTransitionMacro( Imaging,
                           Reset,
                           CommunicationEstablished,
                           ResetFromImagingState );

  igstkAddTransitionMacro( Imaging,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromImagingState );

  igstkAddTransitionMacro( Imaging,
                           ValidFrequency,
                           Imaging,
                           ReportInvalidRequest );

  // Transitions from AttemptingToUpdate
  igstkAddTransitionMacro( AttemptingToUpdate,
                           Success,
                           Imaging,
                           UpdateStatusSuccess );

  igstkAddTransitionMacro( AttemptingToUpdate,
                           Failure,
                           Imaging,
                           UpdateStatusFailure );

  igstkAddTransitionMacro( AttemptingToUpdate,
                           ValidFrequency,
                           AttemptingToUpdate,
                           ReportInvalidRequest );

  // Transitions from AttemptingToStopImaging
  igstkAddTransitionMacro( AttemptingToStopImaging,
                           Success,
                           CommunicationEstablished,
                           StopImagingSuccess );

  igstkAddTransitionMacro( AttemptingToStopImaging,
                           Failure,
                           Imaging,
                           StopImagingFailure );

  igstkAddTransitionMacro( AttemptingToStopImaging,
                           ValidFrequency,
                           AttemptingToStopImaging,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Success,
                           Idle,
                           CloseCommunicationSuccess );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Failure,
                           CommunicationEstablished,
                           CloseCommunicationFailure );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           ValidFrequency,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create a PulseGenerator object.
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Imager::UpdateStatus );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  // This is update rate for sending imaging information to the
  // spatial objects, it should be set to at least 30 Hz
  const double DEFAULT_REFRESH_RATE = 30.0;
  m_PulseGenerator->RequestSetFrequency( DEFAULT_REFRESH_RATE );

  // This is the time period for which transformation should be
  // considered valid.  After this time, they expire.  This time
  // is in milliseconds. The default validity time is computed
  // from the default refresh rate and nonflickering constant
  const double nonFlickeringConstant = 10;

  const TimePeriodType DEFAULT_VALIDITY_TIME =
                    ( 1000.0 /DEFAULT_REFRESH_RATE) + nonFlickeringConstant ;
  m_ValidityTime = DEFAULT_VALIDITY_TIME;

  m_ConditionNextTransformReceived = itk::ConditionVariable::New();
  m_Threader = itk::MultiThreader::New();
  m_ThreadingEnabled = false;
  m_ImagingThreadStarted = false;

//TODO delete this
    std::ofstream ofile;
    ofile.open("ImagerStateMachineDiagram.dot");
    const bool skipLoops = false;
    this->ExportStateMachineDescription( ofile, skipLoops );
    ofile.close();

}

/** Destructor */
Imager::~Imager(void)
{
}


/** The "RequestOpen" method attempts to open communication with the
 *  imaging device. */
void Imager::RequestOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestOpen called...\n");
  igstkPushInputMacro( EstablishCommunication );
  this->m_StateMachine.ProcessInputs();
}


/** The "RequestClose" method closes communication with the device. */
void Imager::RequestClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestClose called ...\n");
  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();
}

/** The "RequestReset" imager method should be used to set the imager
 * to some defined default state. */
void Imager::RequestReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestReset called ...\n");
  igstkPushInputMacro( Reset );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStartImaging" method readies the imager for imaging the
 *  tools connected to the imager. */
void Imager::RequestStartImaging( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestStartImaging called ...\n");
  igstkPushInputMacro( StartImaging );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStopImaging" stops imager from imaging the tools. */
void Imager::RequestStopImaging( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestStopImaging called ...\n");
  igstkPushInputMacro( StopImaging );
  m_StateMachine.ProcessInputs();
}


/** The "UpdateStatus" method is used for updating the status of
 *  tools when the imager is in imaging state. */
void Imager::UpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::UpdateStatus called ...\n");
  igstkPushInputMacro( UpdateStatus );
  m_StateMachine.ProcessInputs();
}


/** The "RequestSetFrequency" method is used for defining the rate at which
 * Transforms are queried from the Imager device */
void Imager::RequestSetFrequency( double frequencyInHz )
{
  igstkLogMacro( DEBUG, "igstk::Imager::RequestSetFrequency called ...\n");
  if( this->ValidateSpecifiedFrequency( frequencyInHz ) )
    {
    this->m_FrequencyToBeSet = frequencyInHz;
    igstkPushInputMacro( ValidFrequency );
    m_StateMachine.ProcessInputs();
    }
}

/** The "ValidateFrequency" method checks if the specified frequency is
 * valid for the imaging device that is being used. This method is to be
 * overridden in the derived imaging-device specific classes to take
 * into account the maximum frequency possible in the imaging device */
Imager::ResultType
Imager::ValidateSpecifiedFrequency( double frequencyInHz )
{
  if ( frequencyInHz < 0.0 )
    {
    return FAILURE;
    }
  return SUCCESS;
}

/** The "AttemptToOpen" method attempts to open communication with a
 *  imaging device. */
void Imager::AttemptToOpenProcessing( void )
{
  igstkLogMacro( DEBUG,
                 "igstk::Imager::AttemptToOpenProcessing called ...\n");

  ResultType result = this->InternalOpen();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after communication setup has been successful. */
void Imager::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::CommunicationEstablishmentSuccessProcessing called ...\n");

  this->InvokeEvent( ImagerOpenEvent() );
}


/** Post-processing after communication setup has failed. */
void Imager::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");

  this->InvokeEvent( ImagerOpenErrorEvent() );
}

/** The Reset methods force the imager to the
 *  CommunicationEstablished state */
void Imager::ResetFromImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "ResetFromImagingStateProcessing() called ...\n");
  // leaving ImagingState, going to CommunicationEstablishedState
  this->ExitImagingStateProcessing();
  this->ResetFromToolsActiveStateProcessing();
}

/** The Reset methods force the imager to the
 *  CommunicationEstablished  state */
void Imager::ResetFromToolsActiveStateProcessing( void )
{
  igstkLogMacro( DEBUG,
         "igstk::Imager::ResetFromToolsActiveStateProcessing() called ...\n");
  this->ResetFromCommunicatingStateProcessing();
}

/** The Reset methods force the imager to the
 *  CommunicationEstablished state */
void Imager::ResetFromCommunicatingStateProcessing( void )
{
  ResultType result = this->InternalReset();

  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::Imager::InternalReset succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::Imager::InternalReset failed ...\n");
    }
}

/** Post-processing after tools setup has been successful. */
void Imager::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::ToolsActivationSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( ImagerInitializeEvent() );
}

/** Post-processing after tools setup has failed. */
void Imager::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::ToolsActivationFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( ImagerInitializeErrorEvent() );
}

/** The "AttemptToStartImaging" method attempts to start imaging. */
void Imager::AttemptToStartImagingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::AttemptToStartImagingProcessing  "
                 "called ...\n");

  ResultType result = this->InternalStartImaging();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after start imaging has been successful. */
void Imager::StartImagingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::StartImagingSuccessProcessing "
                 "called ...\n");

  // Report to all the imager tools that imaging has been started
  typedef ImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_ImagerTools.begin();
  InputConstIterator inputEnd = m_ImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportImagingStarted();
    ++inputItr;
    }


  // going from AttemptingToTrackState to ImagingState
  this->EnterImagingStateProcessing();

  this->InvokeEvent( ImagerStartImagingEvent() );
}

/** Post-processing after start imaging has failed. */
void Imager::StartImagingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::StartImagingFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( ImagerStartImagingErrorEvent() );
}

/** Post-processing after attaching a imager tool to the imager
 *  has been successful. */
void Imager::AttachingImagerToolSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::AttachingImagerToolSuccessProcessing "
                 "called ...\n");

  m_ImagerTools[ m_ImagerToolToBeAttached->GetImagerToolIdentifier() ]
                                   = m_ImagerToolToBeAttached;

  // report to the imager tool that the attachment has been
  // successful
  m_ImagerToolToBeAttached->RequestReportSuccessfulImagerToolAttachment();

  // Add the imager tool to the internal data containers
  this->AddImagerToolToInternalDataContainers( m_ImagerToolToBeAttached );

  //connect the imager tool coordinate system to the imager
  //system. By default, make the imager coordinate system to
  //be a parent of the imager tool coordinate system

  TransformType identityTransform;
  identityTransform.SetToIdentity(
                  igstk::TimeStamp::GetZeroValue() );

  m_ImagerToolToBeAttached->RequestSetTransformAndParent(
    identityTransform, this );
}

/** Post-processing after attaching a imager tool to the imager
 *  has failed. */
void Imager::AttachingImagerToolFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::AttachingImagerToolFailureProcessing "
                 "called ...\n");

  // report to the imager tool that the attachment has failed
  m_ImagerToolToBeAttached->RequestReportFailedImagerToolAttachment();
}

/** The "AttemptToStopImaging" method attempts to stop imaging. */
void Imager::AttemptToStopImagingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::AttemptToStopImagingProcessing "
                        "called ...\n");
  // leaving ImagingState, going to AttemptingToStopImagingState
  this->ExitImagingStateProcessing();

  ResultType result = this->InternalStopImaging();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after stop imaging has been successful. */
void Imager::StopImagingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::StopImagingSuccessProcessing "
                 "called ...\n");

  // Report to all the imager tools that imaging has been stopped
  typedef ImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_ImagerTools.begin();
  InputConstIterator inputEnd = m_ImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportImagingStopped();
    ++inputItr;
    }

  this->InvokeEvent( ImagerStopImagingEvent() );
}

/** Post-processing after start imaging has failed. */
void Imager::StopImagingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::StopImagingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopImagingState to ImagingState
  this->EnterImagingStateProcessing();

  this->InvokeEvent( ImagerStopImagingErrorEvent() );
}

/** Needs to be called every time when entering imaging state. */
void Imager::EnterImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::EnterImagingStateProcessing "
                 "called ...\n");

  if ( ! m_ImagingThreadStarted && this->GetThreadingEnabled() )
    {
    m_ThreadID = m_Threader->SpawnThread( ImagingThreadFunction, this );
    m_ImagingThreadStarted= true;
    }

  m_PulseGenerator->RequestStart();
}

/** Needs to be called every time when exiting imaging state. */
void Imager::ExitImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::ExitImagingStateProcessing "
                 "called ...\n");

  // by default, we will exit imaging by terminating imaging thread
  this->ExitImagingTerminatingImagingThread();
}

/** Exit imaging by terminating imaging thread */
void Imager::ExitImagingWithoutTerminatingImagingThread( void )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::ExitImagingWithoutTerminatingImagingThread "
    "called ...\n");

  m_PulseGenerator->RequestStop();
}

/** Exit imaging by terminating imaging thread */
void Imager::ExitImagingTerminatingImagingThread( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::ExitImagingTerminatingImagingThread "
                 "called ...\n");

  m_PulseGenerator->RequestStop();

  // Terminating the ImagingThread.
  if ( this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    m_ImagingThreadStarted = false;
    }
}

/** The "AttemptToUpdateStatus" method attempts to update status
    during imaging. */
void Imager::AttemptToUpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::AttemptToUpdateStatusProcessing "
                        "called ...\n");

  // Set all tools to "not updated"
  //
  typedef ImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_ImagerTools.begin();
  InputConstIterator inputEnd = m_ImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->SetUpdated(false);
    ++inputItr;
    }

  if ( this->GetThreadingEnabled() )
    {
    m_ConditionNextTransformReceived->Wait(
      & m_LockForConditionNextTransformReceived );
    }
  else
    {
    this->InternalThreadedUpdateStatus();
    }

  ResultType result = this->InternalUpdateStatus();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** This method is called when a call to UpdateStatus succeeded */
void Imager::UpdateStatusSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::UpdateStatusSuccessProcessing "
                 "called ...\n");

  typedef ImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_ImagerTools.begin();
  InputConstIterator inputEnd = m_ImagerTools.end();

  while( inputItr != inputEnd )
    {
    if ( (inputItr->second)->GetUpdated() )
      {

       FrameType frame = (inputItr->second)->GetInternalFrame();

       const double timeToExpiration = frame.GetExpirationTime() -
                                      frame.GetStartTime();

       FrameType updatedFrame;
       updatedFrame.SetImagePtr(frame.GetImagePtr(), timeToExpiration);

       (inputItr->second)->SetInternalFrame( updatedFrame );

       //throw an imager update event
       (inputItr->second)->InvokeEvent( ImagerToolFrameUpdateEvent() );

      }
    ++inputItr;
    }

  this->InvokeEvent( ImagerUpdateStatusEvent() );
}

/** This method is called when a UpdateStatus failed */
void Imager::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::UpdateStatusFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( ImagerUpdateStatusErrorEvent() );
}


/** The "CloseFromImagingStateProcessing" method closes imager in
 *  use, when the imager is in imaging state. */
void Imager::CloseFromImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::CloseFromImagingStateProcessing "
                 "called ...\n");

  // leaving ImagingState, going to AttemptingToCloseState
  this->ExitImagingStateProcessing();

  ResultType result = this->InternalStopImaging();

  // detach all the imager tools from the imager
  this->DetachAllImagerToolsFromImager();

  // Terminating the ImagingThread and if it is started
  if ( m_ImagingThreadStarted && this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    m_ImagingThreadStarted = false;
    }


  if( result == SUCCESS )
    {
    result = this->InternalClose();
    }

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Detach all imager tools from the imager */
void Imager::DetachAllImagerToolsFromImager()
{

  typedef ImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_ImagerTools.begin();
  InputConstIterator inputEnd = m_ImagerTools.end();

  while( inputItr != inputEnd )
    {
    this->RemoveImagerToolFromInternalDataContainers( inputItr->second );
    ++inputItr;
    }

  m_ImagerTools.clear();
}

/** The "CloseFromCommunicatingStateProcessing" method closes
 *  imager in use, when the imager is in communicating state. */
void Imager::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

  // Detach all the imager tools from the imager
  this->DetachAllImagerToolsFromImager();

  // Terminating the ImagingThread and if it is started
  if ( m_ImagingThreadStarted && this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    m_ImagingThreadStarted = false;
    }


  ResultType result = this->InternalClose();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after close imaging has been successful. */
void Imager::CloseCommunicationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "CloseCommunicationSuccessProcessing called ...\n");

  this->InvokeEvent( ImagerCloseEvent() );
}

/** Post-processing after close imaging has failed. */
void Imager::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "CloseCommunicationFailureProcessing called ...\n");

  this->InvokeEvent( ImagerCloseErrorEvent() );
}

/** Print object information */
void Imager::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_PulseGenerator )
    {
    os << indent << this->m_PulseGenerator << std::endl;
    }

  if( this->m_PulseObserver )
    {
    os << indent << this->m_PulseObserver << std::endl;
    }

  os << indent << "ValidityTime: " << this->m_ValidityTime << std::endl;
  os << indent << "CoordinateSystemDelegator: ";
  this->m_CoordinateSystemDelegator->PrintSelf( os, indent );
}

/** The "SetFrequencyProcessing" passes the frequency value to the Pulse
 * Generator. Note that it is still possible for the PulseGenerator to reject
 * the value and stay at its current frequency. */
void Imager::SetFrequencyProcessing( void )
{
  igstkLogMacro( DEBUG,
                 "igstk::Imager::SetFrequencyProcessing called ...\n");

  this->m_PulseGenerator->RequestSetFrequency( this->m_FrequencyToBeSet );

  //Set the validity time of the frames based on the imager frequency
  //Add a constant to avoid any flickering effect

  const double nonFlickeringConstant = 10;
  this->m_ValidityTime = (1000/m_FrequencyToBeSet) + nonFlickeringConstant;
}


/** Request adding a tool to the imager  */
void
Imager::
RequestAttachTool( ImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG, "igstk::Imager::"
                 "RequestAttachTool called ...\n");

  m_ImagerToolToBeAttached = imagerTool;

  igstkPushInputMacro( AttachImagerTool );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToAttachImagerToolProcessing" method attempts to
 * add a imager tool to the imager */
void Imager::AttemptToAttachImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::AttemptToAttachImagerToolProcessing called ...\n");

  // Verify the imager tool information before adding it to the
  // imager. The conditions that need be verified depend on
  // the imager type.
  ResultType result =
    this->VerifyImagerToolInformation( m_ImagerToolToBeAttached );

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Request remove a tool from the imager  */
Imager::ResultType
Imager::
RequestRemoveTool( ImagerToolType * imagerTool )
{
  this->m_ImagerTools.erase( imagerTool->GetImagerToolIdentifier() );
  this->RemoveImagerToolFromInternalDataContainers( imagerTool );
  return SUCCESS;
}

const Imager::ImagerToolsContainerType &
Imager::GetImagerToolContainer() const
{
  return m_ImagerTools;
}

/** Thread function for imaging */
ITK_THREAD_RETURN_TYPE Imager::ImagingThreadFunction(void* pInfoStruct)
{
  struct itk::MultiThreader::ThreadInfoStruct * pInfo =
    (struct itk::MultiThreader::ThreadInfoStruct*)pInfoStruct;

  if( pInfo == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  if( pInfo->UserData == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  Imager *pImager = (Imager*)pInfo->UserData;

  // counters for error rates
  unsigned long errorCount = 0;
  unsigned long totalCount = 0;

  int activeFlag = 1;
  while ( activeFlag )
    {
    ResultType result = pImager->InternalThreadedUpdateStatus();
    pImager->m_ConditionNextTransformReceived->Signal();

    totalCount++;
    if (result != SUCCESS)
      {
      errorCount++;
      }

    // check to see if we are being told to quit
    pInfo->ActiveFlagLock->Lock();
    activeFlag = *pInfo->ActiveFlag;
    pInfo->ActiveFlagLock->Unlock();
    }

  igstkLogMacroStatic(pImager, DEBUG, "ImagingThreadFunction was "
                      "terminated, " << errorCount << " errors "
                      "out of " << totalCount << "updates." << std::endl );

  return ITK_THREAD_RETURN_VALUE;
}

/** Report to the imager tool that the tool is not available */
void
Imager::ReportImagingToolNotAvailable( ImagerToolType * imagerTool ) const
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::ReportImagingToolNotAvailable called...\n");
  imagerTool->RequestReportImagingToolNotAvailable();
}

/** Report to the imager tool that the tool is Visible */
void
Imager::ReportImagingToolVisible( ImagerToolType * imagerTool ) const
{
  igstkLogMacro( DEBUG, "igstk::Imager::ReportImagingToolVisible called...\n");
  imagerTool->RequestReportImagingToolVisible();
}

/** Set Imager Tool Frame */
void
Imager::SetImagerToolFrame(
  ImagerToolType * imagerTool, const FrameType& frame )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::SetImagerToolFrame called...\n");
  imagerTool->SetInternalFrame( frame );
}

/** Get Imager Tool Frame */
void Imager::GetImagerToolFrame(
  ImagerToolType * imagerTool, FrameType& frame )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::GetImagerToolFrame called...\n");
  frame = imagerTool->GetInternalFrame( );
}


/** Turn on/off update flag of the imager tool */
void
Imager::SetImagerToolUpdate(
  ImagerToolType * imagerTool, bool flag ) const
{
  igstkLogMacro( DEBUG,
     "igstk::Imager::SetImagerToolUpdate called...\n");
  imagerTool->SetUpdated( flag );
}

/** Report invalid request */
void Imager::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::Imager::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

}
