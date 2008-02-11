/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBoxObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkBoxObject.h"

namespace igstk
{ 

/** Constructor */
BoxObject::BoxObject():m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "Constructor called ....\n" );

  m_BoxSpatialObject = BoxSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_BoxSpatialObject );
  m_Size[0] = 1.0;
  m_Size[1] = 1.0;
  m_Size[2] = 1.0;
} 

/** Destructor */
BoxObject::~BoxObject()  
{
  igstkLogMacro( DEBUG, "Destructor called ....\n" );
}

/** Set the size of each axis */
void BoxObject::SetSize(double x, double y, double z)
{
  igstkLogMacro( DEBUG, "SetSize() called ....\n" );

  m_Size[0] = x;
  m_Size[1] = y;
  m_Size[2] = z;
}

/** Get size of the X axis */
double BoxObject::GetSizeX() const
{
  igstkLogMacro( DEBUG, "GetSizeX() called ....\n" );
  
  return m_Size[0];
}

/** Get size of the Y axis */
double BoxObject::GetSizeY() const
{
  igstkLogMacro( DEBUG, "GetSizeY() called ....\n" );

  return m_Size[1];
}

/** Get size of the Z axis */
double BoxObject::GetSizeZ() const
{
  igstkLogMacro( DEBUG, "GetSizeZ() called ....\n" );
  
  return m_Size[2];
}

/** Print object information */
void BoxObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Size = " << m_Size << std::endl;
}


} // end namespace igstk
