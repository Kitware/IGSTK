/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPoint3D.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkPoint3D.h"
#include "igstkSpatialObject.h"


namespace igstk {


/* Constructor. It will initialize the point to (0,0,0) and the world
 * coordinate system. */
Point3D::Point3D()
{
  this->Fill( 0.0 );
  this->m_ReferenceFrame = NULL;
}

/* Destructor. */
Point3D::~Point3D()
{
}

/* Set the spatial object serving as coordinate reference system for this point */
void 
Point3D::SetCoordinateReferenceSystem( const SpatialObject * referenceSystem )
{
  this->m_ReferenceFrame = referenceSystem;
}


} // end namespace igstk
