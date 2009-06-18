/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImager.cxx
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

#include "igstkVideoImager.h"

namespace igstk
{

/** Constructor */
VideoImager::VideoImager(void) :  m_StateMachine( this )
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToEstablishCommunication );
  igstkAddStateMacro( AttemptingToCloseCommunication);
  igstkAddStateMacro( AttemptingToAttachVideoImagerTool );
  igstkAddStateMacro( VideoImagerToolAttached );
  igstkAddStateMacro( CommunicationEstablished );
  igstkAddStateMacro( AttemptingToImaging );
  igstkAddStateMacro( AttemptingToStopImaging);
  igstkAddStateMacro( Imaging );
  igstkAddStateMacro( AttemptingToUpdate );

  // Set the input descriptors
  igstkAddInputMacro( EstablishCommunication);
  igstkAddInputMacro( AttachVideoImagerTool);
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
                           AttachVideoImagerTool,
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
                           AttachVideoImagerTool,
                           AttemptingToAttachVideoImagerTool,
                           AttemptToAttachVideoImagerTool );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StartImaging,
                           AttemptingToImaging,
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

  // Transitions from AttemptingToAttachVideoImagerTool
  igstkAddTransitionMacro( AttemptingToAttachVideoImagerTool,
                           Success,
                           VideoImagerToolAttached,
                           AttachingVideoImagerToolSuccess );

  igstkAddTransitionMacro( AttemptingToAttachVideoImagerTool,
                           Failure,
                           CommunicationEstablished,
                           AttachingVideoImagerToolFailure );

  igstkAddTransitionMacro( AttemptingToAttachVideoImagerTool,
                           ValidFrequency,
                           AttemptingToAttachVideoImagerTool,
                           ReportInvalidRequest );

  // Transitions from VideoImagerToolAttached
  igstkAddTransitionMacro( VideoImagerToolAttached,
                           StartImaging,
                           AttemptingToImaging,
                           AttemptToStartImaging );

  igstkAddTransitionMacro( VideoImagerToolAttached,
                           AttachVideoImagerTool,
                           AttemptingToAttachVideoImagerTool,
                           AttemptToAttachVideoImagerTool );

  igstkAddTransitionMacro( VideoImagerToolAttached,
                           ValidFrequency,
                           VideoImagerToolAttached,
                           SetFrequency );

  igstkAddTransitionMacro( VideoImagerToolAttached,
                           StopImaging,
                           VideoImagerToolAttached,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( VideoImagerToolAttached,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  // Transitions from AttemptingToImaging
  igstkAddTransitionMacro( AttemptingToImaging,
                           Success,
                           Imaging,
                           StartImagingSuccess );

  igstkAddTransitionMacro( AttemptingToImaging,
                           Failure,
                           CommunicationEstablished,
                           StartImagingFailure );

  igstkAddTransitionMacro( AttemptingToImaging,
                           ValidFrequency,
                           AttemptingToImaging,
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
  m_PulseObserver->SetCallbackFunction( this, & VideoImager::UpdateStatus );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  // This is update rate for sending imaging information to the
  // spatial objects, it should be set to at least 25 Hz
  const double DEFAULT_REFRESH_RATE = 25.0;
  m_PulseGenerator->RequestSetFrequency( DEFAULT_REFRESH_RATE );

  // This is the time period for which transformation should be
  // considered valid.  After this time, they expire.  This time
  // is in milliseconds. The default validity time is computed
  // from the default refresh rate and nonflickering constant
  const double nonFlickeringConstant = 10;

  const TimePeriodType DEFAULT_VALIDITY_TIME =
                    ( 1000.0 /DEFAULT_REFRESH_RATE) + nonFlickeringConstant;
  m_ValidityTime = DEFAULT_VALIDITY_TIME;

  m_ConditionNextFrameReceived = itk::ConditionVariable::New();
  m_Threader = itk::MultiThreader::New();
  m_ThreadingEnabled = false;
  m_ImagingThreadStarted = false;

  std::ofstream ofile;
  ofile.open("VideoImagerStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();

}

/** Destructor */
VideoImager::~VideoImager(void)
{
}

/** The "RequestOpen" method attempts to open communication with the
 *  imaging device. */
void VideoImager::RequestOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestOpen called...\n");
  igstkPushInputMacro( EstablishCommunication );
  this->m_StateMachine.ProcessInputs();
}

/** The "RequestClose" method closes communication with the device. */
void VideoImager::RequestClose( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestClose called ...\n");
  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();
}

/** The "RequestReset" VideoImager method should be used to set the VideoImager
 * to some defined default state. */
void VideoImager::RequestReset( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestReset called ...\n");
  igstkPushInputMacro( Reset );
  m_StateMachine.ProcessInputs();
}

/** The "RequestStartImaging" method readies the VideoImager for imaging the
 *  tools connected to the VideoImager. */
void VideoImager::RequestStartImaging( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestStartImaging called ...\n");
  igstkPushInputMacro( StartImaging );
  m_StateMachine.ProcessInputs();
}

/** The "RequestStopImaging" stops VideoImager from imaging the tools. */
void VideoImager::RequestStopImaging( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestStopImaging called ...\n");
  igstkPushInputMacro( StopImaging );
  m_StateMachine.ProcessInputs();
}

/** The "UpdateStatus" method is used for updating the status of
 *  tools when the VideoImager is in imaging state. */
void VideoImager::UpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::UpdateStatus called ...\n");
  igstkPushInputMacro( UpdateStatus );
  m_StateMachine.ProcessInputs();
}

/** The "RequestSetFrequency" method is used for defining the rate at which
 * Transforms are queried from the VideoImager device */
void VideoImager::RequestSetFrequency( double frequencyInHz )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::RequestSetFrequency called ...\n");
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
VideoImager::ResultType
VideoImager::ValidateSpecifiedFrequency( double frequencyInHz )
{
  if ( frequencyInHz < 0.0 )
    {
    return FAILURE;
    }
  return SUCCESS;
}

/** The "AttemptToOpen" method attempts to open communication with a
 *  imaging device. */
void VideoImager::AttemptToOpenProcessing( void )
{
  igstkLogMacro( DEBUG,
                 "igstk::VideoImager::AttemptToOpenProcessing called ...\n");

  ResultType result = this->InternalOpen();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after communication setup has been successful. */
void VideoImager::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager"
               << "::CommunicationEstablishmentSuccessProcessing called ...\n");

  this->InvokeEvent( VideoImagerOpenEvent() );
}

