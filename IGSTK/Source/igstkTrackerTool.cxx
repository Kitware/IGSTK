/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerTool.h"
#include "igstkTracker.h"
#include "igstkEvents.h"


namespace igstk
{

TrackerTool::TrackerTool(void):m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // set all transforms to identity 
  m_RawTransform.SetToIdentity( 1e300 );  
  m_CalibratedTransform.SetToIdentity( 1e300 );
  m_CalibratedTransformWithRespectToReferenceTrackerTool.SetToIdentity( 1e300 );
  m_CalibrationTransform.SetToIdentity( 1e300 );  

  m_Updated = false; // not yet updated

//  m_CoordinateReferenceSystem = CoordinateReferenceSystemType::New();

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToConfigureTrackerTool );
  igstkAddStateMacro( Configured );
  igstkAddStateMacro( AttemptingToAttachTrackerToolToTracker );
  igstkAddStateMacro( Attached );
  igstkAddStateMacro( AttemptingToDetachTrackerToolFromTracker );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Tracked );

  // Set the input descriptors
  igstkAddInputMacro( ConfigureTool );
  igstkAddInputMacro( ToolConfigurationSuccess ); 
  igstkAddInputMacro( ToolConfigurationFailure ); 
  igstkAddInputMacro( AttachToolToTracker ); 
  igstkAddInputMacro( AttachmentToTrackerSuccess ); 
  igstkAddInputMacro( AttachmentToTrackerFailure ); 
  igstkAddInputMacro( TrackingStarted ); 
  igstkAddInputMacro( TrackingStopped ); 
  igstkAddInputMacro( TrackerToolVisible ); 
  igstkAddInputMacro( TrackerToolNotAvailable ); 
  igstkAddInputMacro( DetachTrackerToolFromTracker ); 
  igstkAddInputMacro( DetachmentFromTrackerSuccess ); 
  igstkAddInputMacro( DetachmentFromTrackerFailure ); 


  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           ConfigureTool,
                           AttemptingToConfigureTrackerTool,
                           AttemptToConfigure );

  // Transitions from the AttemptingToConfigure
  igstkAddTransitionMacro( AttemptingToConfigureTrackerTool,
                           ToolConfigurationSuccess,
                           Configured,
                           TrackerToolConfigurationSuccess );

  igstkAddTransitionMacro( AttemptingToConfigureTrackerTool,
                           ToolConfigurationFailure,
                           Idle,
                           TrackerToolConfigurationFailure );

  // Transition from Configured state
  igstkAddTransitionMacro( Configured,
                           AttachToolToTracker,
                           AttemptingToAttachTrackerToolToTracker,
                           AttemptToAttachTrackerToolToTracker);

 // Transitions from the AttemptingToAttachTrackerToolToTracker
  igstkAddTransitionMacro( AttemptingToAttachTrackerToolToTracker,
                           AttachmentToTrackerSuccess,
                           Attached,
                           TrackerToolAttachmentToTrackerSuccess );

  igstkAddTransitionMacro( AttemptingToAttachTrackerToolToTracker,
                           AttachmentToTrackerFailure,
                           Configured,
                           TrackerToolAttachmentToTrackerFailure );

  // Transitions from the Attached state
  igstkAddTransitionMacro( Attached,
                           DetachTrackerToolFromTracker,
                           AttemptingToDetachTrackerToolFromTracker,
                           AttemptToDetachTrackerToolFromTracker );

  igstkAddTransitionMacro( Attached,
                           TrackingStarted,
                           NotAvailable,
                           ReportTrackingStarted );

  // Transition from NotAvailable state
  igstkAddTransitionMacro( NotAvailable,
                           TrackerToolVisible,
                           Tracked,
                           ReportTrackerToolVisibleState );

  igstkAddTransitionMacro( NotAvailable,
                           TrackingStopped,
                           Attached,
                           ReportTrackingStopped );

  igstkAddTransitionMacro( NotAvailable,
                           TrackerToolNotAvailable,
                           NotAvailable,
                           No );

  // Transition from Tracked state
  igstkAddTransitionMacro( Tracked,
                           TrackerToolNotAvailable,
                           NotAvailable,
                           ReportTrackerToolNotAvailable );

  igstkAddTransitionMacro( Tracked,
                           TrackerToolVisible,
                           Tracked,
                           No );

  igstkAddTransitionMacro( Tracked,
                           TrackingStopped,
                           Attached,
                           ReportTrackingStopped );


  // Transitions from the AttemptingToDetachTrackerToolFromTracker
  igstkAddTransitionMacro( AttemptingToDetachTrackerToolFromTracker,
                           DetachmentFromTrackerSuccess,
                           Idle,
                           TrackerToolDetachmentFromTrackerSuccess );

  igstkAddTransitionMacro( AttemptingToDetachTrackerToolFromTracker,
                           DetachmentFromTrackerFailure,
                           Attached,
                           TrackerToolDetachmentFromTrackerFailure );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();

}

