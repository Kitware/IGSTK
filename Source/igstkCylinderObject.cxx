/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCylinderObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
CylinderObject::CylinderObject()
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = CylinderSpatialObjectType::New();
  this->SetSpatialObject( m_CylinderSpatialObject );
} 

/** Destructor */
CylinderObject::~CylinderObject()  
{
}


void CylinderObject::SetRadius( double radius )
{
  m_CylinderSpatialObject->SetRadius( radius );
  this->InvokeEvent( GeometryModifiedEvent() );
}


void CylinderObject::SetHeight( double height ) 
{
  m_CylinderSpatialObject->SetHeight( height );
  this->InvokeEvent( GeometryModifiedEvent() );
}

double CylinderObject::GetRadius() const
{
  return m_CylinderSpatialObject->GetRadius();
}


double CylinderObject::GetHeight() const
{
  return m_CylinderSpatialObject->GetHeight();
}


/** Print Self function */
void CylinderObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

