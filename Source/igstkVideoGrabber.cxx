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
  igstkAddStateMacro( AttemptingToSetVideoOutputClipRectangle );
  igstkAddStateMacro( AttemptingToSetVideoOutputPadding );
  igstkAddStateMacro( AttemptingToSetGrabbingMode );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( AttemptingToStartGrabbing );
  igstkAddStateMacro( AttemptingToStopGrabbing );
  
  igstkAddStateMacro( GrabberReady );
  igstkAddStateMacro( AttemptingToActivateGrabber );
  igstkAddStateMacro( AttemptingToDeactivateGrabber );
  igstkAddStateMacro( GrabberActive );
  igstkAddStateMacro( AttemptingToGrabOneFrame );
  igstkAddStateMacro( Grabbing );
  
  
  igstkAddInputMacro( RequestSetVideoBufferSize );
  igstkAddInputMacro( RequestSetWantedFramerate );
  igstkAddInputMacro( RequestSetVideoOutputFormat );
  igstkAddInputMacro( RequestSetVideoOutputClipRectangle );
  igstkAddInputMacro( RequestSetVideoOutputPadding );
  igstkAddInputMacro( RequestSetGrabbingMode );
  igstkAddInputMacro( RequestOpen );
  igstkAddInputMacro( RequestClose );
  igstkAddInputMacro( RequestInitialize );
  igstkAddInputMacro( RequestStartGrabbing );
  igstkAddInputMacro( RequestStopGrabbing );
  igstkAddInputMacro( RequestGrabOneFrame );
  
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );


  /** RequestSet transitions */ 
  igstkAddTransitionMacro( Idle,                            // From_State
                           RequestSetVideoBufferSize,       // Received_Input
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
                           RequestSetWantedFramerate,
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
                           RequestSetVideoOutputFormat,
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
                           RequestSetVideoOutputClipRectangle,
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
                           RequestSetVideoOutputPadding,
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
                           
  igstkAddTransitionMacro( Idle,
                           RequestSetGrabbingMode,
                           AttemptingToSetGrabbingMode,
                           SetGrabbingMode );
  igstkAddTransitionMacro( AttemptingToSetGrabbingMode,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetGrabbingMode,
                           Failure,
                           Idle,
                           No );
                           
  /** Initialize */
  igstkAddTransitionMacro( Idle,
                           RequestInitialize,
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
                           RequestOpen,
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
                           RequestClose,
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
                           RequestGrabOneFrame,
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
                           RequestStartGrabbing,
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
                           RequestStopGrabbing,
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
                           RequestClose,
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

void VideoGrabber::SetVideoBufferSizeProcessing()
{
}

void VideoGrabber::SetWantedFramerateProcessing()
{
}

void VideoGrabber::SetVideoOutputFormatProcessing()
{
}

void VideoGrabber::SetVideoOutputClipRectangleProcessing()
{
}

void VideoGrabber::SetVideoOutputPaddingProcessing()
{
}

void VideoGrabber::SetGrabbingModeProcessing()
{
}

void VideoGrabber::InitializeProcessing()
{
}

void VideoGrabber::ActivateGrabberProcessing()
{
}

void VideoGrabber::DeactivateGrabberProcessing()
{
}

void VideoGrabber::GrabOneFrameProcessing()
{
}

void VideoGrabber::StartGrabbingProcessing()
{
}

void VideoGrabber::StopGrabbingProcessing()
{
}

/** Public request methods */
void VideoGrabber::RequestSetVideoBufferSize()
{}
void VideoGrabber::RequestSetWantedFramerate()
{}
void VideoGrabber::RequestSetVideoOutputFormat()
{}
void VideoGrabber::RequestSetVideoOutputClipRectangle()
{}
void VideoGrabber::RequestSetVideoOutputPadding()
{}
void VideoGrabber::RequestSetGrabbingMode()
{}
void VideoGrabber::RequestOpen()
{}
void VideoGrabber::RequestClose()
{}
void VideoGrabber::RequestInitialize()
{}
void VideoGrabber::RequestStartGrabbing()
{}
void VideoGrabber::RequestStopGrabbing()
{}
void VideoGrabber::RequestGrabOneFrame()
{}

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
void VideoGrabber::GetVideoOutputClipRectangle()
{}
void VideoGrabber::GetVideoOutputPadding()
{}
void VideoGrabber::GetGrabbingMode()
{}

} // end namespace igstk
