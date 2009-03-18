/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkImagerTool.h"
#include "igstkImager.h"
#include "igstkEvents.h"

#include "vtkImageData.h"

#define MAX_FRAMES 50

namespace igstk
{

ImagerTool::ImagerTool(void):m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // set all transforms to identity
  typedef double            TimePeriodType;

  this->m_Updated = false; // not yet updated
  //TODO set delay
  m_Delay = 6;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToConfigureImagerTool );
  igstkAddStateMacro( Configured );
  igstkAddStateMacro( AttemptingToAttachImagerToolToImager );
  igstkAddStateMacro( Attached );
  igstkAddStateMacro( AttemptingToDetachImagerToolFromImager );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Tracked );

  // Set the input descriptors
  igstkAddInputMacro( ConfigureTool );
  igstkAddInputMacro( ToolConfigurationSuccess );
  igstkAddInputMacro( ToolConfigurationFailure );
  igstkAddInputMacro( AttachToolToImager );
  igstkAddInputMacro( AttachmentToImagerSuccess );
  igstkAddInputMacro( AttachmentToImagerFailure );
  igstkAddInputMacro( ImagingStarted );
  igstkAddInputMacro( ImagingStopped );
  igstkAddInputMacro( ImagerToolVisible );
  igstkAddInputMacro( ImagerToolNotAvailable );
  igstkAddInputMacro( DetachImagerToolFromImager );
  igstkAddInputMacro( DetachmentFromImagerSuccess );
  igstkAddInputMacro( DetachmentFromImagerFailure );
  igstkAddInputMacro( GetFrame );

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           ConfigureTool,
                           AttemptingToConfigureImagerTool,
                           AttemptToConfigure );

  igstkAddTransitionMacro( Idle,
                           AttachToolToImager,
                           Idle,
                           ReportInvalidRequestToAttachImagerTool);

  igstkAddTransitionMacro( Idle,
                           DetachImagerToolFromImager,
                           Idle,
                           ReportInvalidRequestToDetachImagerTool);


  // Transitions from the AttemptingToConfigure
  igstkAddTransitionMacro( AttemptingToConfigureImagerTool,
                           ToolConfigurationSuccess,
                           Configured,
                           ImagerToolConfigurationSuccess );

  igstkAddTransitionMacro( AttemptingToConfigureImagerTool,
                           ToolConfigurationFailure,
                           Idle,
                           ImagerToolConfigurationFailure );

  // Transition from Configured state
  igstkAddTransitionMacro( Configured,
                           AttachToolToImager,
                           AttemptingToAttachImagerToolToImager,
                           AttemptToAttachImagerToolToImager);

 // Transitions from the AttemptingToAttachImagerToolToImager
  igstkAddTransitionMacro( AttemptingToAttachImagerToolToImager,
                           AttachmentToImagerSuccess,
                           Attached,
                           ImagerToolAttachmentToImagerSuccess );

  igstkAddTransitionMacro( AttemptingToAttachImagerToolToImager,
                           AttachmentToImagerFailure,
                           Configured,
                           ImagerToolAttachmentToImagerFailure );

  // Transitions from the Attached state
  igstkAddTransitionMacro( Attached,
                           DetachImagerToolFromImager,
                           AttemptingToDetachImagerToolFromImager,
                           AttemptToDetachImagerToolFromImager );

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
                           ImagerToolVisible,
                           Tracked,
                           ReportImagerToolVisibleState );

  igstkAddTransitionMacro( NotAvailable,
                           ImagingStopped,
                           Attached,
                           ReportImagingStopped );

  igstkAddTransitionMacro( NotAvailable,
                           ImagerToolNotAvailable,
                           NotAvailable,
                           No );

  // Transition from Tracked state
  igstkAddTransitionMacro( Tracked,
                           ImagerToolNotAvailable,
                           NotAvailable,
                           ReportImagerToolNotAvailable );

  igstkAddTransitionMacro( Tracked,
                           ImagerToolVisible,
                           Tracked,
                           No );

  igstkAddTransitionMacro( Tracked,
                           ImagingStopped,
                           Attached,
                           ReportImagingStopped );


  // Transitions from the AttemptingToDetachImagerToolFromImager
  igstkAddTransitionMacro( AttemptingToDetachImagerToolFromImager,
                           DetachmentFromImagerSuccess,
                           Idle,
                             ImagerToolDetachmentFromImagerSuccess );

  igstkAddTransitionMacro( AttemptingToDetachImagerToolFromImager,
                           DetachmentFromImagerFailure,
                           Attached,
                           ImagerToolDetachmentFromImagerFailure );

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
  m_FrameRingBuffer = new std::vector< igstk::Frame >(MAX_FRAMES);
  for(unsigned int i=0;i<MAX_FRAMES;i++)
  {
    igstk::Frame frame;
    AddFrameToBuffer(frame);
  }
  cerr << m_FrameRingBuffer->at(10).GetImagePtr() << "FrameConstructor" << endl;

}

