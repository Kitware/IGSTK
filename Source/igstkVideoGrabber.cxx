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
  /** Setup logger, for all other igstk components. */
  m_Logger   = LoggerType::New();
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->SetLogger(m_Logger);
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  igstkLogMacro( DEBUG, "igstk::VideoGrabber constructor called...\n");

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
  
  igstkAddInputMacro( RequestVideoBufferSize );
  igstkAddInputMacro( RequestVideoFrameNo );
  igstkAddInputMacro( RequestFramerate );
  igstkAddInputMacro( RequestVideoOutputFormat );
  igstkAddInputMacro( RequestVideoOutputDimensions );
  igstkAddInputMacro( RequestVideoOutputClipRectangle );
  igstkAddInputMacro( RequestVideoOutputPadding );
  igstkAddInputMacro( RequestNumberOfVideoFrames );
  igstkAddInputMacro( RequestTimeTag );
  
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
  

  /** RequestGet transitions */
  igstkAddTransitionMacro( Idle,
                           RequestVideoBufferSize,
                           Idle,
                           ReportVideoBufferSize );
  igstkAddTransitionMacro( Idle,
                           RequestFramerate,
                           Idle,
                           ReportFramerate );
  igstkAddTransitionMacro( Idle,
                           RequestVideoOutputFormat,
                           Idle,
                           ReportVideoOutputFormat );
  igstkAddTransitionMacro( Idle,
                           RequestVideoOutputDimensions,
                           Idle,
                           ReportVideoOutputDimensions );
  igstkAddTransitionMacro( Idle,
                           RequestVideoOutputClipRectangle,
                           Idle,
                           ReportVideoOutputClipRectangle );
  igstkAddTransitionMacro( Idle,
                           RequestVideoOutputPadding,
                           Idle,
                           ReportVideoOutputPadding );
  
  igstkAddTransitionMacro( Grabbing,
                           RequestNumberOfVideoFrames,
                           Grabbing,
                           ReportVideoOutputPadding );
  igstkAddTransitionMacro( Grabbing,
                           RequestTimeTag,
                           Grabbing,
                           ReportVideoOutputPadding );
  igstkAddTransitionMacro( GrabberActive,
                           RequestTimeTag,
                           GrabberActive,
                           ReportVideoOutputPadding );
                           
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

  /*********************/                           
  /** null transitions */
  /*********************/
  
  // Idle state
  igstkAddTransitionMacro( Idle,
                           Open,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           Close,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           StartGrabbing,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           StopGrabbing,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           GrabOneFrame,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           Success,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           Failure,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           RequestNumberOfVideoFrames,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           RequestTimeTag,
                           Idle,
                           No );
  igstkAddTransitionMacro( Idle,
                           RequestVideoFrameNo,
                           Idle,
                           No );
                           
 // AttemptingToSetVideoBufferSize
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoBufferSize,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetWantedFramerate,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoBufferSize,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestFramerate,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestTimeTag,
                           AttemptingToSetVideoBufferSize,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Open,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Close,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Initialize,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           StartGrabbing,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           StopGrabbing,
                           AttemptingToSetVideoBufferSize,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           GrabOneFrame,
                           AttemptingToSetVideoBufferSize,
                           No );

 // AttemptingToSetWantedFramerate
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoBufferSize,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetWantedFramerate,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputFormat,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputDimensions,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputPadding,
                           AttemptingToSetWantedFramerate,
                           No );

  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoBufferSize,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoFrameNo,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestFramerate,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputFormat,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputDimensions,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputPadding,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestTimeTag,
                           AttemptingToSetWantedFramerate,
                           No );

  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Open,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Close,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Initialize,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           StartGrabbing,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           StopGrabbing,
                           AttemptingToSetWantedFramerate,
                           No );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           GrabOneFrame,
                           AttemptingToSetWantedFramerate,
                           No );

 // AttemptingToSetVideoOutputFormat
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputFormat,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestFramerate,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputFormat,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Open,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Close,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Initialize,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           StartGrabbing,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           StopGrabbing,
                           AttemptingToSetVideoOutputFormat,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputFormat,
                           No );
 
 
  // AttemptingToSetVideoOutputDimensions
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputDimensions,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestFramerate,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputDimensions,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Open,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Close,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Initialize,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           StartGrabbing,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           StopGrabbing,
                           AttemptingToSetVideoOutputDimensions,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputDimensions,
                           No );

  // AttemptingToSetVideoOutputClipRectangle
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestFramerate,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Open,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Close,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Initialize,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           StartGrabbing,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           StopGrabbing,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputClipRectangle,
                           No );

  // AttemptingToSetVideoOutputPadding
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputPadding,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestFramerate,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputPadding,
                           No );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Open,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Close,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Initialize,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           StartGrabbing,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           StopGrabbing,
                           AttemptingToSetVideoOutputPadding,
                           No );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputPadding,
                           No );

  // AttemptingToInitialize
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoBufferSize,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetWantedFramerate,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputFormat,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputDimensions,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputClipRectangle,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputPadding,
                           AttemptingToInitialize,
                           No );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoBufferSize,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoFrameNo,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestFramerate,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputFormat,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputDimensions,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputClipRectangle,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputPadding,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestNumberOfVideoFrames,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestTimeTag,
                           AttemptingToInitialize,
                           No );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           Open,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Close,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Initialize,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           StartGrabbing,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           StopGrabbing,
                           AttemptingToInitialize,
                           No );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GrabOneFrame,
                           AttemptingToInitialize,
                           No );

  // AttemptingToStartGrabbing
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoBufferSize,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetWantedFramerate,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputFormat,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputDimensions,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputClipRectangle,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputPadding,
                           AttemptingToStartGrabbing,
                           No );

  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoBufferSize,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoFrameNo,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestFramerate,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputFormat,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputDimensions,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputClipRectangle,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputPadding,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestNumberOfVideoFrames,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestTimeTag,
                           AttemptingToStartGrabbing,
                           No );

  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Open,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Close,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Initialize,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           StartGrabbing,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           StopGrabbing,
                           AttemptingToStartGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           GrabOneFrame,
                           AttemptingToStartGrabbing,
                           No );

  // AttemptingToStopGrabbing
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoBufferSize,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetWantedFramerate,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputFormat,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputDimensions,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputClipRectangle,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputPadding,
                           AttemptingToStopGrabbing,
                           No );

  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoBufferSize,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoFrameNo,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestFramerate,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputFormat,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputDimensions,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputClipRectangle,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputPadding,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestNumberOfVideoFrames,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestTimeTag,
                           AttemptingToStopGrabbing,
                           No );

  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Open,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Close,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Initialize,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           StartGrabbing,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           StopGrabbing,
                           AttemptingToStopGrabbing,
                           No );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           GrabOneFrame,
                           AttemptingToStopGrabbing,
                           No );


  // GrabberReady
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoBufferSize,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           SetWantedFramerate,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputFormat,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputDimensions,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputClipRectangle,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputPadding,
                           GrabberReady,
                           No );

  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoBufferSize,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoFrameNo,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestFramerate,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputFormat,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputDimensions,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputClipRectangle,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputPadding,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestNumberOfVideoFrames,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           RequestTimeTag,
                           GrabberReady,
                           No );

  igstkAddTransitionMacro( GrabberReady,
                           Close,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           Initialize,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           StartGrabbing,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           StopGrabbing,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           GrabOneFrame,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           Success,
                           GrabberReady,
                           No );
  igstkAddTransitionMacro( GrabberReady,
                           Failure,
                           GrabberReady,
                           No );

  // AttemptingToActivateGrabber
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoBufferSize,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetWantedFramerate,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputFormat,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputDimensions,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputClipRectangle,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputPadding,
                           AttemptingToActivateGrabber,
                           No );

  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoBufferSize,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoFrameNo,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestFramerate,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputFormat,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputDimensions,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputClipRectangle,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputPadding,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestNumberOfVideoFrames,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestTimeTag,
                           AttemptingToActivateGrabber,
                           No );

  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Open,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Close,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Initialize,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           StartGrabbing,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           StopGrabbing,
                           AttemptingToActivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           GrabOneFrame,
                           AttemptingToActivateGrabber,
                           No );

  // AttemptingToDeactivateGrabber
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoBufferSize,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetWantedFramerate,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputFormat,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputDimensions,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputClipRectangle,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputPadding,
                           AttemptingToDeactivateGrabber,
                           No );

  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoBufferSize,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoFrameNo,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestFramerate,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputFormat,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputDimensions,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputClipRectangle,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputPadding,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestNumberOfVideoFrames,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestTimeTag,
                           AttemptingToDeactivateGrabber,
                           No );

  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Open,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Close,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Initialize,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           StartGrabbing,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           StopGrabbing,
                           AttemptingToDeactivateGrabber,
                           No );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           GrabOneFrame,
                           AttemptingToDeactivateGrabber,
                           No );

  // GrabberActive
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoBufferSize,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           SetWantedFramerate,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputFormat,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputDimensions,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputClipRectangle,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputPadding,
                           GrabberActive,
                           No );

  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoBufferSize,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoFrameNo,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestFramerate,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputFormat,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputDimensions,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputClipRectangle,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputPadding,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           RequestNumberOfVideoFrames,
                           GrabberActive,
                           No );

  igstkAddTransitionMacro( GrabberActive,
                           Open,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           Initialize,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           StopGrabbing,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           Success,
                           GrabberActive,
                           No );
  igstkAddTransitionMacro( GrabberActive,
                           Failure,
                           GrabberActive,
                           No );


  // AttemptingToGrabOneFrame
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoBufferSize,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetWantedFramerate,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputFormat,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputDimensions,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputClipRectangle,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputPadding,
                           AttemptingToGrabOneFrame,
                           No );

  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoBufferSize,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoFrameNo,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestFramerate,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputFormat,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputDimensions,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputClipRectangle,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputPadding,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestNumberOfVideoFrames,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestTimeTag,
                           AttemptingToGrabOneFrame,
                           No );

  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Open,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Close,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Initialize,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           StartGrabbing,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           StopGrabbing,
                           AttemptingToGrabOneFrame,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           GrabOneFrame,
                           AttemptingToGrabOneFrame,
                           No );

  // Grabbing
  igstkAddTransitionMacro( Grabbing,
                           SetVideoBufferSize,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           SetWantedFramerate,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputFormat,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputDimensions,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputClipRectangle,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputPadding,
                           Grabbing,
                           No );

  igstkAddTransitionMacro( Grabbing,
                           RequestVideoBufferSize,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoFrameNo,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestFramerate,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputFormat,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputDimensions,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputClipRectangle,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputPadding,
                           Grabbing,
                           No );

  igstkAddTransitionMacro( Grabbing,
                           Open,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           Initialize,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           StartGrabbing,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           GrabOneFrame,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           Success,
                           Grabbing,
                           No );
  igstkAddTransitionMacro( Grabbing,
                           Failure,
                           Grabbing,
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
  igstkLogMacro( DEBUG, "igstk::VideoGrabber destructor called...\n");
}

