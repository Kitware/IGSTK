/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkToolProjectionObject.h"

namespace igstk
{ 

/** Constructor */
ToolProjectionObject::ToolProjectionObject():m_StateMachine(this)
{
  m_ToolProjectionSpatialObject = ToolProjectionSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_ToolProjectionSpatialObject );
  m_Size = 100;
} 

/** Destructor */
ToolProjectionObject::~ToolProjectionObject()  
{
}

/** Set the size of each axis */
void ToolProjectionObject::SetSize(double size)
{
  m_Size = size;
}

/** Get size of the tool */
double ToolProjectionObject::GetSize() const
{
  return m_Size;
}

/** Print object information */
void ToolProjectionObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Size = " << m_Size << std::endl;
}


} // end namespace igstk
