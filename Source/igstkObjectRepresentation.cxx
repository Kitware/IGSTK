/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkObjectRepresentation.h" 

namespace igstk 
{ 

/** Constructor */
ObjectRepresentation::ObjectRepresentation()
{
} 

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()  
{ 
}

/** Return the number of children */
unsigned int ObjectRepresentation::GetNumberOfChildren(unsigned int depth, char * name )
{
  unsigned int count = 0;
  SpatialObjectsListType::const_iterator it = m_SpatialObjectList.begin();
  while(it != m_SpatialObjectList.end())
    {
    count += (*it)->GetNumberOfChildren(depth,name);
    it++;
    }
  return count;
}

/** Print Self function */
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