TrackerTool::~TrackerTool(void)
{
}

void 
TrackerTool::RequestConfigure( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::RequestConfigure called...\n");
  igstkPushInputMacro( ConfigureTool );
  this->m_StateMachine.ProcessInputs();
}

void 
TrackerTool::RequestAttachToTracker( Tracker * tracker )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::RequestAttachToTracker called...\n");

  m_TrackerToAttachTo = tracker;
  igstkPushInputMacro( AttachToolToTracker );
  this->m_StateMachine.ProcessInputs();

}

void 
TrackerTool::RequestDetach( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::RequestDetach called...\n");

  igstkPushInputMacro( DetachTrackerToolFromTracker );
  this->m_StateMachine.ProcessInputs();
}

/** The "SetTrackerToolIdentifier" method assigns an identifier to the tracker
 * tool*/
void 
TrackerTool::SetTrackerToolIdentifier( std::string identifier )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::SetTrackerToolIdentifier called...\n");
  m_TrackerToolIdentifier = identifier;
}

/** The "GetTrackerToolIdentifier" method assigns an identifier to the tracker
 * tool*/
std::string 
TrackerTool::GetTrackerToolIdentifier( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::GetTrackerToolIdentifier called...\n");
  return m_TrackerToolIdentifier;
}

/** The "AttemptToConfigureProcessing" method attempts to configure the tracker tool */
void TrackerTool::AttemptToConfigureProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerTool::AttemptToConfigureProcessing called ...\n");

  bool  result = this->CheckIfTrackerToolIsConfigured();
  
  m_StateMachine.PushInputBoolean( result,
                                   m_ToolConfigurationSuccessInput,
                                   m_ToolConfigurationFailureInput );
}

/** The "AttemptToAttachTrackerToolToTracker" method attempts to attach the tracker tool
 * to the tracker */
void TrackerTool::AttemptToAttachTrackerToolToTrackerProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerTool::AttemptToAttachTrackerToolToTracker called ...\n");

  m_TrackerToAttachTo->RequestAttachTool( this );
}

/** Report successful tracker tool attachment */ 
void TrackerTool::ReportSuccessfulTrackerToolAttachment() 
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerTool::ReportSuccessfulTrackerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToTrackerSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** Report failure in tracker tool attachment attempt */ 
void TrackerTool::ReportFailedTrackerToolAttachment()
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerTool::ReportFailedTrackerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToTrackerFailure );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToDetachTrackerToolFromTracker" method attempts to detach the tracker tool
 * to the tracker */
void TrackerTool::AttemptToDetachTrackerToolFromTrackerProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerTool::AttemptToAttachTrackerToolToTracker called ...\n");

  //implement a method in the tracker class to detach the tool
  bool result = m_TrackerToAttachTo->RequestRemoveTool( this ); 
  m_StateMachine.PushInputBoolean( result,
                                   m_DetachmentFromTrackerSuccessInput,
                                   m_DetachmentFromTrackerFailureInput );
}

/** The "CheckIfTrackerToolIsConfigured" methods returns a boolean indicating
 * if the tracker tool is configured or not. This method is to be overwritten in
 * the derived classes
 */
bool TrackerTool::CheckIfTrackerToolIsConfigured()
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::CheckIfTrackerToolIsConfigured called ...\n");
  return true;
}

/** Post-processing after a successful configuration attempt . */ 
void TrackerTool::TrackerToolConfigurationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolConfigurationSuccessProcessing called ...\n");


  this->InvokeEvent( TrackerToolConfigurationEvent() );
}


/** Post-processing after a failed configuration attempt . */ 
void TrackerTool::TrackerToolConfigurationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolConfigurationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerToolConfigurationErrorEvent() );
}

/** Post-processing after a successful tracker tool to tracker attachment attempt . */ 
void TrackerTool::TrackerToolAttachmentToTrackerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolAttachmentToTrackerSuccessiProcessing called ...\n");

  this->InvokeEvent( TrackerToolAttachmentToTrackerEvent() );
}

/** Post-processing after a failed attachment attempt . */ 
void TrackerTool::TrackerToolAttachmentToTrackerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolConfigurationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerToolAttachmentToTrackerErrorEvent() );
}

/** Post-processing after a successful detachment of the tracker tool from the
 * tracker. */ 
