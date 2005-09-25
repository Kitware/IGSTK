/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSpatialObject.h"
#include "igstkTransformModifiedEvent.h"

namespace igstk
{ 


/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  m_SpatialObject = NULL;
  m_TrackerTool = NULL;

  m_StateMachine.AddInput( m_SpatialObjectValidInput,  "SpatialObjectValidInput" );
  m_StateMachine.AddInput( m_SpatialObjectNullInput,  "SpatialObjectNullInput" );
  m_StateMachine.AddInput( m_TrackingEnabledInput,  "TrackingEnabledInput" );
  m_StateMachine.AddInput( m_TrackingLostInput,     "TrackingLostInput" );
  m_StateMachine.AddInput( m_TrackingRestoredInput, "TrackingRestoredInput" );
  m_StateMachine.AddInput( m_TrackingDisabledInput, "TrackingDisabledInput" );
  m_StateMachine.AddInput( m_ManualTransformInput, "ManualTransformInput" );
  m_StateMachine.AddInput( m_TrackerTransformInput, "TrackerTransformInput" );

  m_StateMachine.AddState( m_InitialState,  "InitialState"  );
  m_StateMachine.AddState( m_NonTrackedState,  "NonTrackedState"  );
  m_StateMachine.AddState( m_TrackedState,     "TrackedState"     );
  m_StateMachine.AddState( m_TrackedLostState, "TrackedLostState" );
  
  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_InitialState, m_SpatialObjectValidInput, m_NonTrackedState,  & SpatialObject::SetSpatialObject );
  m_StateMachine.AddTransition( m_InitialState, m_SpatialObjectNullInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_TrackingEnabledInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_TrackingLostInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_TrackingRestoredInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_TrackingDisabledInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_ManualTransformInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_InitialState, m_TrackerTransformInput, m_InitialState,  & SpatialObject::ReportInvalidRequest );

  m_StateMachine.AddTransition( m_NonTrackedState, m_SpatialObjectValidInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_SpatialObjectNullInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingEnabledInput, m_TrackedState,  & SpatialObject::AttachToTrackerTool );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingLostInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingRestoredInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingDisabledInput, m_NonTrackedState,  & SpatialObject::ReportTrackingDisabled );
  m_StateMachine.AddTransition( m_NonTrackedState, m_ManualTransformInput, m_NonTrackedState,  & SpatialObject::SetTransform );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackerTransformInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );

  m_StateMachine.AddTransition( m_TrackedState, m_SpatialObjectValidInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_SpatialObjectNullInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingEnabledInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingLostInput, m_TrackedLostState,  & SpatialObject::ReportTrackingLost );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingRestoredInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingDisabledInput, m_NonTrackedState, & SpatialObject::ReportTrackingDisabled  );
  m_StateMachine.AddTransition( m_TrackedState, m_ManualTransformInput, m_TrackedState, & SpatialObject::ReportInvalidRequest  );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackerTransformInput, m_TrackedState, & SpatialObject::SetTransform  );

  m_StateMachine.AddTransition( m_TrackedLostState, m_SpatialObjectValidInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_SpatialObjectNullInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingEnabledInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingLostInput, m_TrackedLostState,  NoAction );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingRestoredInput, m_TrackedState,  & SpatialObject::ReportTrackingRestored );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingDisabledInput, m_NonTrackedState,  & SpatialObject::ReportTrackingDisabled );
  m_StateMachine.AddTransition( m_TrackedLostState, m_ManualTransformInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackerTransformInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );

  m_StateMachine.SelectInitialState( m_InitialState );

  m_StateMachine.SetReadyToRun();


  m_TrackerToolObserver = CommandType::New();
  m_TrackerToolObserver->SetCallbackFunction( this, &SpatialObject::TransformUpdateFromTrackerTool );
} 

/** Destructor */
SpatialObject::~SpatialObject()  
{
}


/** Print Self function */
void SpatialObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_SpatialObject )
    {
    os << indent << this->m_SpatialObject << std::endl;
    }
  os << indent << this->m_Transform << std::endl;
  if( this->m_TrackerTool )
    {
    os << indent << m_TrackerTool << std::endl;
    }

}