/** Post-processing after communication setup has failed. */
void VideoImager::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");

  this->InvokeEvent( VideoImagerOpenErrorEvent() );
}

/** The Reset methods force the VideoImager to the
 *  CommunicationEstablished state */
void VideoImager::ResetFromImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "ResetFromImagingStateProcessing() called ...\n");
  // leaving ImagingState, going to CommunicationEstablishedState
  this->ExitImagingStateProcessing();
  this->ResetFromToolsActiveStateProcessing();
}

/** The Reset methods force the VideoImager to the
 *  CommunicationEstablished  state */
void VideoImager::ResetFromToolsActiveStateProcessing( void )
{
  igstkLogMacro( DEBUG,
      "igstk::VideoImager::ResetFromToolsActiveStateProcessing() called ...\n");
  this->ResetFromCommunicatingStateProcessing();
}

/** The Reset methods force the VideoImager to the
 *  CommunicationEstablished state */
void VideoImager::ResetFromCommunicatingStateProcessing( void )
{
  ResultType result = this->InternalReset();

  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::VideoImager::InternalReset succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::VideoImager::InternalReset failed ...\n");
    }
}

/** Post-processing after tools setup has been successful. */
void VideoImager::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::ToolsActivationSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( VideoImagerInitializeEvent() );
}

/** Post-processing after tools setup has failed. */
void VideoImager::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::ToolsActivationFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( VideoImagerInitializeErrorEvent() );
}

/** The "AttemptToStartImaging" method attempts to start imaging. */
void VideoImager::AttemptToStartImagingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::AttemptToStartImagingProcessing  "
                 "called ...\n");

  ResultType result = this->InternalStartImaging();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after start imaging has been successful. */
