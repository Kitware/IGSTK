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

  m_StateMachine.AddInput( m_SpatialObjectValidInput,  "SpatialObjectValidInput" );
  m_StateMachine.AddInput( m_SpatialObjectNullInput,  "SpatialObjectNullInput" );
  m_StateMachine.AddInput( m_TrackingEnabledInput,  "TrackingEnabledInput" );
  m_StateMachine.AddInput( m_TrackingLostInput,     "TrackingLostInput" );
  m_StateMachine.AddInput( m_TrackingRestoredInput, "TrackingRestoredInput" );
  m_StateMachine.AddInput( m_TrackingDisabledInput, "TrackingDisabledInput" );
  m_StateMachine.AddInput( m_ManualTransformInput, "ManualTransformInput" );

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

  m_StateMachine.AddTransition( m_NonTrackedState, m_SpatialObjectValidInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_SpatialObjectNullInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingEnabledInput, m_TrackedState,  & SpatialObject::AttachToTrackerTool );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingLostInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingRestoredInput, m_NonTrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_NonTrackedState, m_TrackingDisabledInput, m_NonTrackedState,  & SpatialObject::ReportTrackingDisabled );
  m_StateMachine.AddTransition( m_NonTrackedState, m_ManualTransformInput, m_NonTrackedState,  & SpatialObject::SetTransform );

  m_StateMachine.AddTransition( m_TrackedState, m_SpatialObjectValidInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_SpatialObjectNullInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingEnabledInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingLostInput, m_TrackedLostState,  & SpatialObject::ReportTrackingLost );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingRestoredInput, m_TrackedState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedState, m_TrackingDisabledInput, m_NonTrackedState, & SpatialObject::ReportTrackingDisabled  );
  m_StateMachine.AddTransition( m_TrackedState, m_ManualTransformInput, m_TrackedState, & SpatialObject::ReportInvalidRequest  );

  m_StateMachine.AddTransition( m_TrackedLostState, m_SpatialObjectValidInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_SpatialObjectNullInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingEnabledInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingLostInput, m_TrackedLostState,  NoAction );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingRestoredInput, m_TrackedState,  & SpatialObject::ReportTrackingRestored );
  m_StateMachine.AddTransition( m_TrackedLostState, m_TrackingDisabledInput, m_NonTrackedState,  & SpatialObject::ReportTrackingDisabled );
  m_StateMachine.AddTransition( m_TrackedLostState, m_ManualTransformInput, m_TrackedLostState,  & SpatialObject::ReportInvalidRequest );

  m_StateMachine.SelectInitialState( m_InitialState );

  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
SpatialObject::~SpatialObject()  
{
}


/** Print Self function */
void SpatialObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Request setting the ITK spatial object that provide internal functionalities. */
void SpatialObject::RequestSetSpatialObject( SpatialObjectType * spatialObject )
{
  std::cout << "SpatialObject::RequestSetSpatialObject() " << std::endl;
  m_SpatialObjectToBeSet = spatialObject;
  if( m_SpatialObjectToBeSet.IsNull() )
    {
    m_StateMachine.ProcessInput( m_SpatialObjectNullInput );
    }
  else 
    {
    m_StateMachine.ProcessInput( m_SpatialObjectValidInput );
    }
}
  
 
/** Set the ITK spatial object that provide internal functionalities. This
 * method should only be called from the StateMachine */
void SpatialObject::SetSpatialObject()
{
  std::cout << "SpatialObject::SetSpatialObject()" << std::endl;
  m_SpatialObject = m_SpatialObjectToBeSet;
}


/** Set the full Transform by a direct call. The state machine will take care
 * of only accepting this transform if the object is not currently being
 * tracked by a TrackerTool. */
void SpatialObject::RequestSetTransform(const Transform & transform )
{
  std::cout << "SpatialObject::RequestSetTransform" << std::endl;
  std::cout << " translation = " << transform.GetTranslation() << std::endl;
  m_TransformToBeSet = transform;
  std::cout << " assigned = " << m_TransformToBeSet.GetTranslation() << std::endl;
  m_StateMachine.ProcessInput( m_ManualTransformInput );
  std::cout << "after processing the m_ManualTransformInput" << std::endl;
}

/** Set the full Transform. Only to be called by the State Machine. */
void SpatialObject::SetTransform()
{
  std::cout << " Setting transform to " << m_TransformToBeSet.GetTranslation() << std::endl;
  m_Transform = m_TransformToBeSet;
  if( m_SpatialObject.IsNotNull() )
    {
    Transform::VectorType translation = m_Transform.GetTranslation();
    std::cout << "translation = " << translation << std::endl;
    m_SpatialObject->GetObjectToWorldTransform()->SetOffset( translation );
    Transform::VersorType rotation = m_Transform.GetRotation();
    std::cout << "rotation = " << rotation << std::endl;
    Transform::VersorType::MatrixType matrix = rotation.GetMatrix();
    m_SpatialObject->GetObjectToWorldTransform()->SetMatrix( matrix );
    std::cout << "Before the event " << std::endl;
    this->InvokeEvent( PositionModifiedEvent() );
    std::cout << "After the event " << std::endl;
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
  m_StateMachine.ProcessInput( m_TrackingEnabledInput );
}

/** Make necessary connections to the TrackerTool */
void
SpatialObject::AttachToTrackerTool()
{
   m_TrackerTool = m_TrackerToolToAttachTo;
   // m_TrackerTool->AddObserver();
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



} // end namespace igstk