/** Request setting the ITK spatial object that provide internal functionalities. */
void SpatialObject::RequestSetSpatialObject( SpatialObjectType * spatialObject )
{
  
  m_SpatialObjectToBeSet = spatialObject;

  if( m_SpatialObjectToBeSet.IsNull() )
    {
    m_StateMachine.PushInput( m_SpatialObjectNullInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_StateMachine.PushInput( m_SpatialObjectValidInput );
    m_StateMachine.ProcessInputs();
    }
}
  
 
/** Set the ITK spatial object that provide internal functionalities. This
 * method should only be called from the StateMachine */
void SpatialObject::SetSpatialObject()
{
  m_SpatialObject = m_SpatialObjectToBeSet;
}


/** Set the full Transform by a direct call. The state machine will take care
 * of only accepting this transform if the object is not currently being
 * tracked by a TrackerTool. */
void SpatialObject::RequestSetTransform(const Transform & transform )
{
  m_TransformToBeSet = transform;
  m_StateMachine.PushInput( m_ManualTransformInput );
  m_StateMachine.ProcessInputs();
}

/** Set the full Transform from a tracker. This call should only be invoked
 *  from the Callback of the observer that is listening to events from a 
 *  tracker.*/
void SpatialObject::RequestSetTrackedTransform(const Transform & transform )
{
  m_TransformToBeSet = transform;
  m_StateMachine.PushInput( m_TrackerTransformInput );
  m_StateMachine.ProcessInputs();
}


/** Set the full Transform. Only to be called by the State Machine. */
void SpatialObject::SetTransform()
{
  m_Transform = m_TransformToBeSet;
  if( m_SpatialObject.IsNotNull() )
    {
    Transform::VectorType translation = m_Transform.GetTranslation();
    m_SpatialObject->GetObjectToWorldTransform()->SetOffset( translation );
    Transform::VersorType rotation = m_Transform.GetRotation();
    Transform::VersorType::MatrixType matrix = rotation.GetMatrix();
    m_SpatialObject->GetObjectToWorldTransform()->SetMatrix( matrix );
    igstkLogMacro( DEBUG, " SpatialObject::SetTransform() T: " 
        << translation << " R: " << rotation << "\n" );
    }
}


/** Get Transform */
const Transform &
SpatialObject::GetTransform()  const
{
  return m_Transform;
}

/** Request the protocol for attaching to a tracker tool. This is a one-time
 * operation. Once a Spatial Object is attached to a tracker tool it is not
 * expected to get back to manual nor to be re-attached to a second tracker
 * tool. */
void SpatialObject::RequestAttachToTrackerTool(const TrackerTool * tool )
{
  m_TrackerToolToAttachTo = tool;
  m_StateMachine.PushInput( m_TrackingEnabledInput );
  m_StateMachine.ProcessInputs();
}

/** Make necessary connections to the TrackerTool */
void
SpatialObject::AttachToTrackerTool()
{
   m_TrackerTool = m_TrackerToolToAttachTo;
   m_TrackerTool->AddObserver( TransformModifiedEvent(), m_TrackerToolObserver ); 
}

/** Report that tracking is now enabled */
void
SpatialObject::ReportTrackingDisabled()
{
}

/** Report that tracking has been lost */
void
SpatialObject::ReportTrackingLost()
{
}

/** Report that tracking has been restored */
void
SpatialObject::ReportTrackingRestored()
{
}

/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this SpatialObject. */
void
SpatialObject::ReportInvalidRequest()
{
}

/** Receive the Events from the Tracker Tool and use them for updating the
 * transform of this Spatial Object */
void
SpatialObject::TransformUpdateFromTrackerTool( const ::itk::EventObject & event  )
{
  const TransformModifiedEvent * transformEvent = 
    dynamic_cast< const TransformModifiedEvent * >( &event );

  if( transformEvent )
    {
    this->RequestSetTrackedTransform( transformEvent->GetTransform() );
    }
}

} // end namespace igstk

