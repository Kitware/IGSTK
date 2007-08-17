/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  m_Parent        = NULL;

  igstkAddInputMacro( InternalSpatialObjectNull );
  igstkAddInputMacro( InternalSpatialObjectValid );
  igstkAddInputMacro( SpatialObjectParentNull );
  igstkAddInputMacro( SpatialObjectParentValid );
  igstkAddInputMacro( TrackerToolNull );
  igstkAddInputMacro( TrackerToolValid );
  igstkAddInputMacro( TransformToSpatialObjectParent );
  igstkAddInputMacro( CalibrationTransformToTrackerTool );
  igstkAddInputMacro( GetTransformToWorld );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( InternalSpatialObjectValidSet );
  igstkAddStateMacro( AttachedToTrackerTool );
  igstkAddStateMacro( AttachedToSpatialObjectParent );
  igstkAddStateMacro( AttachedToTrackerToolAndCalibrated );
  igstkAddStateMacro( AttachedToSpatialObjectParentAndLocated );
  
  igstkAddTransitionMacro( Initial, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  SetInternalSpatialObject );
  igstkAddTransitionMacro( Initial, SpatialObjectParentNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, SpatialObjectParentValid, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackerToolNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackerToolValid, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TransformToSpatialObjectParent, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, CalibrationTransformToTrackerTool, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, GetTransformToWorld, Initial,  ReportInvalidRequest );

  igstkAddTransitionMacro( InternalSpatialObjectValidSet, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, SpatialObjectParentNull, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, SpatialObjectParentValid, AttachedToSpatialObjectParent,  AttachToSpatialObjectParent );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TrackerToolNull, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TrackerToolValid, AttachedToTrackerTool,  AttachToTrackerTool );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TransformToSpatialObjectParent, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, CalibrationTransformToTrackerTool, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, GetTransformToWorld, InternalSpatialObjectValidSet,  ReportInvalidRequest );

  igstkAddTransitionMacro( AttachedToTrackerTool, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, SpatialObjectParentNull, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, SpatialObjectParentValid, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TrackerToolNull, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TrackerToolValid, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TransformToSpatialObjectParent, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, CalibrationTransformToTrackerTool, AttachedToTrackerToolAndCalibrated,  SetCalibrationTransformToTrackerTool );
  igstkAddTransitionMacro( AttachedToTrackerTool, GetTransformToWorld, AttachedToTrackerTool,  ReportInvalidRequest );

  igstkAddTransitionMacro( AttachedToSpatialObjectParent, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, SpatialObjectParentNull, AttachedToSpatialObjectParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, SpatialObjectParentValid, AttachedToSpatialObjectParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, TrackerToolNull, AttachedToSpatialObjectParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, TrackerToolValid, AttachedToSpatialObjectParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, TransformToSpatialObjectParent, AttachedToSpatialObjectParentAndLocated,  SetTransformToSpatialObjectParent );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, CalibrationTransformToTrackerTool, AttachedToSpatialObjectParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParent, GetTransformToWorld, AttachedToSpatialObjectParent,  ReportInvalidRequest );

  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, SpatialObjectParentNull, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, SpatialObjectParentValid, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, TrackerToolNull, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, TrackerToolValid, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, TransformToSpatialObjectParent, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, CalibrationTransformToTrackerTool, AttachedToTrackerToolAndCalibrated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerToolAndCalibrated, GetTransformToWorld, AttachedToTrackerToolAndCalibrated,  BroadcastTransformToWorld );

  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, SpatialObjectParentNull, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, SpatialObjectParentValid, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, TrackerToolNull, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, TrackerToolValid, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, TransformToSpatialObjectParent, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, CalibrationTransformToTrackerTool, AttachedToSpatialObjectParentAndLocated,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToSpatialObjectParentAndLocated, GetTransformToWorld, AttachedToSpatialObjectParentAndLocated,  BroadcastTransformToWorld );

  igstkSetInitialStateMacro( Initial );

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

  if( this->m_SpatialObject )
    {
    os << indent << "Internal Spatial Object = ";
    os << this->m_SpatialObject.GetPointer() << std::endl;
    }
  os << indent << this->m_TransformToSpatialObjectParent << std::endl;
  if( this->m_Parent )
    {
    os << indent << "Spatial Object Parent = ";
    os << indent << m_Parent.GetPointer() << std::endl;
    }

}

/** Request setting the ITK spatial object that provide internal 
 *  functionalities. */
void SpatialObject
::RequestSetInternalSpatialObject( SpatialObjectType * spatialObject )
{  
  m_SpatialObjectToBeSet = spatialObject;

  if( m_SpatialObjectToBeSet.IsNull() )
    {
    igstkPushInputMacro( InternalSpatialObjectNull );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    igstkPushInputMacro( InternalSpatialObjectValid );
    m_StateMachine.ProcessInputs();
    }
}
  
 
/** Null operation for a State Machine transition */
void SpatialObject::NoProcessing()
{
}


/** Set the ITK spatial object that provide internal functionalities.
 * This method should only be called from the StateMachine */
void SpatialObject::SetInternalSpatialObjectProcessing()
{
  m_SpatialObject = m_SpatialObjectToBeSet;
}

/** Request adding this current IGSTK spatial object as child of the
 * Spatial Object passed as argument. */
