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
  igstkAddStateMacro( Attached );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Tracked );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  // Set the input descriptors
  igstkAddInputMacro( InitializeTool );
  igstkAddInputMacro( Success); 
  igstkAddInputMacro( Failure); 


  m_StateMachine.SetReadyToRun();

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           InitializeTool,
                           AttemptingToInitializeTrackerTool,
                           AttemptToInitialize );

  // Transitions from the AttemptingToInitialize
  igstkAddTransitionMacro( AttemptingToInitializeTrackerTool,
                           Success,
                           Initialized,
                           TrackerToolInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeTrackerTool,
                           Failure,
                           Idle,
                           TrackerToolInitializationFailure );
 
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

/** The "AttemptToInitialize" method attempts to initialize the tracker to    ol */
void TrackerToolNew::AttemptToInitializeProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerToolNew::AttemptToInitializeProcessing called ...\n");

  bool  result = this->GetTrackerToolInitialized();
  
  m_StateMachine.PushInputBoolean( result,
                                   m_SuccessInput,
                                   m_FailureInput );
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
