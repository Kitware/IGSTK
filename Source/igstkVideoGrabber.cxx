/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoGrabber.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Made by SINTEF Health Research - Medical technology:
  http://www.sintef.no/medtech

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVideoGrabber.h"

namespace igstk
{

VideoGrabber::VideoGrabber() :
  m_StateMachine( this ),
  m_VideoBufferSizeToBeSet(0),
  m_FramerateToBeSet(0),
  m_VideoOutputFormatToBeSet(0),
  m_VideoOutputPaddingToBeSet(0),
  m_VideoBufferSize(0),
  m_Framerate(0),
  m_VideoOutputFormat(0),
  m_VideoOutputPadding(0)
{
    m_VideoOutputDimensionsToBeSet.m_Height = 0;
    m_VideoOutputDimensionsToBeSet.m_Width = 0;
    m_VideoOutputClipRectangleToBeSet.m_Height = 0;
    m_VideoOutputClipRectangleToBeSet.m_Width = 0;
    m_VideoOutputClipRectangleToBeSet.m_XPosition = 0;
    m_VideoOutputClipRectangleToBeSet.m_YPosition = 0;
    m_VideoOutputDimensions.m_Height = 0;
    m_VideoOutputDimensions.m_Width = 0;
    m_VideoOutputClipRectangle.m_Height = 0;
    m_VideoOutputClipRectangle.m_Width = 0;
    m_VideoOutputClipRectangle.m_XPosition = 0;
    m_VideoOutputClipRectangle.m_YPosition = 0;

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
  igstkAddInputMacro( Wait );


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
  igstkAddTransitionMacro( Grabbing, //GrabberActive,
                           GrabOneFrame,
                           AttemptingToGrabOneFrame,
                           GrabOneFrame );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Success,
                           Grabbing, //GrabberActive,
                           No );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Failure,
                           Grabbing, //GrabberActive,
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
                           Wait,
                           AttemptingToStartGrabbing,
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
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Close,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           StartGrabbing,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           StopGrabbing,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           GrabOneFrame,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Success,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Failure,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           RequestNumberOfVideoFrames,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           RequestTimeTag,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           RequestVideoFrameNo,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Wait,
                           Idle,
                           ReportInvalidRequest );

 // AttemptingToSetVideoBufferSize
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoBufferSize,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetWantedFramerate,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestFramerate,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           RequestTimeTag,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Open,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Close,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Initialize,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           StartGrabbing,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           StopGrabbing,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           GrabOneFrame,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoBufferSize,
                           Wait,
                           AttemptingToSetVideoBufferSize,
                           ReportInvalidRequest );


 // AttemptingToSetWantedFramerate
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoBufferSize,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetWantedFramerate,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputFormat,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputDimensions,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           SetVideoOutputPadding,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoBufferSize,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoFrameNo,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestFramerate,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputFormat,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputDimensions,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestVideoOutputPadding,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           RequestTimeTag,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Open,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Close,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Initialize,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           StartGrabbing,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           StopGrabbing,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           GrabOneFrame,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetWantedFramerate,
                           Wait,
                           AttemptingToSetWantedFramerate,
                           ReportInvalidRequest );

 // AttemptingToSetVideoOutputFormat
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestFramerate,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Open,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Close,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Initialize,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           StartGrabbing,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           StopGrabbing,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputFormat,
                           Wait,
                           AttemptingToSetVideoOutputFormat,
                           ReportInvalidRequest );


  // AttemptingToSetVideoOutputDimensions
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestFramerate,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Open,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Close,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Initialize,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           StartGrabbing,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           StopGrabbing,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputDimensions,
                           Wait,
                           AttemptingToSetVideoOutputDimensions,
                           ReportInvalidRequest );

  // AttemptingToSetVideoOutputClipRectangle
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestFramerate,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Open,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Close,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Initialize,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           StartGrabbing,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           StopGrabbing,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputClipRectangle,
                           Wait,
                           AttemptingToSetVideoOutputClipRectangle,
                           ReportInvalidRequest );

  // AttemptingToSetVideoOutputPadding
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoBufferSize,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetWantedFramerate,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputFormat,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputDimensions,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           SetVideoOutputPadding,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoBufferSize,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoFrameNo,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestFramerate,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputFormat,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputDimensions,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputClipRectangle,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestVideoOutputPadding,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestNumberOfVideoFrames,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           RequestTimeTag,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Open,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Close,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Initialize,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           StartGrabbing,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           StopGrabbing,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           GrabOneFrame,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToSetVideoOutputPadding,
                           Wait,
                           AttemptingToSetVideoOutputPadding,
                           ReportInvalidRequest );

  // AttemptingToInitialize
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoBufferSize,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetWantedFramerate,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputFormat,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputDimensions,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputClipRectangle,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetVideoOutputPadding,
                           AttemptingToInitialize,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoBufferSize,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoFrameNo,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestFramerate,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputFormat,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputDimensions,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputClipRectangle,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestVideoOutputPadding,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestNumberOfVideoFrames,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           RequestTimeTag,
                           AttemptingToInitialize,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           Open,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Close,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Initialize,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           StartGrabbing,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           StopGrabbing,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GrabOneFrame,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Wait,
                           AttemptingToInitialize,
                           ReportInvalidRequest );

  // AttemptingToStartGrabbing
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoBufferSize,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetWantedFramerate,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputFormat,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputDimensions,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputClipRectangle,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           SetVideoOutputPadding,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoBufferSize,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoFrameNo,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestFramerate,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputFormat,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputDimensions,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputClipRectangle,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestVideoOutputPadding,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestNumberOfVideoFrames,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           RequestTimeTag,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Open,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Close,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           Initialize,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           StartGrabbing,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           StopGrabbing,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStartGrabbing,
                           GrabOneFrame,
                           AttemptingToStartGrabbing,
                           ReportInvalidRequest );

  // AttemptingToStopGrabbing
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoBufferSize,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetWantedFramerate,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputFormat,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputDimensions,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputClipRectangle,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           SetVideoOutputPadding,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoBufferSize,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoFrameNo,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestFramerate,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputFormat,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputDimensions,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputClipRectangle,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestVideoOutputPadding,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestNumberOfVideoFrames,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           RequestTimeTag,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Open,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Close,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Initialize,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           StartGrabbing,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           StopGrabbing,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           GrabOneFrame,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToStopGrabbing,
                           Wait,
                           AttemptingToStopGrabbing,
                           ReportInvalidRequest );


  // GrabberReady
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoBufferSize,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           SetWantedFramerate,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputFormat,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputDimensions,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputClipRectangle,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           SetVideoOutputPadding,
                           GrabberReady,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoBufferSize,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoFrameNo,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestFramerate,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputFormat,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputDimensions,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputClipRectangle,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestVideoOutputPadding,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestNumberOfVideoFrames,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           RequestTimeTag,
                           GrabberReady,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( GrabberReady,
                           Close,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           Initialize,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           StartGrabbing,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           StopGrabbing,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           GrabOneFrame,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           Success,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           Failure,
                           GrabberReady,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberReady,
                           Wait,
                           GrabberReady,
                           ReportInvalidRequest );

  // AttemptingToActivateGrabber
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoBufferSize,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetWantedFramerate,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputFormat,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputDimensions,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputClipRectangle,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           SetVideoOutputPadding,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoBufferSize,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoFrameNo,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestFramerate,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputFormat,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputDimensions,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputClipRectangle,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestVideoOutputPadding,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestNumberOfVideoFrames,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           RequestTimeTag,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Open,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Close,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Initialize,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           StartGrabbing,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           StopGrabbing,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           GrabOneFrame,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToActivateGrabber,
                           Wait,
                           AttemptingToActivateGrabber,
                           ReportInvalidRequest );

  // AttemptingToDeactivateGrabber
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoBufferSize,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetWantedFramerate,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputFormat,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputDimensions,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputClipRectangle,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           SetVideoOutputPadding,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoBufferSize,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoFrameNo,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestFramerate,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputFormat,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputDimensions,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputClipRectangle,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestVideoOutputPadding,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestNumberOfVideoFrames,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           RequestTimeTag,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Open,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Close,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Initialize,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           StartGrabbing,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           StopGrabbing,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           GrabOneFrame,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToDeactivateGrabber,
                           Wait,
                           AttemptingToDeactivateGrabber,
                           ReportInvalidRequest );

  // GrabberActive
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoBufferSize,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           SetWantedFramerate,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputFormat,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputDimensions,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputClipRectangle,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           SetVideoOutputPadding,
                           GrabberActive,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoBufferSize,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoFrameNo,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestFramerate,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputFormat,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputDimensions,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputClipRectangle,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestVideoOutputPadding,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           RequestNumberOfVideoFrames,
                           GrabberActive,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( GrabberActive,
                           Open,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           Initialize,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           StopGrabbing,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           Success,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           Failure,
                           GrabberActive,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive,
                           Wait,
                           GrabberActive,
                           ReportInvalidRequest );


  // AttemptingToGrabOneFrame
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoBufferSize,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetWantedFramerate,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputFormat,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputDimensions,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputClipRectangle,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           SetVideoOutputPadding,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoBufferSize,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoFrameNo,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestFramerate,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputFormat,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputDimensions,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputClipRectangle,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestVideoOutputPadding,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestNumberOfVideoFrames,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           RequestTimeTag,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Open,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Close,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Initialize,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           StartGrabbing,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           StopGrabbing,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           GrabOneFrame,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToGrabOneFrame,
                           Wait,
                           AttemptingToGrabOneFrame,
                           ReportInvalidRequest );

  // Grabbing
  igstkAddTransitionMacro( Grabbing,
                           SetVideoBufferSize,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           SetWantedFramerate,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputFormat,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputDimensions,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputClipRectangle,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           SetVideoOutputPadding,
                           Grabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Grabbing,
                           RequestVideoBufferSize,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoFrameNo,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestFramerate,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputFormat,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputDimensions,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputClipRectangle,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           RequestVideoOutputPadding,
                           Grabbing,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Grabbing,
                           Open,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           Initialize,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           StartGrabbing,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( GrabberActive, //Grabbing,
                           GrabOneFrame,
                           GrabberActive, //Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           Success,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           Failure,
                           Grabbing,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Grabbing,
                           Wait,
                           Grabbing,
                           ReportInvalidRequest );


  /** State machine ready to run */
  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();

  /** Export the state machine description */
/*  std::ofstream ofile;
  ofile.open("VideoGrabberStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();*/

}

