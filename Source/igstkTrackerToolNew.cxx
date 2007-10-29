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
  // Initialize the variables
  m_RawTransform.SetToIdentity( 1e300 );  // FIXME: DEPRECATED: This is now the Transform to parent between the m_CoordinateReferenceSystem of this tool and the m_CoordinateReferenceSystem of the Tracker.
  m_ToolCalibrationTransform.SetToIdentity( 1e300 );  // FIXME : DEPRECATED: This is now the Transfrom To Parent of the spatial object attached to this tool.
  m_Updated = false; // not yet updated

  m_CoordinateReferenceSystem = CoordinateReferenceSystemType::New();

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitializeTrackerTool );
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( AttemptingToAttachTrackerToolToTracker );
  igstkAddStateMacro( Attached );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Tracked );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  // Set the input descriptors
  igstkAddInputMacro( InitializeTool );
  igstkAddInputMacro( ToolInitializationSuccess ); 
  igstkAddInputMacro( ToolInitializationFailure ); 
  igstkAddInputMacro( AttachToolToTracker ); 
  igstkAddInputMacro( AttachmentToTrackerSuccess ); 
  igstkAddInputMacro( AttachmentToTrackerFailure ); 


  m_StateMachine.SetReadyToRun();

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           InitializeTool,
                           AttemptingToInitializeTrackerTool,
                           AttemptToInitialize );

  // Transitions from the AttemptingToInitialize
  igstkAddTransitionMacro( AttemptingToInitializeTrackerTool,
                           ToolInitializationSuccess,
                           Initialized,
                           TrackerToolInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeTrackerTool,
                           ToolInitializationFailure,
                           Idle,
                           TrackerToolInitializationFailure );

  // Transition from Initialized state
  igstkAddTransitionMacro( Initialized,
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
                           Initialized,
                           TrackerToolAttachmentToTrackerFailure );

 
}

TrackerToolNew::~TrackerToolNew(void)
{
}

void 
TrackerToolNew::RequestInitialize( )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::RequestInitialize called...\n");
  igstkPushInputMacro( InitializeTool );
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
TrackerToolNew::SetTrackerToolIdentifier( std::string identifier )
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::SetTrackerToolIdentifier called...\n");
  m_TrackerToolIdentifier = identifier;
}

/** The "AttemptToInitializeProcessing" method attempts to initialize the tracker tool */
void TrackerToolNew::AttemptToInitializeProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToInitializeProcessing called ...\n");

  bool  result = this->GetTrackerToolInitialized();
  
  m_StateMachine.PushInputBoolean( result,
                                   m_ToolInitializationSuccessInput,
                                   m_ToolInitializationFailureInput );
}

/** The "AttemptToAttachTrackerToolToTracker" method attempts to attach the tracker tool
 * to the tracker */
void TrackerToolNew::AttemptToAttachTrackerToolToTrackerProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToAttachTrackerToolToTracker called ...\n");

  // FIXME: Add code to verify if the the tool is attached successfully or not
  m_Tracker->RequestAddTool( m_TrackerToolIdentifier, this );
  bool result = true;
  m_StateMachine.PushInputBoolean( result,
                                   m_AttachmentToTrackerSuccessInput,
                                   m_AttachmentToTrackerFailureInput );
}

/** The "GetTrackerToolInitialized" methods returns a boolean indicating
 * if the tracker tool is initialized or not. This method is to be overriden in
 * the dervied classes
 */
bool  TrackerToolNew::GetTrackerToolInitialized()
{
  igstkLogMacro( DEBUG, "igstk::TrackerToolNew::GetTrackerToolInitialized called ...\n");
  return true;
}

/** Post-processing after a successful initialization attempt . */ 
void TrackerToolNew::TrackerToolInitializationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolInitializationSuccessProcessing called ...\n");


  // FIXME: convert this std::cout to an event 
  std::cout << "TrackerToolNew Initialized succesfully: " << std::endl;
}


/** Post-processing after a failed initialization attempt . */ 
void TrackerToolNew::TrackerToolInitializationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolInitializationFailureProcessing called ...\n");


  // FIXME: convert this std::cout to an event 
  std::cerr << "TrackerTool Initialization failed: Make sure to establish the required tool parameters " << std::endl;
}

/** Post-processing after a successful tracker tool to tracker attachment attempt . */ 
void TrackerToolNew::TrackerToolAttachmentToTrackerSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolAttachmentToTrackerSuccessiProcessing called ...\n");


  // FIXME: convert this std::cout to an event 
  std::cout << "TrackerToolNew attached to the tracker succesfully: " << std::endl;
}

/** Post-processing after a failed attachment attempt . */ 
void TrackerToolNew::TrackerToolAttachmentToTrackerFailureProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::TrackerToolInitializationFailureProcessing called ...\n");


  // FIXME: convert this std::cout to an event 
  std::cerr << "TrackerTool to tracker attachment attempt failed " << std::endl;
}

/** This method should only be available to the Tracker */
void 
TrackerToolNew::RequestSetTransform( const TransformType & transform )
{
  m_Transform = transform;

  TransformModifiedEvent event;
  event.Set( transform );
  this->InvokeEvent( event );
}

/** Attach and spatial object to be tracked.
 *  FIXME: Add a state machine to this class and pass first this 
 *         request through the transition matrix. Then invoke a
 *         AttachSpatialObjectProcessing() method.
 */
void
TrackerToolNew::RequestAttachSpatialObject( SpatialObject * spatialObject )
{
  // Note: the ToolCalibrationTransform should be the transform relating
  //       the spatial object to the tracker tool. FIXME.
  spatialObject->RequestAttachToTrackerTool( this->m_CoordinateReferenceSystem );
}

/** Print object information */
void TrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform: " << this->m_Transform << std::endl;
  os << indent << "RawTransform: " << this->m_RawTransform << std::endl;
  os << indent << "ToolCalibrationTransform: "
               << this->m_ToolCalibrationTransform << std::endl;
}


std::ostream& operator<<(std::ostream& os, const TrackerToolNew& o)
{
  o.Print(os, 0);
  return os;  
}


}
