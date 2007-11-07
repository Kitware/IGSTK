/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCylinderObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
CylinderObject::CylinderObject():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = CylinderSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_CylinderSpatialObject );
} 

/** Destructor */
CylinderObject::~CylinderObject()  
{
}


void CylinderObject::SetRadius( double radius )
{
  m_CylinderSpatialObject->SetRadius( radius );
}


void CylinderObject::SetHeight( double height ) 
{
  m_CylinderSpatialObject->SetHeight( height );
}

double CylinderObject::GetRadius() const
{
  return m_CylinderSpatialObject->GetRadius();
}


double CylinderObject::GetHeight() const
{
  return m_CylinderSpatialObject->GetHeight();
}


/** Print object information */
void CylinderObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_CylinderSpatialObject )
    {
    os << indent << this->m_CylinderSpatialObject << std::endl;
    }
}


} // end namespace igstk
