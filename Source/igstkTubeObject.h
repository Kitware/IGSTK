/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTubeObject_h
#define __igstkTubeObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkTubeSpatialObject.h>

namespace igstk
{

/** \class TubeObject
 * 
 * \brief This class represents a Tube object.
 * The tube is basically defined by a set of points representing its centerline.
 * Each point as a position and an associated radius value.
 *
 * \ingroup Object
 */

class TubeObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TubeObject, SpatialObject )

public:

  /** Internal typedef */
  typedef itk::TubeSpatialObject<3>             TubeSpatialObjectType;
  typedef TubeSpatialObjectType::TubePointType  PointType;
  typedef TubeSpatialObjectType::PointListType  PointListType;

  /** Add a point to the tube */
  void AddPoint(PointType & point);

  /** Return the number of points in the tube */
  unsigned int GetNumberOfPoints(void) const;

  /** Return a given point */
  const PointType * GetPoint(unsigned int pointId) const;

  /** Return the internal list of points */
  const PointListType GetPoints() const;

protected:

  /** Constructor */
  TubeObject( void );

  /** Destructor */
  ~TubeObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  TubeSpatialObjectType::Pointer   m_TubeSpatialObject;

};

} // end namespace igstk

#endif // __igstkTubeObject_h