void VideoImager::StartImagingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::StartImagingSuccessProcessing "
                 "called ...\n");

  // Report to all the VideoImager tools that imaging has been started
  typedef VideoImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_VideoImagerTools.begin();
  InputConstIterator inputEnd = m_VideoImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportImagingStarted();
    ++inputItr;
    }


  // going from AttemptingToImagingState to ImagingState
  this->EnterImagingStateProcessing();

  this->InvokeEvent( VideoImagerStartImagingEvent() );
}

/** Post-processing after start imaging has failed. */
void VideoImager::StartImagingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::StartImagingFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( VideoImagerStartImagingErrorEvent() );
}

/** Post-processing after attaching a VideoImager tool to the VideoImager
 *  has been successful. */
void VideoImager::AttachingVideoImagerToolSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
  "igstk::VideoImager::AttachingVideoImagerToolSuccessProcessing called ...\n");

  m_VideoImagerTools[
                 m_VideoImagerToolToBeAttached->GetVideoImagerToolIdentifier() ]
                                                = m_VideoImagerToolToBeAttached;

  // report to the VideoImager tool that the attachment has been
  // successful
  m_VideoImagerToolToBeAttached
                           ->RequestReportSuccessfulVideoImagerToolAttachment();

  // Add the VideoImager tool to the internal data containers
  this->AddVideoImagerToolToInternalDataContainers(
                                                m_VideoImagerToolToBeAttached );

  //connect the VideoImager tool coordinate system to the VideoImager
  //system. By default, make the VideoImager coordinate system to
  //be a parent of the VideoImager tool coordinate system

  TransformType identityTransform;
  identityTransform.SetToIdentity(
                  igstk::TimeStamp::GetZeroValue() );

  m_VideoImagerToolToBeAttached->RequestSetTransformAndParent(
    identityTransform, this );
}

/** Post-processing after attaching a VideoImager tool to the VideoImager
 *  has failed. */
void VideoImager::AttachingVideoImagerToolFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
  "igstk::VideoImager::AttachingVideoImagerToolFailureProcessing called ...\n");

  // report to the VideoImager tool that the attachment has failed
  m_VideoImagerToolToBeAttached->RequestReportFailedVideoImagerToolAttachment();
}

/** The "AttemptToStopImaging" method attempts to stop imaging. */
void VideoImager::AttemptToStopImagingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::AttemptToStopImagingProcessing "
                        "called ...\n");
  // leaving ImagingState, going to AttemptingToStopImagingState
  this->ExitImagingStateProcessing();

  ResultType result = this->InternalStopImaging();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after stop imaging has been successful. */
void VideoImager::StopImagingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::StopImagingSuccessProcessing "
                 "called ...\n");

  // Report to all the VideoImager tools that imaging has been stopped
  typedef VideoImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_VideoImagerTools.begin();
  InputConstIterator inputEnd = m_VideoImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->RequestReportImagingStopped();
    ++inputItr;
    }

  this->InvokeEvent( VideoImagerStopImagingEvent() );
}

/** Post-processing after start imaging has failed. */
void VideoImager::StopImagingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::StopImagingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopImagingState to ImagingState
  this->EnterImagingStateProcessing();

  this->InvokeEvent( VideoImagerStopImagingErrorEvent() );
}

/** Needs to be called every time when entering imaging state. */
void VideoImager::EnterImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::EnterImagingStateProcessing "
                 "called ...\n");

  if ( ! m_ImagingThreadStarted && this->GetThreadingEnabled() )
    {
    m_ThreadID = m_Threader->SpawnThread( ImagingThreadFunction, this );
    m_ImagingThreadStarted= true;
    }

  m_PulseGenerator->RequestStart();
}

/** Needs to be called every time when exiting imaging state. */
void VideoImager::ExitImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::ExitImagingStateProcessing "
                 "called ...\n");

  // by default, we will exit imaging by terminating imaging thread
  this->ExitImagingTerminatingImagingThread();
}

