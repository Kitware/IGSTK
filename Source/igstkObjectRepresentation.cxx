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
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
} 

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()  
{ 
}

/** Add a spatial object to the list */
void
ObjectRepresentation::AddSpatialObject( SpatialObjectType * spatialObject ) 
{
  m_SpatialObjects.push_back( spatialObject );
}


/** Return the number of children */
unsigned int 
ObjectRepresentation::GetNumberOfChildren() const
{
  return m_SpatialObjects.size();
}

/** Add an actor to the actors list */
void
ObjectRepresentation::AddActor( vtkProp3D * actor )
{
  m_Actors.push_back( actor );
}

/** Set the color */
void ObjectRepresentation::SetColor(float r, float g, float b)
{
  if(m_Color[0] == r && m_Color[1] == g && m_Color[2] == b)
    {
    return;
    }
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
  this->Modified();
}


/** Print Self function */
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << m_Color[0] << " : " << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Opacity << std::endl;
}

} // end namespace igstk