void TrackerTool::TrackerToolDetachmentFromTrackerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolDetachmentFromTrackerSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerToolDetachmentFromTrackerEvent() );
}

/** Post-processing after a failed detachment . */ 
void TrackerTool::TrackerToolDetachmentFromTrackerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::TrackerToolDetachmentFromTrackerFailureProcessing called ...\n");


  this->InvokeEvent( TrackerToolDetachmentFromTrackerErrorEvent() );
}

/** Report tracker tool is in a tracked state */ 
void TrackerTool::ReportTrackerToolVisibleStateProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::ReportTrackerToolVisibleStateProcessing called ...\n");

  this->InvokeEvent( TrackerToolMadeTransitionToTrackedStateEvent() );
}

/** Report tracker tool not available state*/ 
void TrackerTool::ReportTrackerToolNotAvailableProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::ReportTrackerToolNotAvailableProcessing called ...\n");

  this->InvokeEvent( TrackerToolNotAvailableToBeTrackedEvent() );
}

/** Report tracking started */ 
void TrackerTool::ReportTrackingStartedProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::ReportTrackingStarted called ...\n");

  this->InvokeEvent( ToolTrackingStartedEvent() );
}

/** Report tracking stopped */ 
void TrackerTool::ReportTrackingStoppedProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerTool::ReportTrackingStopped called ...\n");

  this->InvokeEvent( ToolTrackingStoppedEvent() );
}

/** Push TrackingStarted state input to the tracker tool */
void TrackerTool::ReportTrackingStarted( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::ReportTrackingStarted called...\n");

  igstkPushInputMacro( TrackingStarted );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackingStarted state input to the tracker tool */
void TrackerTool::ReportTrackingStopped( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::ReportTrackingStopped called...\n");

  igstkPushInputMacro( TrackingStopped );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackerToolNotAvailable input to the tracker tool */
void TrackerTool::ReportTrackingToolNotAvailable( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::ReportTrackingToolNotAvailable called...\n");

  igstkPushInputMacro( TrackerToolNotAvailable );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackerToolVisible input to the tracker tool  */
void TrackerTool::ReportTrackingToolVisible( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::ReportTrackingToolVisible called...\n");

  igstkPushInputMacro( TrackerToolVisible );
  this->m_StateMachine.ProcessInputs();
}

/** Report invalid request */
void TrackerTool::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerTool::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** No Processing for this state machine transition*/
void TrackerTool::NoProcessing( void )
{

}

/** Method to set the calibrated raw transform with respect to 
 *  a reference tracker tool.
 * This method should only be called by the Tracker */
void 
TrackerTool
::SetCalibratedTransformWithRespectToReferenceTrackerTool( const TransformType & transform )
{
  m_CalibratedTransformWithRespectToReferenceTrackerTool = 
                                                         transform;
  TransformModifiedEvent event;
  event.Set( transform );
  this->InvokeEvent( event );
}

/** Method to set the calibration transform for the tracker tool */ 
void 
TrackerTool::SetCalibrationTransform( const CalibrationTransformType & transform )
{
  m_CalibrationTransform = transform;
}

/** Method to set the raw transform for the tracker tool
 *  This method should only be called by the Tracker */ 
void 
TrackerTool::SetRawTransform( const TransformType & transform )
{
  m_RawTransform = transform;
}

/** Method to set the calibrated raw transform for the tracker tool
 *  This method should only be called by the Tracker */ 
void 
TrackerTool::SetCalibratedTransform( const TransformType & transform )
{
  m_CalibratedTransform = transform;
}

/** Attach and spatial object to be tracked.
 *  FIXME: Add a state machine to this class and pass first this 
 *         request through the transition matrix. Then invoke a
 *         AttachSpatialObjectProcessing() method.
 */
void
TrackerTool::RequestAttachSpatialObject( SpatialObject * spatialObject )
{
  // Note: the CalibrationTransform should be the transform relating
  //       the spatial object to the tracker tool. FIXME.
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
  spatialObject->RequestAttachToTrackerTool( this->m_CoordinateReferenceSystem );
#endif
}

/** Print object information */
void TrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform: "    
     << this->m_CalibratedTransformWithRespectToReferenceTrackerTool << std::endl;
  os << indent << "Raw transform: " << this->m_RawTransform << std::endl;
  os << indent << "CalibrationTransform: "
               << this->m_CalibrationTransform << std::endl;
  os << indent << "Calibrated raw transform: "
               << this->m_CalibratedTransform << std::endl;
}

std::ostream& operator<<(std::ostream& os, const TrackerTool& o)
{
  o.Print(os, 0);
  return os;  
}


}
