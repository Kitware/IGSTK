/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolNew.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerToolNew.h"
#include "igstkTrackerNew.h"
#include "igstkEvents.h"


namespace igstk
{

TrackerToolNew::TrackerToolNew(void):m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // Configure the variables
  m_RawTransform.SetToIdentity( 1e300 );  
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

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

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


  m_StateMachine.SetReadyToRun();

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

}

TrackerToolNew::~TrackerToolNew(void)
{
}

void 
TrackerToolNew::RequestConfigure( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::RequestConfigure called...\n");
  igstkPushInputMacro( ConfigureTool );
  this->m_StateMachine.ProcessInputs();
}

void 
TrackerToolNew::RequestAttachToTracker( TrackerNew * tracker )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::RequestAttachToTracker called...\n");

  m_Tracker = tracker;
  igstkPushInputMacro( AttachToolToTracker );
  this->m_StateMachine.ProcessInputs();

}

void 
TrackerToolNew::RequestDetach( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::RequestDetach called...\n");

  igstkPushInputMacro( DetachTrackerToolFromTracker );
  this->m_StateMachine.ProcessInputs();
}

/** The "SetTrackerToolIdentifier" method assigns an identifier to the tracker
 * tool*/
void 
TrackerToolNew::SetTrackerToolIdentifier( std::string identifier )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::SetTrackerToolIdentifier called...\n");
  m_TrackerToolIdentifier = identifier;
}

/** The "GetTrackerToolIdentifier" method assigns an identifier to the tracker
 * tool*/
std::string 
TrackerToolNew::GetTrackerToolIdentifier( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::GetTrackerToolIdentifier called...\n");
  return m_TrackerToolIdentifier;
}

/** The "AttemptToConfigureProcessing" method attempts to configure the tracker tool */
void TrackerToolNew::AttemptToConfigureProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToConfigureProcessing called ...\n");

  bool  result = this->CheckIfTrackerToolIsConfigured();
  
  m_StateMachine.PushInputBoolean( result,
                                   m_ToolConfigurationSuccessInput,
                                   m_ToolConfigurationFailureInput );
}

/** The "AttemptToAttachTrackerToolToTracker" method attempts to attach the tracker tool
 * to the tracker */
void TrackerToolNew::AttemptToAttachTrackerToolToTrackerProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToAttachTrackerToolToTracker called ...\n");

  m_Tracker->RequestAttachTool( m_TrackerToolIdentifier, this );
}

/** Report successful tracker tool attachment */ 
void TrackerToolNew::ReportSuccessfulTrackerToolAttachment() 
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::ReportSuccessfulTrackerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToTrackerSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** Report failure in tracker tool attachment attempt */ 
void TrackerToolNew::ReportFailedTrackerToolAttachment()
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::ReportFailedTrackerToolAttachment called ...\n");

  igstkPushInputMacro( AttachmentToTrackerFailure );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToDetachTrackerToolFromTracker" method attempts to detach the tracker tool
 * to the tracker */
void TrackerToolNew::AttemptToDetachTrackerToolFromTrackerProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToAttachTrackerToolToTracker called ...\n");

  //FIXME: implement a method in the tracker class to detach the tool
  bool result = m_Tracker->RequestRemoveTool( m_TrackerToolIdentifier );
  m_StateMachine.PushInputBoolean( result,
                                   m_DetachmentFromTrackerSuccessInput,
                                   m_DetachmentFromTrackerFailureInput );
}

/** The "CheckIfTrackerToolIsConfigured" methods returns a boolean indicating
 * if the tracker tool is configured or not. This method is to be overriden in
 * the dervied classes
 */
bool TrackerToolNew::CheckIfTrackerToolIsConfigured()
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::CheckIfTrackerToolIsConfigured called ...\n");
  return true;
}

/** Post-processing after a successful configuration attempt . */ 
void TrackerToolNew::TrackerToolConfigurationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolConfigurationSuccessProcessing called ...\n");


  this->InvokeEvent( TrackerToolNewConfigurationEvent() );
}


/** Post-processing after a failed configuration attempt . */ 
void TrackerToolNew::TrackerToolConfigurationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolConfigurationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewConfigurationErrorEvent() );
}

/** Post-processing after a successful tracker tool to tracker attachment attempt . */ 
void TrackerToolNew::TrackerToolAttachmentToTrackerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolAttachmentToTrackerSuccessiProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewAttachmentToTrackerEvent() );
}

