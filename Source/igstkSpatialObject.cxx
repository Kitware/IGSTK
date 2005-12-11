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
#include "igstkEvents.h"

namespace igstk
{ 


/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  m_SpatialObject = NULL;
  m_TrackerTool = NULL;

  igstkAddInputMacro( SpatialObjectValidInput );
  igstkAddInputMacro( SpatialObjectNullInput );
  igstkAddInputMacro( ObjectValidInput );
  igstkAddInputMacro( ObjectNullInput );
  igstkAddInputMacro( TrackingEnabledInput );
  igstkAddInputMacro( TrackingLostInput );
  igstkAddInputMacro( TrackingRestoredInput );
  igstkAddInputMacro( TrackingDisabledInput );
  igstkAddInputMacro( ManualTransformInput );
  igstkAddInputMacro( TrackerTransformInput );

  igstkAddStateMacro( InitialState  );
  igstkAddStateMacro( NonTrackedState  );
  igstkAddStateMacro( TrackedState     );
  igstkAddStateMacro( TrackedLostState );
  
  igstkAddTransitionMacro( InitialState, ObjectValidInput, InitialState,  AddObject );
  igstkAddTransitionMacro( InitialState, ObjectValidInput, NonTrackedState,  AddObject );
  igstkAddTransitionMacro( NonTrackedState, ObjectValidInput, NonTrackedState,  AddObject );
  igstkAddTransitionMacro( InitialState, ObjectNullInput, InitialState,  ReportInvalidRequest );

  igstkAddTransitionMacro( InitialState, SpatialObjectValidInput, NonTrackedState,  SetSpatialObject );
  igstkAddTransitionMacro( InitialState, SpatialObjectNullInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, TrackingEnabledInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, TrackingLostInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, TrackingRestoredInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, TrackingDisabledInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, ManualTransformInput, InitialState,  ReportInvalidRequest );
  igstkAddTransitionMacro( InitialState, TrackerTransformInput, InitialState,  ReportInvalidRequest );

  igstkAddTransitionMacro( NonTrackedState, SpatialObjectValidInput, NonTrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTrackedState, SpatialObjectNullInput, NonTrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTrackedState, TrackingEnabledInput, TrackedState,  AttachToTrackerTool );
  igstkAddTransitionMacro( NonTrackedState, TrackingLostInput, NonTrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTrackedState, TrackingRestoredInput, NonTrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTrackedState, TrackingDisabledInput, NonTrackedState,  ReportTrackingDisabled );
  igstkAddTransitionMacro( NonTrackedState, ManualTransformInput, NonTrackedState,  SetTransform );
  igstkAddTransitionMacro( NonTrackedState, TrackerTransformInput, NonTrackedState,  ReportInvalidRequest );

  igstkAddTransitionMacro( TrackedState, SpatialObjectValidInput, TrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedState, SpatialObjectNullInput, TrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedState, TrackingEnabledInput, TrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedState, TrackingLostInput, TrackedLostState,  ReportTrackingLost );
  igstkAddTransitionMacro( TrackedState, TrackingRestoredInput, TrackedState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedState, TrackingDisabledInput, NonTrackedState, ReportTrackingDisabled  );
  igstkAddTransitionMacro( TrackedState, ManualTransformInput, TrackedState, ReportInvalidRequest  );
  igstkAddTransitionMacro( TrackedState, TrackerTransformInput, TrackedState, SetTransform  );

  igstkAddTransitionMacro( TrackedLostState, SpatialObjectValidInput, TrackedLostState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLostState, SpatialObjectNullInput, TrackedLostState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLostState, TrackingEnabledInput, TrackedLostState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLostState, TrackingLostInput, TrackedLostState,  NoAction );
  igstkAddTransitionMacro( TrackedLostState, TrackingRestoredInput, TrackedState,  ReportTrackingRestored );
  igstkAddTransitionMacro( TrackedLostState, TrackingDisabledInput, NonTrackedState,  ReportTrackingDisabled );
  igstkAddTransitionMacro( TrackedLostState, ManualTransformInput, TrackedLostState,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLostState, TrackerTransformInput, TrackedLostState,  ReportInvalidRequest );

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
  
 
/** Null operation for a State Machine transition */
void SpatialObject::NoAction()
{
}


/** Set the ITK spatial object that provide internal functionalities. This
 * method should only be called from the StateMachine */
void SpatialObject::SetSpatialObject()
{
  m_SpatialObject = m_SpatialObjectToBeSet;
}

/** Request setting the ITK spatial object that provide internal functionalities. */
void SpatialObject::RequestAddObject(Self * object )
{
  m_ObjectToBeAdded = object;

  if( m_ObjectToBeAdded.IsNull() )
    {
    m_StateMachine.PushInput( m_ObjectNullInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_StateMachine.PushInput( m_ObjectValidInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Add an Object */
void SpatialObject::AddObject()
{
  m_SpatialObject->AddSpatialObject(m_ObjectToBeAdded->GetSpatialObject());
  m_InternalObjectList.push_back(m_ObjectToBeAdded);
}

/** Return a child object given the id */
const SpatialObject::Self * SpatialObject::GetObject(unsigned long id) const
{
  if(id > m_InternalObjectList.size())
    {
    return NULL;
    }

  return m_InternalObjectList[id];
}


/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * SpatialObject::GetSpatialObject()
{
  return m_SpatialObject;
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
    this->RequestSetTrackedTransform( transformEvent->Get() );
    }
}

} // end namespace igstk

