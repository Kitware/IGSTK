/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVesselObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVesselObject.h"

namespace igstk
{ 

/** Constructor */
VesselObject::VesselObject():m_StateMachine(this)
{
  // We create the verssel spatial object
  m_VesselSpatialObject = VesselSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_VesselSpatialObject );
} 

/** Destructor */
VesselObject::~VesselObject()  
{
}

/** Set the ITKVesselSpatialObject. 
 *  this is accessible only from the friend classes */
void VesselObject
::SetVesselSpatialObject( VesselSpatialObjectType * vessel ) 
{
  if(vessel)
    {
    m_VesselSpatialObject = vessel;
    this->RequestSetInternalSpatialObject( m_VesselSpatialObject );
    }
}

/** Add a point to the Vessel */
void VesselObject::AddPoint(PointType & point)
{
  m_VesselSpatialObject->GetPoints().push_back(point);
}

/** Return the spacing of the vessel */
const double*
VesselObject::GetSpacing() const
{
  return m_VesselSpatialObject->GetSpacing();
}

/** Remove all the points in the list */
void VesselObject::Clear( void )
{
  m_VesselSpatialObject->Clear();
}

/** Return the number of points in the Vessel */
unsigned int VesselObject::GetNumberOfPoints(void) const
{
  return m_VesselSpatialObject->GetPoints().size();
}

/** Return a given point */
const VesselObject::PointType * 
VesselObject::GetPoint(unsigned int pointId) const
{
  if(pointId >= m_VesselSpatialObject->GetPoints().size())
    {
    return NULL;
    }

  PointListType &points = m_VesselSpatialObject->GetPoints();
  return &(points[pointId]);
}


/** Return the internal list of points */
const VesselObject::PointListType VesselObject::GetPoints() const
{
  return m_VesselSpatialObject->GetPoints();
}


/** Print object information */
void VesselObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_VesselSpatialObject )
    {
    os << indent << this->m_VesselSpatialObject << std::endl;
    }
}

} // end namespace igstk
