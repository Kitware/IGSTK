/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkEllipsoidObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkEllipsoidObject.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
EllipsoidObject::EllipsoidObject()
{
  // We create the ellipse spatial object
  m_EllipseSpatialObject = EllipseSpatialObjectType::New();
  this->SetSpatialObject( m_EllipseSpatialObject );
} 

/** Destructor */
EllipsoidObject::~EllipsoidObject()  
{
}

/** Set the radius along each axis */
void EllipsoidObject::SetRadius( double rx, double ry, double rz )
{
  EllipseSpatialObjectType::ArrayType radius;
  radius[0] = rx;
  radius[1] = ry;
  radius[2] = rz;
  m_EllipseSpatialObject->SetRadius( radius );
  this->InvokeEvent( GeometryModifiedEvent() );
}


/** Set the radii along each axis */
void EllipsoidObject::SetRadius( const ArrayType & radii )
{
  m_EllipseSpatialObject->SetRadius( radii );
  this->InvokeEvent( GeometryModifiedEvent() );
}


/** Get the radii along each axis */
const EllipsoidObject::ArrayType & 
EllipsoidObject::GetRadius() const
{
  return m_EllipseSpatialObject->GetRadius();
}



/** Print Self function */
void EllipsoidObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << m_EllipseSpatialObject << std::endl; 
}


} // end namespace igstk

