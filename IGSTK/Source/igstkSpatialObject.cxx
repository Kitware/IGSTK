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

} // end namespace igstk
