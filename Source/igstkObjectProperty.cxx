/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectProperty.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkObjectProperty.h" 

namespace igstk 
{ 

/** Constructor */
ObjectProperty::ObjectProperty()
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Color[3] = 1.0;
} 

/** Destructor */
ObjectProperty::~ObjectProperty()  
{ 
}

/** Set the color */
void ObjectProperty::SetColor(float r, float g, float b, float a)
{
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
  m_Color[3] = a;
}


/** Print Self function */
void ObjectProperty::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Color: " << m_Color[0] << " : " << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Color[3] << std::endl;
}

} // end namespace igstk

