/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeObject::TubeObject()
{
  // We create the ellipse spatial object
  m_TubeSpatialObject = TubeSpatialObjectType::New();
  this->RequestSetSpatialObject( m_TubeSpatialObject );
} 

/** Destructor */
TubeObject::~TubeObject()  
{
}


/** Add a point to the tube */
void TubeObject::AddPoint(PointType & point)
{
  m_TubeSpatialObject->GetPoints().push_back(point);
}

/** Return the number of points in the tube */
unsigned int TubeObject::GetNumberOfPoints(void) const
{
  return m_TubeSpatialObject->GetPoints().size();
}

/** Return a given point */
const TubeObject::PointType * TubeObject::GetPoint(unsigned int pointId) const
{
  if(pointId >= m_TubeSpatialObject->GetPoints().size())
    {
    return NULL;
    }

  PointListType points = m_TubeSpatialObject->GetPoints();
  PointListType::const_iterator it = points.begin();
  unsigned int i=0;
  while(it != points.end())
    {
    if(i == pointId)
      {
      return &(*it);
      }
    i++;
    it++;
    }

  return NULL;
}

/** Print object information */
void TubeObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_TubeSpatialObject )
  {
    os << indent << this->m_TubeSpatialObject << std::endl;
  }
}


} // end namespace igstk

