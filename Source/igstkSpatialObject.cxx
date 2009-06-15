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

namespace igstk
{ 

/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  this->m_SpatialObject = NULL;

  igstkAddInputMacro( InternalSpatialObjectNull );
  igstkAddInputMacro( InternalSpatialObjectValid );
  igstkAddInputMacro( RequestBounds );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( Ready  );

  igstkAddTransitionMacro( Initial, InternalSpatialObjectNull, 
                           Initial, ReportSpatialObjectNull );
  igstkAddTransitionMacro( Initial, InternalSpatialObjectValid, 
                           Ready,   SetInternalSpatialObject );
  igstkAddTransitionMacro( Initial, RequestBounds, 
                           Initial, ReportBoundsNotAvailable );

  igstkAddTransitionMacro( Ready, RequestBounds, 
                           Ready, ReportBounds );

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();

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
  os << indent << "CoordinateSystemDelegator: ";
  this->m_CoordinateSystemDelegator->PrintSelf( os, indent );
}

/** Request setting the ITK spatial object that provide internal 
 *  functionalities. */
void SpatialObject
::RequestSetInternalSpatialObject( SpatialObjectType * spatialObject )
{  
  this->m_SpatialObjectToBeSet = spatialObject;

  if( this->m_SpatialObjectToBeSet.IsNull() )
    {
    igstkPushInputMacro( InternalSpatialObjectNull );
    this->m_StateMachine.ProcessInputs();
    }
  else 
    {
    igstkPushInputMacro( InternalSpatialObjectValid );
    this->m_StateMachine.ProcessInputs();
    }
}
 

/** Set the ITK spatial object that provide internal functionalities.
 * This method should only be called from the StateMachine */
void SpatialObject::SetInternalSpatialObjectProcessing()
{
  this->m_SpatialObject = this->m_SpatialObjectToBeSet;
}

/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * 
SpatialObject::GetInternalSpatialObject() const
{
  return this->m_SpatialObject;
}

void
SpatialObject
::RequestGetBounds() const
{
  igstkLogMacro( DEBUG, "SpatialObject::RequestGetBounds() called ....\n");
  Self * self = const_cast< Self * >( this );
  self->RequestGetBounds();
}

void
SpatialObject
::RequestGetBounds() 
{
  igstkLogMacro( DEBUG, "SpatialObject::RequestGetBounds() called ....\n");

  igstkPushInputMacro( RequestBounds );
  this->m_StateMachine.ProcessInputs();
}

void
SpatialObject
::ReportBoundsProcessing() 
{
  igstkLogMacro( DEBUG, "SpatialObject::ReportBoundsProcessing() called....\n");

  BoundingBoxEvent  event;
  event.Set( this->GetInternalSpatialObject()->GetBoundingBox() );
  this->InvokeEvent( event );
}

void
SpatialObject
::ReportBoundsNotAvailableProcessing()
{
  igstkLogMacro( WARNING, 
    "Spatial Object bounds are not available." );
  this->InvokeEvent( InvalidRequestErrorEvent() );
}

void
SpatialObject
::ReportSpatialObjectNullProcessing()
{
  igstkLogMacro( WARNING, 
    "Spatial object was NULL when trying to SetInternalSpatialObject." );
  this->InvokeEvent( InvalidRequestErrorEvent() );
}

} // end namespace igstk
