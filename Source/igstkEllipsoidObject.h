/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkEllipsoidObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkEllipsoidObject_h
#define __igstkEllipsoidObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkEllipseSpatialObject.h"

namespace igstk
{

/** \class EllipsoidObject
 * 
 * \brief This class represents an ellipsoid object. The parameters of the object
 * are the three radii defining the lenght of the principal axes of the ellipsoid.
 * The axes of the ellipsoid are orthogonal.
 * 
 * \ingroup Object
 */

class EllipsoidObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef EllipsoidObject                Self;
  typedef SpatialObject                  Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef itk::EllipseSpatialObject<3>   EllipseSpatialObjectType;
  typedef EllipseSpatialObjectType::ArrayType         ArrayType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, SpatialObject );

  /** Method for creation of a reference counted object. */
  NewMacro( EllipsoidObject );

  /** Set the radius of the underlying ITK Spatial object */
  void SetRadius( double rx, double ry, double rz );
  void SetRadius( const ArrayType & radius );

  /** Get the three radius */
  const ArrayType & GetRadius() const;

protected:

  EllipsoidObject( void );
  ~EllipsoidObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  EllipseSpatialObjectType::Pointer   m_EllipseSpatialObject;

};

} // end namespace igstk

#endif // __igstkEllipsoidObject_h