ImagerTool::~ImagerTool(void)
{
}

void
ImagerTool::RequestGetFrame( )
{
  igstkLogMacro( DEBUG, "igstk::ImagerTool::RequestGetFrame called...\n");
  igstkPushInputMacro( GetFrame );
  this->m_StateMachine.ProcessInputs();
}

void
ImagerTool::RequestConfigure( )
{
  igstkLogMacro( DEBUG, "igstk::ImagerTool::RequestConfigure called...\n");
  igstkPushInputMacro( ConfigureTool );
  this->m_StateMachine.ProcessInputs();
}

void
ImagerTool::RequestAttachToImager( Imager * imager )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::RequestAttachToImager called...\n");

  this->m_ImagerToAttachTo = imager;
  igstkPushInputMacro( AttachToolToImager );
  this->m_StateMachine.ProcessInputs();

}

void
ImagerTool::RequestDetachFromImager( )
{
  igstkLogMacro( DEBUG,
                 "igstk::ImagerTool::RequestDetachFromImager called...\n");

  igstkPushInputMacro( DetachImagerToolFromImager );
  this->m_StateMachine.ProcessInputs();
}

/** The "SetImagerToolIdentifier" method assigns an identifier
 * to the imager tool. */
void
ImagerTool::SetImagerToolIdentifier( const std::string identifier )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::SetImagerToolIdentifier called...\n");
  this->m_ImagerToolIdentifier = identifier;
}

/** The "GetImagerToolIdentifier" method assigns an identifier
 * to the imager tool. */
const std::string
ImagerTool::GetImagerToolIdentifier( ) const
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::GetImagerToolIdentifier called...\n");
  return this->m_ImagerToolIdentifier;
}


void ImagerTool::GetFrameProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::GetFrameProcessing called ...\n");
cout << "getframeprocessing called" << endl;
  igstk::FrameModifiedEvent  event;
  event.Set( this->GetInternalFrame() );
  this->InvokeEvent( event );
}

/** The "AttemptToConfigureProcessing" method attempts to configure
 * the imager tool. */
void ImagerTool::AttemptToConfigureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::AttemptToConfigureProcessing called ...\n");

  bool  result = this->CheckIfImagerToolIsConfigured();

  this->m_StateMachine.PushInputBoolean( result,
                                   this->m_ToolConfigurationSuccessInput,
                                   this->m_ToolConfigurationFailureInput );
}

/** The "AttemptToAttachImagerToolToImager" method attempts to attach the
 * imager tool to the imager. */
void ImagerTool::AttemptToAttachImagerToolToImagerProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::AttemptToAttachImagerToolToImager called ...\n");

  this->m_ImagerToAttachTo->RequestAttachTool( this );
}

/** Push AttachmentToImagerSuccess input to the imager tool*/
void ImagerTool::RequestReportSuccessfulImagerToolAttachment()
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportSuccessfulImagerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToImagerSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** Push AttachmentToImagerFailure input to the imager tool*/
void ImagerTool::RequestReportFailedImagerToolAttachment()
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportFailedImagerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToImagerFailure );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToDetachImagerToolFromImager" method attempts to detach the
 * imager tool to the imager. */
