/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkAxesObject.h"

namespace igstk
{ 

/** Constructor */
AxesObject::AxesObject():m_StateMachine(this)
{
  m_AxesSpatialObject = AxesSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_AxesSpatialObject );
  m_Size[0] = 1.0;
  m_Size[1] = 1.0;
  m_Size[2] = 1.0;
} 

/** Destructor */
AxesObject::~AxesObject()  
{
}

/** Set the size of each axis */
void AxesObject::SetSize(double x, double y, double z)
{
  m_Size[0] = x;
  m_Size[1] = y;
  m_Size[2] = z;
}

/** Get size of the X axis */
double AxesObject::GetSizeX() const
{
  return m_Size[0];
}

/** Get size of the Y axis */
double AxesObject::GetSizeY() const
{
  return m_Size[1];
}

/** Get size of the Z axis */
double AxesObject::GetSizeZ() const
{
  return m_Size[2];
}

/** Print object information */
void AxesObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Size = " << m_Size[0] << " , ";
  os << m_Size[1] << "," << m_Size[2] << std::endl;
}


} // end namespace igstk
