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
  m_SpatialObject->GetObjectToWorldTransform()->SetOffset(offset);
  this->InvokeEvent( PositionModifiedEvent() ); 
}

/** Get Offset */
SpatialObject::VectorType
SpatialObject::GetOffset()  const
{
   return m_SpatialObject->GetObjectToWorldTransform()->GetOffset();
}


/** Get Matrix */
SpatialObject::MatrixType
SpatialObject::GetMatrix()  const
{
   return m_SpatialObject->GetObjectToWorldTransform()->GetMatrix();
}


} // end namespace igstk