void ImagerTool::AttemptToDetachImagerToolFromImagerProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::AttemptToAttachImagerToolToImager called ...\n");

  //implement a method in the imager class to detach the tool
  bool result = this->m_ImagerToAttachTo->RequestRemoveTool( this );
  this->m_StateMachine.PushInputBoolean( result,
                                   this->m_DetachmentFromImagerSuccessInput,
                                   this->m_DetachmentFromImagerFailureInput );
}

/** Report invalid request to attach the imager tool. */
void ImagerTool::ReportInvalidRequestToAttachImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportInvalidRequestToAttachImagerToolProcessing "
    << "called ...\n");


  this->InvokeEvent( InvalidRequestToAttachImagerToolErrorEvent() );
}

/** Report invalid request to detach the imager tool. */
void ImagerTool::ReportInvalidRequestToDetachImagerToolProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportInvalidRequestToDetachImagerToolProcessing "
    << "called ...\n");


  this->InvokeEvent( InvalidRequestToDetachImagerToolErrorEvent() );
}


/** Post-processing after a successful configuration attempt . */
void ImagerTool::ImagerToolConfigurationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolConfigurationSuccessProcessing "
    << "called ...\n");

  this->InvokeEvent( ImagerToolConfigurationEvent() );
}


/** Post-processing after a failed configuration attempt . */
void ImagerTool::ImagerToolConfigurationFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolConfigurationFailureProcessing "
    << "called ...\n");

  this->InvokeEvent( ImagerToolConfigurationErrorEvent() );
}

/** Post-processing after a successful imager tool to imager attachment
 * attempt. */
void ImagerTool::ImagerToolAttachmentToImagerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolAttachmentToImagerSuccessiProcessing "
    << "called ...\n");

  this->InvokeEvent( ImagerToolAttachmentToImagerEvent() );
}

/** Post-processing after a failed attachment attempt . */
void ImagerTool::ImagerToolAttachmentToImagerFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolConfigurationFailureProcessing "
    << "called ...\n");

  this->InvokeEvent( ImagerToolAttachmentToImagerErrorEvent() );
}

/** Post-processing after a successful detachment of the imager tool from the
 * imager. */
void ImagerTool::ImagerToolDetachmentFromImagerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolDetachmentFromImagerSuccessProcessing "
    << "called ...\n");

  this->InvokeEvent( ImagerToolDetachmentFromImagerEvent() );
}

/** Post-processing after a failed detachment . */
void ImagerTool::ImagerToolDetachmentFromImagerFailureProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ImagerToolDetachmentFromImagerFailureProcessing "
    << "called ...\n");


  this->InvokeEvent( ImagerToolDetachmentFromImagerErrorEvent() );
}

/** Report imager tool is in a tracked state */
void ImagerTool::ReportImagerToolVisibleStateProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportImagerToolVisibleStateProcessing called ...\n");

  this->InvokeEvent( ImagerToolMadeTransitionToTrackedStateEvent() );
}

/** Report imager tool not available state. */
void ImagerTool::ReportImagerToolNotAvailableProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportImagerToolNotAvailableProcessing called ...\n");

  this->InvokeEvent( ImagerToolNotAvailableToBeTrackedEvent() );
}

/** Report tracking started */
void ImagerTool::ReportImagingStartedProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportImagingStarted called ...\n");

  this->InvokeEvent( ToolImagingStartedEvent() );
}

/** Report tracking stopped */
void ImagerTool::ReportImagingStoppedProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportImagingStopped called ...\n");

  this->InvokeEvent( ToolImagingStoppedEvent() );
}

/** Push ImagingStarted state input to the imager tool */
void ImagerTool::RequestReportImagingStarted( )
{
  igstkLogMacro( DEBUG,
          "igstk::ImagerTool::RequestReportImagingStarted called...\n");

  igstkPushInputMacro( ImagingStarted );
  this->m_StateMachine.ProcessInputs();
}