/** Method to be invoked when no operation is required */
void VideoGrabber::NoProcessing()
{
  igstkLogMacro( DEBUG, "VideoGrabber::NoProcessing \
                           called ...\n");
}

/** State machine Input error processing. */
void VideoGrabber::ErrorProcessing()
{
  igstkLogMacro( CRITICAL, "VideoGrabber::ErrorProcessing \
                           called ...\n");
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
  m_VideoOutputDimensions = m_VideoOutputDimensionsToBeSet;
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
  m_VideoOutputClipRectangle = m_VideoOutputClipRectangleToBeSet;
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

void VideoGrabber::ReportVideoBufferSizeProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportVideoBufferSizeProcessing called...\n");
  UnsignedLongEvent event;
  event.Set( m_VideoBufferSize );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoFrameNoProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportVideoFrameNoProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportNumberOfVideoFramesProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportNumberOfVideoFramesProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportTimeTagProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportTimeTagProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportFramerateProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportFramerateProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_Framerate );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputFormatProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportVideoOutputFormatProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_VideoOutputFormat );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputDimensionsProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportVideoOutputDimensionsProcessing called...\n");
  VideoDimensionsEvent event;
  event.Set( m_VideoOutputDimensions );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputClipRectangleProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                 ::ReportVideoOutputClipRectangleProcessing called...\n");
  VideoClipRectangleEvent event;
  event.Set( m_VideoOutputClipRectangle );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputPaddingProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::ReportVideoOutputPaddingProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_VideoOutputPadding );
  this->InvokeEvent( event );
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
void VideoGrabber::RequestSetVideoBufferSize( unsigned long bufferSize )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoBufferSize called...\n");
  m_VideoBufferSizeToBeSet = bufferSize;
  igstkPushInputMacro( SetVideoBufferSize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetWantedFramerate( unsigned int framerate )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetWantedFramerate called...\n");
  m_FramerateToBeSet = framerate;
  igstkPushInputMacro( SetWantedFramerate );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputFormat( unsigned int format )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputFormat called...\n");
  m_VideoOutputFormatToBeSet = format;
  igstkPushInputMacro( SetVideoOutputFormat );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputDimensions( unsigned int videoHeight, 
                                                    unsigned int videoWidth )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputDimensions called...\n");
  m_VideoOutputDimensionsToBeSet.m_Height = videoHeight;
  m_VideoOutputDimensionsToBeSet.m_Width = videoWidth;
  igstkPushInputMacro( SetVideoOutputDimensions );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputClipRectangle( unsigned int height, 
                                                       unsigned int width,
                                                       signed int xPosition, 
                                                       signed int yPosition )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputClipRectangle called...\n");
  m_VideoOutputClipRectangleToBeSet.m_Height = height;
  m_VideoOutputClipRectangleToBeSet.m_Width = width;
  m_VideoOutputClipRectangleToBeSet.m_XPosition = xPosition;
  m_VideoOutputClipRectangleToBeSet.m_YPosition = yPosition;
  igstkPushInputMacro( SetVideoOutputClipRectangle );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputPadding( unsigned int padding )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestSetVideoOutputPadding called...\n");
  m_VideoOutputPaddingToBeSet = padding;
  igstkPushInputMacro( SetVideoOutputPadding );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestOpen()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestOpen called...\n");
  igstkPushInputMacro( Open );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestClose()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestClose called...\n");
  igstkPushInputMacro( Close );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestInitialize()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestInitialize called...\n");
  igstkPushInputMacro( Initialize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStartGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestStartGrabbing called...\n");
  igstkPushInputMacro( StartGrabbing );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStopGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestStopGrabbing called...\n");
  igstkPushInputMacro( StopGrabbing );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGrabOneFrame()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGrabOneFrame called...\n");
  igstkPushInputMacro( GrabOneFrame );
  m_StateMachine.ProcessInputs();
}

/** Public get methods */
void VideoGrabber::RequestGetVideoBufferSize()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoBufferSize called...\n");
  igstkPushInputMacro( RequestVideoBufferSize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoFrameNo()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoFrameNo called...\n");
  igstkPushInputMacro( RequestVideoFrameNo );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetNumberOfVideoFrames()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetNumberOfVideoFrames called...\n");
  igstkPushInputMacro( RequestNumberOfVideoFrames );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetTimeTag()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetTimeTag called...\n");
  igstkPushInputMacro( RequestTimeTag );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetFramerate()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetFramerate called...\n");
  igstkPushInputMacro( RequestFramerate );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputFormat()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoOutputFormat called...\n");
  igstkPushInputMacro( RequestVideoOutputFormat );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputDimensions()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoOutputDimensions called...\n");
  igstkPushInputMacro( RequestVideoOutputDimensions );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputClipRectangle()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoOutputClipRectangle called...\n");
  igstkPushInputMacro( RequestVideoOutputClipRectangle );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputPadding()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                        ::RequestGetVideoOutputPadding called...\n");
  igstkPushInputMacro( RequestVideoOutputPadding );
  m_StateMachine.ProcessInputs();
}


} // end namespace igstk
