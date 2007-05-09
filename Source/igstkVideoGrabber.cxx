/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoGrabber.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVideoGrabber.h"

namespace igstk
{

VideoGrabber::VideoGrabber() : m_StateMachine( this )
{

  /** Initialize  Machine */
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToSetVideoBufferSize );
  igstkAddStateMacro( AttemptingToSetWantedFramerate );
  igstkAddStateMacro( AttemptingToSetVideoOutputFormat );
  igstkAddStateMacro( AttemptingToSetVideoOutputDimensions );
  igstkAddStateMacro( AttemptingToSetVideoOutputClipRectangle );
  igstkAddStateMacro( AttemptingToSetVideoOutputPadding );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( AttemptingToStartGrabbing );
  igstkAddStateMacro( AttemptingToStopGrabbing );
  
  igstkAddStateMacro( GrabberReady );
  igstkAddStateMacro( AttemptingToActivateGrabber );
  igstkAddStateMacro( AttemptingToDeactivateGrabber );
  igstkAddStateMacro( GrabberActive );
  igstkAddStateMacro( AttemptingToGrabOneFrame );
  igstkAddStateMacro( Grabbing );
  
  
  igstkAddInputMacro( SetVideoBufferSize );
  igstkAddInputMacro( SetWantedFramerate );
  igstkAddInputMacro( SetVideoOutputFormat );
  igstkAddInputMacro( SetVideoOutputDimensions );
  igstkAddInputMacro( SetVideoOutputClipRectangle );
  igstkAddInputMacro( SetVideoOutputPadding );
  igstkAddInputMacro( Open );
  igstkAddInputMacro( Close );
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( StartGrabbing );
  igstkAddInputMacro( StopGrabbing );
  igstkAddInputMacro( GrabOneFrame );
  
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );


  /** RequestSet transitions */ 
  igstkAddTransitionMacro( Idle,                            // From_State
                           SetVideoBufferSize,              // Received_Input
                           AttemptingToSetVideoBufferSize,  // To_State
                           SetVideoBufferSize );            // Action
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Failure,
                           Idle,
                           No );
                           
  igstkAddTransitionMacro( Idle,
                           SetWantedFramerate,
                           AttemptingToSetWantedFramerate,
                           SetWantedFramerate );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Failure,
                           Idle,
                           No );
                           
  igstkAddTransitionMacro( Idle,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputFormat,
                           SetVideoOutputFormat );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Failure,
                           Idle,
                           No );
                           
  igstkAddTransitionMacro( Idle,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputDimensions );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Failure,
                           Idle,
                           No );
                           
  igstkAddTransitionMacro( Idle,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputClipRectangle );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Failure,
                           Idle,
                           No );
                           
  igstkAddTransitionMacro( Idle,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputPadding,
                           SetVideoOutputPadding );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Failure,
                           Idle,
                           No );
                           
                           
  /** Initialize */
  igstkAddTransitionMacro( Idle,
                           Initialize,
                           AttemptingToInitialize,
                           Initialize );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Success,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Failure,
                           Idle,
                           No );
                           
  /** Activate grabber */ 
  igstkAddTransitionMacro( GrabberReady,
                           Open,
                           AttemptingToActivateGrabber,
                           ActivateGrabber );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Success,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Failure,
                           GrabberReady,
                           No );
                           
  /** Deactivate grabber */ 
  igstkAddTransitionMacro( GrabberActive,
                           Close,
                           AttemptingToDeactivateGrabber,
                           DeactivateGrabber );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Success,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Failure,
                           GrabberActive,
                           No );
                           
  /** Grab one frame */ 
  igstkAddTransitionMacro( GrabberActive,
                           GrabOneFrame,
                           AttemptingToGrabOneFrame,
                           GrabOneFrame );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Success,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Failure,
                           GrabberActive,
                           No );
                           
  /** Start continious grabbing */ 
  igstkAddTransitionMacro( GrabberActive,
                           StartGrabbing,
                           AttemptingToStartGrabbing,
                           StartGrabbing );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Success,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Failure,
                           GrabberActive,
                           No );
                           
  /** Stop continious grabbing */ 
  igstkAddTransitionMacro( Grabbing,
                           StopGrabbing,
                           AttemptingToStopGrabbing,
                           StopGrabbing );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Success,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Failure,
                           Grabbing,
                           No );
                           
  igstkAddTransitionMacro( Grabbing,
                           Close,
                           AttemptingToDeactivateGrabber,
                           DeactivateGrabber );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Success,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Failure,
                           GrabberActive,
                           No );
               
  /** State machine ready to run */
  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();   
  
  /** Export the state machine description */
  std::ofstream ofile;
  ofile.open("VideoGrabberStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();

}

