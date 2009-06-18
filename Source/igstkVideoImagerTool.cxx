/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVideoImagerTool.h"
#include "igstkVideoImager.h"
#include "igstkEvents.h"

#include "vtkImageData.h"

#define MAX_FRAMES 20

namespace igstk
{

VideoImagerTool::VideoImagerTool(void):m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  /** Set all transforms to identity */
  typedef double            TimePeriodType;

  this->m_Updated = false;

  /** shownFrameIndex = currentlyCapturedFrameIndex - m_Delay */
  m_Delay = 6;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToConfigureVideoImagerTool );
  igstkAddStateMacro( Configured );
  igstkAddStateMacro( AttemptingToAttachVideoImagerToolToVideoImager );
  igstkAddStateMacro( Attached );
  igstkAddStateMacro( AttemptingToDetachVideoImagerToolFromVideoImager );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Streaming );

  // Set the input descriptors
  igstkAddInputMacro( ConfigureTool );
  igstkAddInputMacro( ToolConfigurationSuccess );
  igstkAddInputMacro( ToolConfigurationFailure );
  igstkAddInputMacro( AttachToolToVideoImager );
  igstkAddInputMacro( AttachmentToVideoImagerSuccess );
  igstkAddInputMacro( AttachmentToVideoImagerFailure );
  igstkAddInputMacro( ImagingStarted );
  igstkAddInputMacro( ImagingStopped );
  igstkAddInputMacro( VideoImagerToolStreaming );
  igstkAddInputMacro( VideoImagerToolNotAvailable );
  igstkAddInputMacro( DetachVideoImagerToolFromVideoImager );
  igstkAddInputMacro( DetachmentFromVideoImagerSuccess );
  igstkAddInputMacro( DetachmentFromVideoImagerFailure );
  igstkAddInputMacro( GetFrame );

  // Programming the state machine transitions:
  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           ConfigureTool,
                           AttemptingToConfigureVideoImagerTool,
                           AttemptToConfigure );

  igstkAddTransitionMacro( Idle,
                           AttachToolToVideoImager,
                           Idle,
                           ReportInvalidRequestToAttachVideoImagerTool);

  igstkAddTransitionMacro( Idle,
                           DetachVideoImagerToolFromVideoImager,
                           Idle,
                           ReportInvalidRequestToDetachVideoImagerTool);

  // Transitions from the AttemptingToConfigure
  igstkAddTransitionMacro( AttemptingToConfigureVideoImagerTool,
                           ToolConfigurationSuccess,
                           Configured,
                           VideoImagerToolConfigurationSuccess );

  igstkAddTransitionMacro( AttemptingToConfigureVideoImagerTool,
                           ToolConfigurationFailure,
                           Idle,
                           VideoImagerToolConfigurationFailure );

  // Transition from Configured state
  igstkAddTransitionMacro( Configured,
                           AttachToolToVideoImager,
                           AttemptingToAttachVideoImagerToolToVideoImager,
                           AttemptToAttachVideoImagerToolToVideoImager);

  // Transitions from the AttemptingToAttachVideoImagerToolToVideoImager
  igstkAddTransitionMacro( AttemptingToAttachVideoImagerToolToVideoImager,
                           AttachmentToVideoImagerSuccess,
                           Attached,
                           VideoImagerToolAttachmentToVideoImagerSuccess );

  igstkAddTransitionMacro( AttemptingToAttachVideoImagerToolToVideoImager,
                           AttachmentToVideoImagerFailure,
                           Configured,
                           VideoImagerToolAttachmentToVideoImagerFailure );

  // Transitions from the Attached state
  igstkAddTransitionMacro( Attached,
                           DetachVideoImagerToolFromVideoImager,
                           AttemptingToDetachVideoImagerToolFromVideoImager,
                           AttemptToDetachVideoImagerToolFromVideoImager );

  igstkAddTransitionMacro( Attached,
                           ImagingStarted,
                           NotAvailable,
                           ReportImagingStarted );

  igstkAddTransitionMacro( Attached,
                           GetFrame,
                           Attached,
                           GetFrame );

  // Transition from NotAvailable state
  igstkAddTransitionMacro( NotAvailable,
                           VideoImagerToolStreaming,
                           Streaming,
                           ReportVideoImagerToolStreamingState );

  igstkAddTransitionMacro( NotAvailable,
                           ImagingStopped,
                           Attached,
                           ReportImagingStopped );

  igstkAddTransitionMacro( NotAvailable,
                           VideoImagerToolNotAvailable,
                           NotAvailable,
                           No );

  // Transition from Streaming state
  igstkAddTransitionMacro( Streaming,
                           VideoImagerToolNotAvailable,
                           NotAvailable,
                           ReportVideoImagerToolNotAvailable );

  igstkAddTransitionMacro( Streaming,
                           VideoImagerToolStreaming,
                           Streaming,
                           No );

  igstkAddTransitionMacro( Streaming,
                           ImagingStopped,
                           Attached,
                           ReportImagingStopped );


  // Transitions from the AttemptingToDetachVideoImagerToolFromVideoImager
  igstkAddTransitionMacro( AttemptingToDetachVideoImagerToolFromVideoImager,
                           DetachmentFromVideoImagerSuccess,
                           Idle,
                             VideoImagerToolDetachmentFromVideoImagerSuccess );

  igstkAddTransitionMacro( AttemptingToDetachVideoImagerToolFromVideoImager,
                           DetachmentFromVideoImagerFailure,
                           Attached,
                           VideoImagerToolDetachmentFromVideoImagerFailure );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  this->m_StateMachine.SetReadyToRun();

  this->m_CalibrationCoordinateSystem = CoordinateSystem::New();

  this->m_FrameDimensions[0] = 0;
  this->m_FrameDimensions[1] = 0;
  this->m_FrameDimensions[2] = 0;
  this->m_PixelDepth = 0;

  m_MaxBufferSize=MAX_FRAMES;
  m_Index=0;
  m_NumberOfFramesInBuffer=0;

  m_FrameRingBuffer = new std::vector< igstk::Frame* >(MAX_FRAMES);

  /*
  std::ofstream ofile;
  ofile.open("VideoImagerToolStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();
  */

}