/** Push ImagingStopped state input to the imager tool */
void ImagerTool::RequestReportImagingStopped( )
{
  igstkLogMacro( DEBUG,
                "igstk::ImagerTool::RequestReportImagingStopped called...\n");

  igstkPushInputMacro( ImagingStopped );
  this->m_StateMachine.ProcessInputs();
}

/** Push ImagerToolNotAvailable input to the imager tool */
void ImagerTool::RequestReportImagingToolNotAvailable( )
{
  igstkLogMacro( DEBUG,
  "igstk::ImagerTool::RequestReportImagingToolNotAvailable called ");

  igstkPushInputMacro( ImagerToolNotAvailable );
  this->m_StateMachine.ProcessInputs();
}

/** Push ImagerToolVisible input to the imager tool  */
void ImagerTool::RequestReportImagingToolVisible( )
{
  igstkLogMacro( DEBUG, "igstk::ImagerTool::RequestReportImagingToolVisible "
  << "called...\n");

  igstkPushInputMacro( ImagerToolVisible );
  this->m_StateMachine.ProcessInputs();
}

/** Report invalid request */
void ImagerTool::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagerTool::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** No Processing for this state machine transition. */
void ImagerTool::NoProcessing( void )
{

}

/** Method to get the internal frame of the imager tool
 *  This method should only be called by the Imager */
ImagerTool::FrameType
ImagerTool::GetInternalFrame( )
{
   return GetFrameFromBuffer( m_Index );
}

/** Method to set the internal frame for the imager tool
 *  This method should only be called by the Imager */
void
ImagerTool::SetInternalFrame( const FrameType & frame )
{
  this->AddFrameToBuffer(frame);
}

void
ImagerTool::SetFrameDimensions(unsigned int *dims)
{
  this->m_FrameDimensions[0] = dims[0];
  this->m_FrameDimensions[1] = dims[1];
  this->m_FrameDimensions[2] = dims[2];
}

void
ImagerTool::GetFrameDimensions(unsigned int *dims)
{
  dims[0] = this->m_FrameDimensions[0];
  dims[1] = this->m_FrameDimensions[1];
  dims[2] = this->m_FrameDimensions[2];
}

igstk::Frame ImagerTool::GetFrameFromBuffer(const unsigned int index)
{
  try
  {
    return m_FrameRingBuffer->at(index);
  }
  catch( std::exception& e )
  {
    cout << endl << "Exception in GetFrameFromBuffer (ImagingSourceImager): "
    << e.what() << endl;
  }
//  if (m_NumberOfFramesInBuffer < m_MaxBufferSize)
//  {
//    m_NumberOfFramesInBuffer += 1;
//  }
}

igstk::Frame ImagerTool::GetTemporalCalibratedFrame()
{
  try
  {
    return m_FrameRingBuffer->at((MAX_FRAMES + (m_Index - m_Delay)) % MAX_FRAMES);
  }
  catch( std::exception& e )
  {
    cout << endl << "Exception in GetFrameFromBuffer (ImagingSourceImager): "
    << e.what() << endl;
  }
//  if (m_NumberOfFramesInBuffer < m_MaxBufferSize)
//  {
//    m_NumberOfFramesInBuffer += 1;
//  }
}

void ImagerTool::AddFrameToBuffer(const igstk::Frame& frame)
{
  try
  {
    m_FrameRingBuffer->at(m_Index)=frame;
  }
  catch( std::exception& e )
  {
    cout << endl << "Exception in AddFrameToBuffer (ImagingSourceImager): "
    << e.what() << endl;
  }
  m_Index=(m_Index + 1) % m_MaxBufferSize;

  if (m_NumberOfFramesInBuffer < m_MaxBufferSize)
  {
    m_NumberOfFramesInBuffer += 1;
  }
}

/** Method to set the calibration transform for the imager tool*/
//void
//ImagerTool::SetCalibrationTransform( const TransformType & transform )
//{
//  this->m_CalibrationTransform = transform;
//}

/** Print object information */
void ImagerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
//TODO
  //os << indent << "Raw frame: " << GetInternalFrame() << std::endl;
}

std::ostream& operator<<(std::ostream& os, const ImagerTool& o)
{
  o.Print(os, 0);
  return os;
}


}
