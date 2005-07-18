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
 * \brief This class represents a Tube object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class TubeObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef TubeObject                            Self;
  typedef SpatialObject                         Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer; 
  typedef itk::TubeSpatialObject<3>             TubeSpatialObjectType;
  typedef TubeSpatialObjectType::TubePointType  PointType;
  typedef TubeSpatialObjectType::PointListType  PointListType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( TubeObject, SpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( TubeObject );

  /** Add a point to the tube */
  void AddPoint(PointType & point);

  /** Return the number of points in the tube */
  unsigned int GetNumberOfPoints(void) const;

  /** Return a given point */
  const PointType * GetPoint(unsigned int pointId) const;

protected:

  TubeObject( void );
  ~TubeObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  TubeSpatialObjectType::Pointer   m_TubeSpatialObject;

};

} // end namespace igstk

#endif // __igstkTubeObject_h