VideoImagerTool::~VideoImagerTool(void)
{
}

void
VideoImagerTool::RequestGetFrame( )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::RequestGetFrame called...\n");
  igstkPushInputMacro( GetFrame );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerTool::RequestConfigure( )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::RequestConfigure called...\n");
  igstkPushInputMacro( ConfigureTool );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerTool::RequestAttachToVideoImager( VideoImager * videoVideoImager )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::RequestAttachToVideoImager called...\n");

  this->m_VideoImagerToAttachTo = videoVideoImager;
  igstkPushInputMacro( AttachToolToVideoImager );
  this->m_StateMachine.ProcessInputs();

}

void
VideoImagerTool::RequestDetachFromVideoImager( )
{
  igstkLogMacro( DEBUG,
            "igstk::VideoImagerTool::RequestDetachFromVideoImager called...\n");

  igstkPushInputMacro( DetachVideoImagerToolFromVideoImager );
  this->m_StateMachine.ProcessInputs();
}

/** The "SetVideoImagerToolIdentifier" method assigns an identifier
 * to the VideoImager tool. */
void
VideoImagerTool::SetVideoImagerToolIdentifier( const std::string identifier )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::SetVideoImagerToolIdentifier called...\n");
  this->m_VideoImagerToolIdentifier = identifier;
}

/** The "GetVideoImagerToolIdentifier" method assigns an identifier
 * to the VideoImager tool. */
const std::string
VideoImagerTool::GetVideoImagerToolIdentifier( ) const
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::GetVideoImagerToolIdentifier called...\n");
  return this->m_VideoImagerToolIdentifier;
}


void VideoImagerTool::GetFrameProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::GetFrameProcessing called ...\n");

  igstk::FrameModifiedEvent  event;
  event.Set( *this->GetInternalFrame() );
  this->InvokeEvent( event );
}

/** The "AttemptToConfigureProcessing" method attempts to configure
 * the VideoImager tool. */
void VideoImagerTool::AttemptToConfigureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::AttemptToConfigureProcessing called ...\n");

  bool  result = this->CheckIfVideoImagerToolIsConfigured();

  this->m_StateMachine.PushInputBoolean( result,
                                   this->m_ToolConfigurationSuccessInput,
                                   this->m_ToolConfigurationFailureInput );
}

/** The "AttemptToAttachVideoImagerToolToVideoImager" method attempts to attach
 * the VideoImager tool to the VideoImager. */
void VideoImagerTool
::AttemptToAttachVideoImagerToolToVideoImagerProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
                 << "AttemptToAttachVideoImagerToolToVideoImager called ...\n");

  this->m_VideoImagerToAttachTo->RequestAttachTool( this );
}

/** Push AttachmentToVideoImagerSuccess input to the VideoImager tool*/
void VideoImagerTool::RequestReportSuccessfulVideoImagerToolAttachment()
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
                   << "ReportSuccessfulVideoImagerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToVideoImagerSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** Push AttachmentToVideoImagerFailure input to the VideoImager tool*/
