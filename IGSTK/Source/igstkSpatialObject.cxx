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

#ifdef USE_SPATIAL_OBJECT_DEPRECATED 

/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  m_SpatialObject = NULL;
  m_Parent        = NULL;

  igstkAddInputMacro( InternalSpatialObjectNull );
  igstkAddInputMacro( InternalSpatialObjectValid );
  igstkAddInputMacro( TrackerToolNull );
  igstkAddInputMacro( TrackerToolValid );
  igstkAddInputMacro( TransformAndParent );
  // igstkAddInputMacro( GetTransformToWorld );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( InternalSpatialObjectValidSet );
  igstkAddStateMacro( AttachedToParent );
  igstkAddStateMacro( AttachedToTrackerTool );
  
  igstkAddTransitionMacro( Initial, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  SetInternalSpatialObject );
  igstkAddTransitionMacro( Initial, TrackerToolNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackerToolValid, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TransformAndParent, Initial,  ReportInvalidRequest );
  // igstkAddTransitionMacro( Initial, GetTransformToWorld, Initial,  BroadcastInvalidTransformMessage );

  igstkAddTransitionMacro( InternalSpatialObjectValidSet, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TrackerToolNull, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TrackerToolValid, AttachedToTrackerTool,  AttachToTrackerTool );
  igstkAddTransitionMacro( InternalSpatialObjectValidSet, TransformAndParent, AttachedToParent,  ReportInvalidRequest );
  // igstkAddTransitionMacro( InternalSpatialObjectValidSet, GetTransformToWorld, InternalSpatialObjectValidSet,  BroadcastInvalidTransformMessage );

  igstkAddTransitionMacro( AttachedToTrackerTool, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TrackerToolNull, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TrackerToolValid, AttachedToTrackerTool,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToTrackerTool, TransformAndParent, AttachedToTrackerTool,  ReportInvalidRequest );
  // igstkAddTransitionMacro( AttachedToTrackerTool, GetTransformToWorld, AttachedToTrackerTool,  BroadcastInvalidTransformMessage );

  igstkAddTransitionMacro( AttachedToParent, InternalSpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToParent, InternalSpatialObjectValid, InternalSpatialObjectValidSet,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToParent, TrackerToolNull, AttachedToParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToParent, TrackerToolValid, AttachedToParent,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttachedToParent, TransformAndParent, AttachedToParent,  SetTransformToSpatialObjectParent );
  // igstkAddTransitionMacro( AttachedToParent, GetTransformToWorld, AttachedToParent,  BroadcastInvalidTransformMessage );

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
  os << indent << "Transform To Parent = ";
  os << indent << this->m_TransformToSpatialObjectParent << std::endl;

  // os << indent << "Transform To World = ";
  // os << indent << this->m_TransformToWorld << std::endl;

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
/* FIXCS
void SpatialObject::RequestAttachToSpatialObjectParent(Self * object )
{
  m_ParentToBeSet    = object;
  m_TransformToParentToBeSet = transform;

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
*/

/** Attach this object to a parent. */
/* FIXCS
void SpatialObject::AttachToSpatialObjectParentProcessing()
{
  m_Parent = m_ParentToBeSet;
  m_Parent->GetInternalSpatialObject()->AddSpatialObject( 
    this->GetInternalSpatialObject() );
}
*/ 

/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * 
SpatialObject::GetInternalSpatialObject() const
{
  return m_SpatialObject;
}

/** Set the Transform to the Parent by a direct call. The state machine
 * will take care of only accepting this transform if the object is not
 * currently being tracked by a TrackerTool. */
// IMPLEMENTATION MOVED TO THE .h file because this method is templated.
// void SpatialObject
//::RequestSetTransform(const Transform & transform, const Self * parent )
//{
  // FIXCS m_ParentToBeSet = parent;
//  m_TransformToSpatialObjectParentToBeSet = transform;

  /* FIXCS
  if( parent.IsNotNull() )
    {
    igstkPushInputMacro( TransformAndParent );
    m_StateMachine.ProcessInputs();
    }
  */
// }


/** Set the Transform relative to the spatial object parent. Only to be
 * called by the State Machine and only to be called once. */