/** Exit imaging without terminating imaging thread */
void VideoImager::ExitImagingWithoutTerminatingImagingThread( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImager::ExitImagingWithoutTerminatingImagingThread "
    "called ...\n");

  m_PulseGenerator->RequestStop();
}

/** Exit imaging by terminating imaging thread */
void VideoImager::ExitImagingTerminatingImagingThread( void )
{
  igstkLogMacro( DEBUG,
        "igstk::VideoImager::ExitImagingTerminatingImagingThread called ...\n");

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
void VideoImager::AttemptToUpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::AttemptToUpdateStatusProcessing "
                        "called ...\n");

  // Set all tools to "not updated"
  typedef VideoImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_VideoImagerTools.begin();
  InputConstIterator inputEnd = m_VideoImagerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->SetUpdated(false);
    ++inputItr;
    }

  if ( this->GetThreadingEnabled() )
    {
    m_ConditionNextFrameReceived->Wait(
      & m_LockForConditionNextFrameReceived );
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
void VideoImager::UpdateStatusSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::UpdateStatusSuccessProcessing "
                 "called ...\n");

  typedef VideoImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_VideoImagerTools.begin();
  InputConstIterator inputEnd = m_VideoImagerTools.end();

  while( inputItr != inputEnd )
    {
    if ( (inputItr->second)->GetUpdated() )
        {

       FrameType* frame = (inputItr->second)->GetInternalFrame();

      const double timeToExpiration = frame->GetExpirationTime() -
                                      frame->GetStartTime();

      FrameType* updatedFrame = new FrameType();
      updatedFrame->SetImagePtr(frame->GetImagePtr(), timeToExpiration);

      (inputItr->second)->SetInternalFrame( updatedFrame );

      (inputItr->second)->InvokeEvent( FrameModifiedEvent() );
      }
    ++inputItr;
    }

  this->InvokeEvent( VideoImagerUpdateStatusEvent() );
}

/** This method is called when a UpdateStatus failed */
void VideoImager::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::UpdateStatusFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( VideoImagerUpdateStatusErrorEvent() );
}

/** The "CloseFromImagingStateProcessing" method closes VideoImager in
 *  use, when the VideoImager is in imaging state. */
void VideoImager::CloseFromImagingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::CloseFromImagingStateProcessing "
                 "called ...\n");

  // leaving ImagingState, going to AttemptingToCloseState
  this->ExitImagingStateProcessing();

  ResultType result = this->InternalStopImaging();

  // detach all the VideoImager tools from the VideoImager
  this->DetachAllVideoImagerToolsFromVideoImager();

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

/** Detach all VideoImager tools from the VideoImager */
void VideoImager::DetachAllVideoImagerToolsFromVideoImager()
{
  typedef VideoImagerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_VideoImagerTools.begin();
  InputConstIterator inputEnd = m_VideoImagerTools.end();

  while( inputItr != inputEnd )
    {
    this->RemoveVideoImagerToolFromInternalDataContainers( inputItr->second );
    ++inputItr;
    }

  m_VideoImagerTools.clear();
}

/** The "CloseFromCommunicatingStateProcessing" method closes
 *  VideoImager in use, when the VideoImager is in communicating state. */
void VideoImager::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

  // Detach all the VideoImager tools from the VideoImager
  this->DetachAllVideoImagerToolsFromVideoImager();

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
void VideoImager::CloseCommunicationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "CloseCommunicationSuccessProcessing called ...\n");

  this->InvokeEvent( VideoImagerCloseEvent() );
}

/** Post-processing after close imaging has failed. */
void VideoImager::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "CloseCommunicationFailureProcessing called ...\n");

  this->InvokeEvent( VideoImagerCloseErrorEvent() );
}

/** Print object information */
void VideoImager::PrintSelf( std::ostream& os, itk::Indent indent ) const
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
void VideoImager::SetFrequencyProcessing( void )
{
  igstkLogMacro( DEBUG,
                 "igstk::VideoImager::SetFrequencyProcessing called ...\n");

  this->m_PulseGenerator->RequestSetFrequency( this->m_FrequencyToBeSet );

  //Set the validity time of the frames based on the VideoImager frequency
  //Add a constant to avoid any flickering effect

  const double nonFlickeringConstant = 10;
  this->m_ValidityTime = (1000/m_FrequencyToBeSet) + nonFlickeringConstant;
}