/** Destructor */
VideoGrabber::~VideoGrabber()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber destructor called...\n");
}

/** Method to be invoked when no operation is required */
void VideoGrabber::NoProcessing()
{
  igstkLogMacro( DEBUG, "VideoGrabber::NoProcessing called ...\n");
}

/** Method to be invoked when request is performed in an illegal state */
void VideoGrabber::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "igstk::VideoGrabber::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

/** State machine Input error processing. */
void VideoGrabber::ErrorProcessing()
{
  igstkLogMacro( CRITICAL, "VideoGrabber::ErrorProcessing called ...\n");
}

/** Internal fuction to set and test video buffer size.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoBufferSizeProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                 ::InternalSetVideoBufferSizeProcessing called...\n");
  m_VideoBufferSize = m_VideoBufferSizeToBeSet;
  return SUCCESS;
}*/

/** Internal fuction to set and test video framerate.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalSetWantedFramerateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber\
                 ::InternalSetWantedFramerateProcessing called...\n");
  m_Framerate = m_FramerateToBeSet;
  return SUCCESS;
}*/

/** Internal fuction to set and test video output format.
* This method must be overridden by a decendant class
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputFormatProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::InternalSetVideoOutputFormatProcessing called...\n");
  if ( m_VideoOutputFormatToBeSet == 8 ||
       m_VideoOutputFormatToBeSet == 16 ||
       m_VideoOutputFormatToBeSet == 32 )
  {
    m_VideoOutputFormat = m_VideoOutputFormatToBeSet;
    return SUCCESS;
  } else
  {
    igstkLogMacro( CRITICAL,
                   "VideoGrabber::InternalSetVideoOutputFormatProcessing: "
                   "Invalid video output format!\n");
    return FAILURE;
  }
}

/** Internal fuction to set and test video output dimensions.
* This method must be overridden by a decendant class
* with testing of the input value */
VideoGrabber::ResultType
VideoGrabber
::InternalSetVideoOutputDimensionsProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalSetVideoOutputDimensionsProcessing called...\n");
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
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalSetVideoOutputClipRectangleProcessing called...\n");
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
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalSetVideoOutputPaddingProcessing called...\n");
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
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalInitializeProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to activate grabber.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalActivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalActivateGrabberProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to deactivate grabber.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalDeactivateGrabberProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalDeactivateGrabberProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to grab one framer.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalGrabOneFrameProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalGrabOneFrameProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to start grabbing.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalStartGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalStartGrabbingProcessing called...\n");
  return SUCCESS;
}*/