void VideoImagerTool::RequestReportFailedVideoImagerToolAttachment()
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
                       << "ReportFailedVideoImagerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToVideoImagerFailure );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToDetachVideoImagerToolFromVideoImager" method attempts to
 * detach the VideoImager tool from the VideoImager. */
void VideoImagerTool
::AttemptToDetachVideoImagerToolFromVideoImagerProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
                 << "AttemptToAttachVideoImagerToolToVideoImager called ...\n");

  //implement a method in the VideoImager class to detach the tool
  bool result = this->m_VideoImagerToAttachTo->RequestRemoveTool( this );
  this->m_StateMachine.PushInputBoolean( result,
                                this->m_DetachmentFromVideoImagerSuccessInput,
                                this->m_DetachmentFromVideoImagerFailureInput );
}

/** Report invalid request to attach the VideoImager tool. */
void VideoImagerTool::
ReportInvalidRequestToAttachVideoImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
       << "ReportInvalidRequestToAttachVideoImagerToolProcessing called ...\n");

  this->InvokeEvent( InvalidRequestToAttachVideoImagerToolErrorEvent() );
}

/** Report invalid request to detach the VideoImager tool. */
void VideoImagerTool
::ReportInvalidRequestToDetachVideoImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
       << "ReportInvalidRequestToDetachVideoImagerToolProcessing called ...\n");

  this->InvokeEvent( InvalidRequestToDetachVideoImagerToolErrorEvent() );
}

/** Post-processing after a successful configuration attempt . */
void VideoImagerTool::VideoImagerToolConfigurationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::VideoImagerToolConfigurationSuccessProcessing "
    << "called ...\n");

  this->InvokeEvent( VideoImagerToolConfigurationEvent() );
}

/** Post-processing after a failed configuration attempt . */
void VideoImagerTool::VideoImagerToolConfigurationFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::VideoImagerToolConfigurationFailureProcessing "
    << "called ...\n");

  this->InvokeEvent( VideoImagerToolConfigurationErrorEvent() );
}

/** Post-processing after a successful VideoImager tool to VideoImager
 * attachment attempt. */
void VideoImagerTool
::VideoImagerToolAttachmentToVideoImagerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
    << "VideoImagerToolAttachmentToVideoImagerSuccessiProcessing called ...\n");

  this->InvokeEvent( VideoImagerToolAttachmentToVideoImagerEvent() );
}

/** Post-processing after a failed attachment attempt . */
void VideoImagerTool
::VideoImagerToolAttachmentToVideoImagerFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::VideoImagerToolConfigurationFailureProcessing "
    << "called ...\n");

  this->InvokeEvent( VideoImagerToolAttachmentToVideoImagerErrorEvent() );
}

/** Post-processing after a successful detachment of the VideoImager tool
 * from the VideoImager. */
void VideoImagerTool
::VideoImagerToolDetachmentFromVideoImagerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
   << "VideoImagerToolDetachmentFromVideoImagerSuccessProcessing called ...\n");

  this->InvokeEvent( VideoImagerToolDetachmentFromVideoImagerEvent() );
}

/** Post-processing after a failed detachment . */
void VideoImagerTool
::VideoImagerToolDetachmentFromVideoImagerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
   << "VideoImagerToolDetachmentFromVideoImagerFailureProcessing called ...\n");

  this->InvokeEvent( VideoImagerToolDetachmentFromVideoImagerErrorEvent() );
}

/** Report VideoImager tool is in a Streaming state */
void VideoImagerTool::ReportVideoImagerToolStreamingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
               << "ReportVideoImagerToolStreamingStateProcessing called ...\n");

  this->InvokeEvent( VideoImagerToolMadeTransitionToStreamingStateEvent() );
}

/** Report VideoImager tool not available state. */
void VideoImagerTool::ReportVideoImagerToolNotAvailableProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerTool::"
                 << "ReportVideoImagerToolNotAvailableProcessing called ...\n");
  this->InvokeEvent( VideoImagerToolNotAvailableEvent() );
}

/** Report imaging started */
void VideoImagerTool::ReportImagingStartedProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::ReportImagingStarted called ...\n");

  this->InvokeEvent( ToolImagingStartedEvent() );
}

/** Report imaging stopped */
void VideoImagerTool::ReportImagingStoppedProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::ReportImagingStopped called ...\n");

  this->InvokeEvent( ToolImagingStoppedEvent() );
}

