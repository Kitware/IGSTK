/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkSpatialObject.h"
#include "igstkEvents.h"

namespace igstk
{ 


/** Constructor */
SpatialObject::SpatialObject()
{
  m_SpatialObject = NULL;
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

/** Set the offset in the Transform */
void SpatialObject::SetSpatialObject( SpatialObjectType * spatialObject )
{
  m_SpatialObject = spatialObject;
}
 
/** Set the offset in the Transform */
void SpatialObject::SetOffset(double ofx, double ofy, double ofz)  
{
  VectorType offset;
  offset[0] = ofx;
  offset[1] = ofy;
  offset[2] = ofz;
  if(m_SpatialObject)
    {
    m_SpatialObject->GetObjectToWorldTransform()->SetOffset(offset);
    this->InvokeEvent( PositionModifiedEvent() );
    }
}

/** Get Offset */
const SpatialObject::VectorType &
SpatialObject::GetOffset()  const
{
  if(m_SpatialObject)
    {
    return m_SpatialObject->GetObjectToWorldTransform()->GetOffset();
    }

  // FIXME: we should handle error here
  return m_FakeVector;
}


/** Get Matrix */
const SpatialObject::MatrixType &
SpatialObject::GetMatrix()  const
{
  if(m_SpatialObject)
    {
    return m_SpatialObject->GetObjectToWorldTransform()->GetMatrix();
    }

  // FIXME: we should handle error here
  return m_FakeMatrix;
}


} // end namespace igstk