/** Internal fuction to stop grabbing.
* This method must be overridden by a decendant class
* with testing of the input value */
/*VideoGrabber::ResultType
VideoGrabber
::InternalStopGrabbingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::InternalStopGrabbingProcessing called...\n");
  return SUCCESS;
}*/

void VideoGrabber::SetVideoBufferSizeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetVideoBufferSizeProcessing called...\n");
  ResultType result = this->InternalSetVideoBufferSizeProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetWantedFramerateProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetWantedFramerateProcessing called...\n");
  ResultType result = this->InternalSetWantedFramerateProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputFormatProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetVideoOutputFormatProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputFormatProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputDimensionsProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetVideoOutputDimensionsProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputDimensionsProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputClipRectangleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetVideoOutputClipRectangleProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputClipRectangleProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::SetVideoOutputPaddingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::SetVideoOutputPaddingProcessing called...\n");
  ResultType result = this->InternalSetVideoOutputPaddingProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::ReportVideoBufferSizeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportVideoBufferSizeProcessing called...\n");
  UnsignedLongEvent event;
  event.Set( m_VideoBufferSize );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoFrameNoProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportVideoFrameNoProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportNumberOfVideoFramesProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportNumberOfVideoFramesProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportTimeTagProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportTimeTagProcessing called...\n");
  // Create new event type
  IGSTKEvent event;
  //event.Set(  );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportFramerateProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportFramerateProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_Framerate );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputFormatProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportVideoOutputFormatProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_VideoOutputFormat );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputDimensionsProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportVideoOutputDimensionsProcessing called...\n");
  VideoDimensionsEvent event;
  event.Set( m_VideoOutputDimensions );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputClipRectangleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                 "::ReportVideoOutputClipRectangleProcessing called...\n");
  VideoClipRectangleEvent event;
  event.Set( m_VideoOutputClipRectangle );
  this->InvokeEvent( event );
}

