/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkConeObject.h"

namespace igstk
{ 

/** Constructor */
ConeObject::ConeObject():m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "Constructor called ....\n" );

  // We create the ellipse spatial object
  m_ConeSpatialObject = ConeSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_ConeSpatialObject );
  m_Radius = 1;
  m_Height = 1;
} 

/** Destructor */
ConeObject::~ConeObject()  
{
  igstkLogMacro( DEBUG, "Destructor called ....\n" );
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