/** Push ImagingStarted state input to the VideoImager tool */
void VideoImagerTool::RequestReportImagingStarted( )
{
  igstkLogMacro( DEBUG,
          "igstk::VideoImagerTool::RequestReportImagingStarted called...\n");

  igstkPushInputMacro( ImagingStarted );
  this->m_StateMachine.ProcessInputs();
}

/** Push ImagingStopped state input to the VideoImager tool */
void VideoImagerTool::RequestReportImagingStopped( )
{
  igstkLogMacro( DEBUG,
  "igstk::VideoImagerTool::RequestReportImagingStopped called...\n");

  igstkPushInputMacro( ImagingStopped );
  this->m_StateMachine.ProcessInputs();
}

/** Push VideoImagerToolNotAvailable input to the VideoImager tool */
void VideoImagerTool::RequestReportImagingToolNotAvailable( )
{
  igstkLogMacro( DEBUG,
  "igstk::VideoImagerTool::RequestReportImagingToolNotAvailable called ");

  igstkPushInputMacro( VideoImagerToolNotAvailable );
  this->m_StateMachine.ProcessInputs();
}

/** Push VideoImagerToolStreaming input to the VideoImager tool  */
void VideoImagerTool::RequestReportImagingToolStreaming( )
{
  igstkLogMacro( DEBUG,
  "igstk::VideoImagerTool::RequestReportImagingToolStreaming called...\n");

  igstkPushInputMacro( VideoImagerToolStreaming );
  this->m_StateMachine.ProcessInputs();
}

/** Report invalid request */
void VideoImagerTool::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::VideoImagerTool::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** No Processing for this state machine transition. */
void VideoImagerTool::NoProcessing( void )
{
}

/** Method to get the internal frame of the VideoImager tool
 *  This method should only be called by the VideoImager */
VideoImagerTool::FrameType*
VideoImagerTool::GetInternalFrame( )
{
   return GetFrameFromBuffer( m_Index );
}

/** Method to set the internal frame for the VideoImager tool
 *  This method should only be called by the VideoImager */
void
VideoImagerTool::SetInternalFrame( FrameType* frame )
{
  this->AddFrameToBuffer(frame);
}

void
VideoImagerTool::SetFrameDimensions(unsigned int *dims)
{
  this->m_FrameDimensions[0] = dims[0];
  this->m_FrameDimensions[1] = dims[1];
  this->m_FrameDimensions[2] = dims[2];

  for(unsigned int i=0;i<MAX_FRAMES;i++)
    {
    igstk::Frame* frame = new igstk::Frame();
    frame->SetFrameDimensions(
    this->m_FrameDimensions[0],
    this->m_FrameDimensions[1],
    this->m_FrameDimensions[2]);
    AddFrameToBuffer(frame);
    }
}

void
VideoImagerTool::GetFrameDimensions(unsigned int *dims)
{
  dims[0] = this->m_FrameDimensions[0];
  dims[1] = this->m_FrameDimensions[1];
  dims[2] = this->m_FrameDimensions[2];
}

igstk::Frame* VideoImagerTool::GetFrameFromBuffer(const unsigned int index)
{
  try
    {
    return m_FrameRingBuffer->at(index);
    }
  catch( std::exception& e )
    {
    igstkLogMacro( FATAL,
    "Exception in GetFrameFromBuffer (igstkVideoImagerTool): " << e.what());
    return NULL;
    }
}

igstk::Frame* VideoImagerTool::GetTemporalCalibratedFrame()
{
  try
    {
    return m_FrameRingBuffer->at((MAX_FRAMES + (m_Index - m_Delay))%MAX_FRAMES);
    }
  catch( std::exception& e )
    {
    igstkLogMacro( FATAL,
              "Exception in GetTemporalCalibratedFrame (igstkVideoImagerTool): "
                                                                   << e.what());
    return NULL;
    }
}

void VideoImagerTool::AddFrameToBuffer(igstk::Frame* frame)
{
  try
    {
    m_FrameRingBuffer->at(m_Index) = frame;
    }
  catch( std::exception& e )
    {
    igstkLogMacro( FATAL,
    "Exception in AddFrameToBuffer (igstkVideoImagerTool): " << e.what());
    }

  m_Index=(m_Index + 1) % m_MaxBufferSize;

  if (m_NumberOfFramesInBuffer < m_MaxBufferSize)
    {
    m_NumberOfFramesInBuffer += 1;
    }
}

/** Print object information */
void VideoImagerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

std::ostream& operator<<(std::ostream& os, const VideoImagerTool& o)
{
  o.Print(os, 0);
  return os;
}

}