/** Post-processing after a failed attachment attempt . */ 
void TrackerToolNew::TrackerToolAttachmentToTrackerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolConfigurationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewAttachmentToTrackerErrorEvent() );
}

/** Post-processing after a successful detachment of the tracker tool from the
 * tracker. */ 
void TrackerToolNew::TrackerToolDetachmentFromTrackerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolDetachmentFromTrackerSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewDetachmentFromTrackerEvent() );
}

/** Post-processing after a failed detachment . */ 
void TrackerToolNew::TrackerToolDetachmentFromTrackerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolDetachmentFromTrackerFailureProcessing called ...\n");


  this->InvokeEvent( TrackerToolNewDetachmentFromTrackerErrorEvent() );
}

/** Report tracker tool is in a tracked state */ 
void TrackerToolNew::ReportTrackerToolVisibleStateProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportTrackerToolVisibleStateProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewMadeTransitionToTrackedStateEvent() );
}

/** Report tracker tool not available state*/ 
void TrackerToolNew::ReportTrackerToolNotAvailableProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportTrackerToolNotAvailableProcessing called ...\n");

  this->InvokeEvent( TrackerToolNewNotAvailableToBeTrackedEvent() );
}

/** Report tracking started */ 
void TrackerToolNew::ReportTrackingStartedProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportTrackingStarted called ...\n");

  this->InvokeEvent( ToolTrackingStartedEvent() );
}

/** Report tracking stopped */ 
void TrackerToolNew::ReportTrackingStoppedProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportTrackingStopped called ...\n");

  this->InvokeEvent( ToolTrackingStoppedEvent() );
}

/** Push TrackingStarted state input to the tracker tool */
void TrackerToolNew::ReportTrackingStarted( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::ReportTrackingStarted called...\n");

  igstkPushInputMacro( TrackingStarted );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackingStarted state input to the tracker tool */
void TrackerToolNew::ReportTrackingStopped( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::ReportTrackingStopped called...\n");

  igstkPushInputMacro( TrackingStopped );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackerToolNotAvailable input to the tracker tool */
void TrackerToolNew::ReportTrackingToolNotAvailable( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::ReportTrackingToolNotAvailable called...\n");

  igstkPushInputMacro( TrackerToolNotAvailable );
  this->m_StateMachine.ProcessInputs();
}

/** Push TrackerToolVisible input to the tracker tool  */
void TrackerToolNew::ReportTrackingToolIsInVisibleState( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::ReportTrackingToolIsInVisibleState called...\n");

  igstkPushInputMacro( TrackerToolVisible );
  this->m_StateMachine.ProcessInputs();
}

/** Report invalid request */
void TrackerToolNew::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** No Processing for this state machine transition*/
void TrackerToolNew::NoProcessing( void )
{

}

/** Method to set the transform (compsition of raw and calibration transform)
 * This method should only be called by the Tracker */
void 
TrackerToolNew::RequestSetTransform( const TransformType & transform )
{
  m_Transform = transform;

  TransformModifiedEvent event;
  event.Set( transform );
  this->InvokeEvent( event );
}

/** Method to set the calibration transfrom for the tracker tool */ 
void 
TrackerToolNew::RequestSetCalibrationTransform( const CalibrationTransformType & transform )
{
  m_CalibrationTransform = transform;
}

/** Method to set the raw transfrom for the tracker tool */ 
void 
TrackerToolNew::RequestSetRawTransform( const TransformType & transform )
{
  m_RawTransform = transform;
}

/** Attach and spatial object to be tracked.
 *  FIXME: Add a state machine to this class and pass first this 
 *         request through the transition matrix. Then invoke a
 *         AttachSpatialObjectProcessing() method.
 */
void
TrackerToolNew::RequestAttachSpatialObject( SpatialObject * spatialObject )
{
  // Note: the CalibrationTransform should be the transform relating
  //       the spatial object to the tracker tool. FIXME.
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
  spatialObject->RequestAttachToTrackerTool( this->m_CoordinateReferenceSystem );
#endif
}

/** Print object information */
void TrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform: "     << this->m_Transform << std::endl;
  os << indent << "Raw transform: " << this->m_RawTransform << std::endl;
  os << indent << "CalibrationTransform: "
               << this->m_CalibrationTransform << std::endl;
}

std::ostream& operator<<(std::ostream& os, const TrackerToolNew& o)
{
  o.Print(os, 0);
  return os;  
}


}
