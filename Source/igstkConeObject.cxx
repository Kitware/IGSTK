/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkConeObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
ConeObject::ConeObject():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_ConeSpatialObject = ConeSpatialObjectType::New();
  this->RequestSetSpatialObject( m_ConeSpatialObject );
  m_Radius = 1;
  m_Height = 1;
} 

/** Destructor */
ConeObject::~ConeObject()  
{
}


void ConeObject::SetRadius( double radius )
{
  //m_ConeSpatialObject->SetRadius( radius );
  m_Radius = radius;
}


void ConeObject::SetHeight( double height ) 
{
  //m_ConeSpatialObject->SetHeight( height );
  m_Height = height;
}

double ConeObject::GetRadius() const
{
  //return m_ConeSpatialObject->GetRadius();
  return m_Radius;
}


double ConeObject::GetHeight() const
{
  //return m_ConeSpatialObject->GetHeight();
  return m_Height;
}


/** Print object information */
void ConeObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_ConeSpatialObject )
  {
    os << indent << this->m_ConeSpatialObject << std::endl;
  }
  os << indent << "Radius = " << m_Radius << std::endl;
  os << indent << "Height = " << m_Height << std::endl;
}


} // end namespace igstk