void VideoGrabber::ReportVideoOutputPaddingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ReportVideoOutputPaddingProcessing called...\n");
  UnsignedIntEvent event;
  event.Set( m_VideoOutputPadding );
  this->InvokeEvent( event );
}

void VideoGrabber::InitializeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::InitializeProcessing called...\n");

  ResultType result = SUCCESS;

  // Test if all necessary variables have been set
  if (m_VideoOutputDimensions.m_Height  == 0 ||
      m_VideoOutputDimensions.m_Width   == 0 ||
      //m_VideoBufferSize                 == 0 ||
      //m_Framerate                       == 0 ||
      m_VideoOutputFormat               == 0 )
    result = FAILURE;

  if (result == SUCCESS)
    result = this->InternalInitializeProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::ActivateGrabberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::ActivateGrabberProcessing called...\n");
  ResultType result = this->InternalActivateGrabberProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::DeactivateGrabberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::DeactivateGrabberProcessing called...\n");
  ResultType result = this->InternalDeactivateGrabberProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::GrabOneFrameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::GrabOneFrameProcessing called...\n");
  ResultType result = this->InternalGrabOneFrameProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

void VideoGrabber::StartGrabbingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::StartGrabbingProcessing called...\n");
  ResultType result = this->InternalStartGrabbingProcessing();

  switch(result)
  {
    case SUCCESS:
      igstkPushInputMacro(Success);
      break;
    case WAIT:
      igstkPushInputMacro(Wait);
      break;
    case FAILURE:
    default:
      igstkPushInputMacro(Failure);
      break;
  }

  m_StateMachine.ProcessInputs();

  // Transition to GrabbingState is initiated after the grabber is ready.
  // => The function InternalGrabbingTransition() is then called.
}