void SpatialObject::SetTransformToSpatialObjectParentProcessing()
{
  m_TransformToSpatialObjectParent = m_TransformToSpatialObjectParentToBeSet;

  /* FIXCS
  this->m_CoordinateReferenceSystem->SetTransform( 
    this->m_TransformToSpatialObjectParent,
    m_Parent->m_CoordinateReferenceSystem );
   */

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
/* 
FIXCS
void SpatialObject::SetCalibrationTransformToTrackerToolProcessing()
{
  // Delegate to the method that sets the transform to parent.  In this case
  // the parent simply happens to be the coordinate reference system of the
  // TrackerTool.
  this->SetTransformToSpatialObjectParentProcessing();
}
*/

/** Request Get Transform */
/*
void SpatialObject::RequestGetTransformToWorld()
{
  igstkPushInputMacro( GetTransformToWorld );
  m_StateMachine.ProcessInputs();
}
*/

/** Broadcast Transform */
/* FIXCS - Deprecate
void SpatialObject::BroadcastTransformToWorldProcessing()
{
  TransformModifiedEvent event;
  event.Set( this->ComputeTransformToWorld() );
  this->InvokeEvent( event );
}
*/ 


/** Broadcast Transform */
void SpatialObject::BroadcastInvalidTransformMessageProcessing()
{
  TransformNotAvailableEvent event;
  this->InvokeEvent( event );
}


/** Compute the transfrom from the current object to the World Coordinate
 * system by recursively composing transforms from parents. At the end of
 * the execution of this method the new transform to world is stored in the
 * cache mutable member variable m_TransformToWorld. */
#if 0
const Transform & SpatialObject::ComputeTransformToWorld() const
{
  // FIXCS
  // this->m_TransformToWorld = 
  //   this->m_CoordinateReferenceSystem->ComputeTransformToWorld();

  Transform::VectorType translation = 
    this->m_TransformToWorld.GetTranslation();
  this->m_SpatialObject->GetObjectToWorldTransform()->SetOffset( translation );

  Transform::VersorType rotation = 
    this->m_TransformToWorld.GetRotation();
  Transform::VersorType::MatrixType matrix = rotation.GetMatrix();
  this->m_SpatialObject->GetObjectToWorldTransform()->SetMatrix( matrix );

  igstkLogMacro( DEBUG, " SpatialObject::ComputeTransformToWorld() T: " 
      << translation << " R: " << rotation << "\n" );

  return this->m_TransformToWorld;
}
#endif 

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
    return;
    }
  else 
    {
    igstkPushInputMacro( TrackerToolValid );
    m_StateMachine.ProcessInputs();
    return;
    }

//  m_StateMachine.ProcessInputs();
}


/** Attach this object to the coordinate reference system of a tracker tool. */
void SpatialObject::AttachToTrackerToolProcessing()
{
  m_Parent = m_ParentToBeSet;
  m_Parent->GetInternalSpatialObject()->AddSpatialObject( 
    this->GetInternalSpatialObject() );
}


/** DEPRECATED method, Used to used when building a scene graph with SpatialObjects. */
void 
SpatialObject
::RequestAddObject( Self * object )
{
if( object )  // ALL THIS METHOD WILL GO AWAY... don't be too picky about the style at this point.
  {
  Transform transform;
  transform.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
  object->RequestSetTransform( transform, this );
  }
}


/** Report that an invalid or suspicious operation has been requested.
 * This may mean that an error condition has arised in one of the
 * componenta that interact with this SpatialObject. */
void
SpatialObject::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}
#else // USE_SPATIAL_OBJECT_DEPRECATED not defined
/** 
 *  This version is the new API. There are no deprecated methods for the 
 *  implementation below.
 */

/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  m_SpatialObject = NULL;

  igstkAddInputMacro( InternalSpatialObjectNull );
  igstkAddInputMacro( InternalSpatialObjectValid );

  igstkAddStateMacro( Initial  );

  igstkAddTransitionMacro( Initial, InternalSpatialObjectNull, 
                           Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, InternalSpatialObjectValid, 
                           Initial,  SetInternalSpatialObject );

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

/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * 
SpatialObject::GetInternalSpatialObject() const
{
  return m_SpatialObject;
}

/** Report that an invalid or suspicious operation has been requested.
 * This may mean that an error condition has arised in one of the
 * componenta that interact with this SpatialObject. */
void
SpatialObject::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

void
SpatialObject
::InternalSpatialObjectNullProcessing()
{
  igstkLogMacro( WARNING, "Spatial object was NULL when trying to SetInternalSpatialObject." );
}

#endif // USE_SPATIAL_OBJECT_DEPRECATED
} // end namespace igstk