void SpatialObject::RequestAttachToSpatialObjectParent(Self * object )
{
  m_ParentToBeSet = object;

  if( m_ParentToBeSet.IsNull() || 
      m_ParentToBeSet->GetInternalSpatialObject() == NULL)
    {
    igstkPushInputMacro( SpatialObjectParentNull );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    igstkPushInputMacro( SpatialObjectParentValid );
    m_StateMachine.ProcessInputs();
    }
}

/** Attach this object to a parent. */
void SpatialObject::AttachToSpatialObjectParentProcessing()
{
  m_Parent = m_ParentToBeSet;
  m_Parent->GetInternalSpatialObject()->AddSpatialObject( 
    this->GetInternalSpatialObject() );
}


/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * 
SpatialObject::GetInternalSpatialObject()
{
  return m_SpatialObject;
}

/** Set the Transform to the Parent by a direct call. The state machine
 * will take care of only accepting this transform if the object is not
 * currently being tracked by a TrackerTool. */
void SpatialObject
::RequestSetTransformToSpatialObjectParent(const Transform & transform )
{
  m_TransformToSpatialObjectParentToBeSet = transform;
  igstkPushInputMacro( TransformToSpatialObjectParent );
  m_StateMachine.ProcessInputs();
}

/** Set the Calibration Transform relative to the Tracker Tool to which
 * this spatial object is presumably attached. This call should only be
 * invoked once. Subsequent calls will be ignored by the State Machine. */
void SpatialObject
::RequestSetCalibrationTransformToTrackerTool(const Transform & transform )
{
  m_TransformToSpatialObjectParentToBeSet = transform;
  igstkPushInputMacro( CalibrationTransformToTrackerTool );
  m_StateMachine.ProcessInputs();
}


/** Set the Transform relative to the spatial object parent. Only to be
 * called by the State Machine and only to be called once. */
void SpatialObject::SetTransformToSpatialObjectParentProcessing()
{
  m_TransformToSpatialObjectParent = m_TransformToSpatialObjectParentToBeSet;
    
  Transform::VectorType translation = 
    m_TransformToSpatialObjectParent.GetTranslation();
  m_SpatialObject->GetObjectToParentTransform()->SetOffset( translation );

  Transform::VersorType rotation = 
    m_TransformToSpatialObjectParent.GetRotation();
  Transform::VersorType::MatrixType matrix = rotation.GetMatrix();
  m_SpatialObject->GetObjectToParentTransform()->SetMatrix( matrix );

  igstkLogMacro( DEBUG, " SpatialObject::SetTransform() T: " 
      << translation << " R: " << rotation << "\n" );
}

/** Set the calibration transform that relates this Spatial Object to the 
 *  Tracker tool to which it is attached */
void SpatialObject::SetCalibrationTransformToTrackerToolProcessing()
{
  // Delegate to the method that sets the transform to parent.  In this case
  // the parent simply happens to be the coordinate reference system of the
  // TrackerTool.
  this->SetTransformToSpatialObjectParentProcessing();
}


/** Request Get Transform */
void SpatialObject::RequestGetTransformToWorld()
{
  igstkPushInputMacro( GetTransformToWorld );
  m_StateMachine.ProcessInputs();
}


/** Broadcast Transform */
void SpatialObject::BroadcastTransformToWorldProcessing()
{
  std::cout << "SpatialObject::BroadcastTransformToWorldProcessing " << std::endl;

  this->ComputeTransformToWorld();

  TransformModifiedEvent event;
  event.Set( this->ComputeTransformToWorld() );
  this->InvokeEvent( event );
  //
  //      VERY IMPORTANT METHOD: TEST CAREFULLY !!!
  //
}


/** Compute the transfrom from the current object to the World Coordinate
 * system by recursively composing transforms from parents. At the end of
 * the execution of this method the new transform to world is stored in the
 * cache mutable member variable m_TransformToWorld. */
const Transform & SpatialObject::ComputeTransformToWorld() const
{
  //
  //      VERY IMPORTANT METHOD: TEST CAREFULLY !!!
  //
  this->m_TransformToWorld.TransformCompose( 
    this->m_Parent->ComputeTransformToWorld(),
    this->m_TransformToSpatialObjectParent );
 
  return this->m_TransformToWorld;
}

/** Request to attach this spatial object to a tracker tool. If the
 * operation succeeds then the coordinate system of the Tracker tool
 * will become the parent of the current spatial object.  This is a
 * one-time operation. Once a Spatial Object is attached to a tracker
 * tool it is not expected to get back to manual nor to be re-attached
 * to a second tracker tool. */
void SpatialObject::RequestAttachToTrackerTool( 
  Self * trackerToolCoordinateReferenceSystem )
{
  m_ParentToBeSet = trackerToolCoordinateReferenceSystem;

  if( m_ParentToBeSet.IsNull() || 
      m_ParentToBeSet->GetInternalSpatialObject() == NULL)
    {
    igstkPushInputMacro( TrackerToolNull );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    igstkPushInputMacro( TrackerToolValid );
    m_StateMachine.ProcessInputs();
    }

  m_StateMachine.ProcessInputs();
}


/** Attach this object to the coordinate reference system of a tracker tool. */
void SpatialObject::AttachToTrackerToolProcessing()
{
  m_Parent = m_ParentToBeSet;
  m_Parent->GetInternalSpatialObject()->AddSpatialObject( 
    this->GetInternalSpatialObject() );
}


/** Report that an invalid or suspicious operation has been requested.
 * This may mean that an error condition has arised in one of the
 * componenta that interact with this SpatialObject. */
void
SpatialObject::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

} // end namespace igstk