/** Internal fuction to initiate transition to GrabbingState. */

void VideoGrabber::GrabbingTransition()
{
  igstkLogMacro( DEBUG, "VideoGrabber"
                 "::GrabbingTransition called ...\n");

  ResultType result = this->InternalGrabbingTransition();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );

  this->InvokeEvent(GrabbingStartedEvent());
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::StopGrabbingProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::StopGrabbingProcessing called...\n");
  ResultType result = this->InternalStopGrabbingProcessing();
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Public request methods */
void VideoGrabber::RequestSetVideoBufferSize( unsigned long bufferSize )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetVideoBufferSize called...\n");
  m_VideoBufferSizeToBeSet = bufferSize;
  igstkPushInputMacro( SetVideoBufferSize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetWantedFramerate( unsigned int framerate )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetWantedFramerate called...\n");
  m_FramerateToBeSet = framerate;
  igstkPushInputMacro( SetWantedFramerate );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputFormat( unsigned int format )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetVideoOutputFormat called...\n");
  m_VideoOutputFormatToBeSet = format;
  igstkPushInputMacro( SetVideoOutputFormat );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputDimensions( unsigned int videoHeight,
                                                    unsigned int videoWidth )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetVideoOutputDimensions called...\n");
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
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetVideoOutputClipRectangle called...\n");
  m_VideoOutputClipRectangleToBeSet.m_Height = height;
  m_VideoOutputClipRectangleToBeSet.m_Width = width;
  m_VideoOutputClipRectangleToBeSet.m_XPosition = xPosition;
  m_VideoOutputClipRectangleToBeSet.m_YPosition = yPosition;
  igstkPushInputMacro( SetVideoOutputClipRectangle );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestSetVideoOutputPadding( unsigned int padding )
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestSetVideoOutputPadding called...\n");
  m_VideoOutputPaddingToBeSet = padding;
  igstkPushInputMacro( SetVideoOutputPadding );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestOpen()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber::RequestOpen called...\n");
  igstkPushInputMacro( Open );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestClose()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber::RequestClose called...\n");
  igstkPushInputMacro( Close );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestInitialize()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestInitialize called...\n");
  igstkPushInputMacro( Initialize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStartGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestStartGrabbing called...\n");
  igstkPushInputMacro( StartGrabbing );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestStopGrabbing()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestStopGrabbing called...\n");
  igstkPushInputMacro( StopGrabbing );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGrabOneFrame()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGrabOneFrame called...\n");
  igstkPushInputMacro( GrabOneFrame );
  m_StateMachine.ProcessInputs();
}

/** Public get methods */
void VideoGrabber::RequestGetVideoBufferSize()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoBufferSize called...\n");
  igstkPushInputMacro( RequestVideoBufferSize );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoFrameNo()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoFrameNo called...\n");
  igstkPushInputMacro( RequestVideoFrameNo );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetNumberOfVideoFrames()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetNumberOfVideoFrames called...\n");
  igstkPushInputMacro( RequestNumberOfVideoFrames );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetTimeTag()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetTimeTag called...\n");
  igstkPushInputMacro( RequestTimeTag );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetFramerate()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetFramerate called...\n");
  igstkPushInputMacro( RequestFramerate );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputFormat()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoOutputFormat called...\n");
  igstkPushInputMacro( RequestVideoOutputFormat );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputDimensions()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoOutputDimensions called...\n");
  igstkPushInputMacro( RequestVideoOutputDimensions );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputClipRectangle()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoOutputClipRectangle called...\n");
  igstkPushInputMacro( RequestVideoOutputClipRectangle );
  m_StateMachine.ProcessInputs();
}

void VideoGrabber::RequestGetVideoOutputPadding()
{
  igstkLogMacro( DEBUG, "igstk::VideoGrabber"
                        "::RequestGetVideoOutputPadding called...\n");
  igstkPushInputMacro( RequestVideoOutputPadding );
  m_StateMachine.ProcessInputs();
}


} // end namespace igstk
