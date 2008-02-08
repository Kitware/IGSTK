/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPoint3D.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPoint3D_h
#define __igstkPoint3D_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "itkPoint.h"
#include "itkSmartPointer.h"


namespace igstk {

// forward declaration.
class SpatialObject; 

/** \class Point3D
 * 
 * \brief Class for representing a position in 3D Space.
 *
 * \ingroup Object
 */
class Point3D : public ::itk::Point< double, 3 >
{

public:
    
  typedef Point3D                   Self;
  typedef itk::Point<double, 3>     Superclass;

  typedef ::itk::SmartPointer< const SpatialObject > SpatialObjectPointer;

  /** Constructor. It will initialize the point to (0,0,0) and the world
   * coordinate system. */
  Point3D();
  ~Point3D();

  /** Set the spatial object serving as coordinate reference system for this point */
  void SetCoordinateSystem( const SpatialObject * referenceSystem );

private:

  SpatialObjectPointer  m_ReferenceFrame;

};

} // end namespace igstk

#endif