/** Destructor */
VideoGrabber::~VideoGrabber()
{
}

/** Method to be invoked when no operation is required */
void VideoGrabber::NoProcessing()
{
}

/** Internal fuction to set and test video buffer size. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoBufferSizeProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetVideoBufferSizeProcessing called...\n");
  m_VideoBufferSize = m_VideoBufferSizeToBeSet;
  return SUCCESS;
}

/** Internal fuction to set and test video framerate. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetWantedFramerateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetWantedFramerateProcessing called...\n");
  m_Framerate = m_FramerateToBeSet;
  return SUCCESS;
}

/** Internal fuction to set and test video output format. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputFormatProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetVideoOutputFormatProcessing called...\n");
  m_VideoOutputFormat = m_VideoOutputFormatToBeSet;
  return SUCCESS;
}

/** Internal fuction to set and test video output dimensions. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputDimensionsProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetVideoOutputDimensionsProcessing called...\n");
  m_VideoOutputHeight = m_VideoOutputHeightToBeSet;
  m_VideoOutputWidth = m_VideoOutputWidthToBeSet;
  return SUCCESS;
}

/** Internal fuction to set and test video output clipping rectangle. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputClipRectangleProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetVideoOutputClipRectangleProcessing called...\n");
  m_VideoOutputClipRectangleHeight = m_VideoOutputClipRectangleHeightToBeSet;
  m_VideoOutputClipRectangleWidth = m_VideoOutputClipRectangleWidthToBeSet;
  m_VideoOutputClipRectangleXPosition = 
    m_VideoOutputClipRectangleXPositionToBeSet;
  m_VideoOutputClipRectangleYPosition = 
    m_VideoOutputClipRectangleYPositionToBeSet;
  return SUCCESS;
}

/** Internal fuction to set and test framerate. 
* This method must be overridden by a decendant class 
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputPaddingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalSetVideoOutputPaddingProcessing called...\n");
  m_VideoOutputPadding = m_VideoOutputPaddingToBeSet;
  return SUCCESS;
}

/** The following methods should be abstract to make sure they are reimplemeted
    in the decendant classes */

/** Internal fuction to initialize grabber. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalInitializeProcessing( void ) 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalInitializeProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to activate grabber. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalActivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalActivateGrabberProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to deactivate grabber. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalDeactivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalDeactivateGrabberProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to grab one framer. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalGrabOneFrameProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalGrabOneFrameProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to start grabbing. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalStartGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalStartGrabbingProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to stop grabbing. 
* This method must be overridden by a decendant class 
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalStopGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
::InternalStopGrabbingProcessing called...\n");
  return SUCCESS;
}*/

