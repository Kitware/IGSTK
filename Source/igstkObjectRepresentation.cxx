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

/** Return the Property used for rendering */
ObjectProperty * 
ObjectRepresentation::GetProperty()
{
  return & m_Property;
}

/** Add an actor to the actors list */
void
ObjectRepresentation::AddActor( vtkProp3D * actor )
{
  m_Actors.push_back( actor );
}


/** Print Self function */
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