/** Request adding a tool to the VideoImager  */
void
VideoImager::
RequestAttachTool( VideoImagerToolType * VideoImagerTool )
{
  igstkLogMacro( DEBUG, "igstk::VideoImager::"
                 "RequestAttachTool called ...\n");

  m_VideoImagerToolToBeAttached = VideoImagerTool;

  igstkPushInputMacro( AttachVideoImagerTool );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToAttachVideoImagerToolProcessing" method attempts to
 * add a VideoImager tool to the VideoImager */
void VideoImager::AttemptToAttachVideoImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG,
   "igstk::VideoImager::AttemptToAttachVideoImagerToolProcessing called ...\n");

  // Verify the VideoImager tool information before adding it to the
  // VideoImager. The conditions that need be verified depend on
  // the VideoImager type.
  ResultType result =
    this->VerifyVideoImagerToolInformation( m_VideoImagerToolToBeAttached );

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Request remove a tool from the VideoImager  */
VideoImager::ResultType
VideoImager::
RequestRemoveTool( VideoImagerToolType * VideoImagerTool )
{
  this->m_VideoImagerTools.erase(
                              VideoImagerTool->GetVideoImagerToolIdentifier() );
  this->RemoveVideoImagerToolFromInternalDataContainers( VideoImagerTool );
  return SUCCESS;
}

const VideoImager::VideoImagerToolsContainerType &
VideoImager::GetVideoImagerToolContainer() const
{
  return m_VideoImagerTools;
}

/** Thread function for imaging */
ITK_THREAD_RETURN_TYPE VideoImager::ImagingThreadFunction(void* pInfoStruct)
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

  VideoImager *pVideoImager = (VideoImager*)pInfo->UserData;

  // counters for error rates
  unsigned long errorCount = 0;
  unsigned long totalCount = 0;

  int activeFlag = 1;
  while ( activeFlag )
    {
    ResultType result = pVideoImager->InternalThreadedUpdateStatus();
    pVideoImager->m_ConditionNextFrameReceived->Signal();

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

  igstkLogMacroStatic(pVideoImager, DEBUG, "ImagingThreadFunction was "
                      "terminated, " << errorCount << " errors "
                      "out of " << totalCount << "updates." << std::endl );

  return ITK_THREAD_RETURN_VALUE;
}

/** Report to the VideoImager tool that the tool is not available */
void
VideoImager
::ReportImagingToolNotAvailable( VideoImagerToolType * videoImagerTool ) const
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImager::ReportImagingToolNotAvailable called...\n");
  videoImagerTool->RequestReportImagingToolNotAvailable();
}

/** Report to the VideoImager tool that the tool is Streaming */
void
VideoImager
::ReportImagingToolStreaming( VideoImagerToolType * videoImagerTool ) const
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImager::ReportImagingToolStreaming called...\n");
  videoImagerTool->RequestReportImagingToolStreaming();
}

/** Set VideoImager Tool Frame */
void
VideoImager::SetVideoImagerToolFrame(
  VideoImagerToolType * videoImagerTool, FrameType* frame )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImager::SetVideoImagerToolFrame called...\n");
  videoImagerTool->SetInternalFrame( frame );
}

/** Get VideoImager Tool Frame */
igstk::Frame* VideoImager::GetVideoImagerToolFrame(
  VideoImagerToolType * videoImagerTool)
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImager::GetVideoImagerToolFrame called...\n");
  return videoImagerTool->GetInternalFrame( );
}

/** Turn on/off update flag of the VideoImager tool */
void
VideoImager::SetVideoImagerToolUpdate(
  VideoImagerToolType * videoImagerTool, bool flag ) const
{
  igstkLogMacro( DEBUG,
     "igstk::VideoImager::SetVideoImagerToolUpdate called...\n");
  videoImagerTool->SetUpdated( flag );
}

/** Report invalid request */
void VideoImager::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImager::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

}