void VideoGrabber::SetVideoBufferSizeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetVideoBufferSizeProcessing called...\n");
  ResultType result = this->InternalSetVideoBufferSizeProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetWantedFramerateProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetWantedFramerateProcessing called...\n");
  ResultType result = this->InternalSetWantedFramerateProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputFormatProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetVideoOutputFormatProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputFormatProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputDimensionsProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetVideoOutputDimensionsProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputDimensionsProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputClipRectangleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetVideoOutputClipRectangleProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputClipRectangleProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputPaddingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::SetVideoOutputPaddingProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputPaddingProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::InitializeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::InitializeProcessing called...\n");
  ResultType result = this->InternalInitializeProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::ActivateGrabberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ActivateGrabberProcessing called...\n");
  ResultType result = this->InternalActivateGrabberProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::DeactivateGrabberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::DeactivateGrabberProcessing called...\n");
  ResultType result = this->InternalDeactivateGrabberProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::GrabOneFrameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::GrabOneFrameProcessing called...\n");
  ResultType result = this->InternalGrabOneFrameProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::StartGrabbingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::StartGrabbingProcessing called...\n");
  ResultType result = this->InternalStartGrabbingProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::StopGrabbingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::StopGrabbingProcessing called...\n");
  ResultType result = this->InternalStopGrabbingProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Public request methods */
void VideoGrabber::RequestSetVideoBufferSize( unsigned long videoBufferSize )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoBufferSize called...\n");
  m_VideoBufferSizeToBeSet = videoBufferSize;
  igstkPushInputMacro( SetVideoBufferSize );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetWantedFramerate( unsigned int framerate )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetWantedFramerate called...\n");
  m_FramerateToBeSet = framerate;
  igstkPushInputMacro( SetWantedFramerate );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputFormat( unsigned int format )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputFormat called...\n");
  m_VideoOutputFormatToBeSet = format;
  igstkPushInputMacro( SetVideoOutputFormat );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputDimensions( unsigned int videoHeight, 
                                                    unsigned int videoWidth )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputDimensions called...\n");
  m_VideoOutputHeightToBeSet = videoHeight;
  m_VideoOutputWidthToBeSet = videoWidth;
  igstkPushInputMacro( SetVideoOutputDimensions );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputClipRectangle( unsigned int height, 
                                                       unsigned int width,
                                                       signed int xPosition, 
                                                       signed int yPosition )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputClipRectangle called...\n");
  m_VideoOutputClipRectangleHeightToBeSet = height;
  m_VideoOutputClipRectangleWidthToBeSet = width;
  m_VideoOutputClipRectangleXPositionToBeSet = xPosition;
  m_VideoOutputClipRectangleYPositionToBeSet = yPosition;
  igstkPushInputMacro( SetVideoOutputClipRectangle );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputPadding( unsigned int padding )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputPadding called...\n");
  m_VideoOutputPaddingToBeSet = padding;
  igstkPushInputMacro( SetVideoOutputPadding );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestOpen()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestOpen called...\n");
  igstkPushInputMacro( Open );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestClose()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestClose called...\n");
  igstkPushInputMacro( Close );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestInitialize()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestInitialize called...\n");
  igstkPushInputMacro( Initialize );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStartGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestStartGrabbing called...\n");
  igstkPushInputMacro( StartGrabbing );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStopGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestStopGrabbing called...\n");
  igstkPushInputMacro( StopGrabbing );
  this->m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGrabOneFrame()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGrabOneFrame called...\n");
  igstkPushInputMacro( GrabOneFrame );
  this->m_StateMachine.ProcessInputs();
}

/** Public get methods */
void VideoGrabber::GetVideoBufferSize()
{}
void VideoGrabber::GetVideoBufferPtr()
{}
void VideoGrabber::GetVideoFrameNo()
{}
void VideoGrabber::GetNumberOfVideoFrames()
{}
void VideoGrabber::GetTimeTag()
{}
void VideoGrabber::GetFramerate()
{}
void VideoGrabber::GetVideoOutputFormat()
{}
void VideoGrabber::GetVideoOutputDimensions()
{}
void VideoGrabber::GetVideoOutputClipRectangle()
{}
void VideoGrabber::GetVideoOutputPadding()
{}

} // end namespace igstk
