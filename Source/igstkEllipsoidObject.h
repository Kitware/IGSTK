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
#include "igstkObjectRepresentation.h"
#include <itkEllipseSpatialObject.h>

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
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef EllipsoidObject                Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef itk::EllipseSpatialObject<3>   EllipseSOType;
  typedef EllipseSOType::ArrayType         ArrayType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, Superclass );

  /** Method for creation of a reference counted object. */
  NewMacro( EllipsoidObject );

  /** Set all radii to the same radius value.  Each radius is
   *  half the length of one axis of the ellipse.  */
  void SetRadius(double radius);
  void SetRadius(double r0, double r1, double r2);
  ArrayType GetRadius();

  /** Get radii via an array of radius values */
  itkGetConstReferenceMacro(Radius,ArrayType);

  /** Create the VTK actors */
  void CreateVTKActors();

protected:

  EllipsoidObject( void );
  ~EllipsoidObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Radius array */
  ArrayType               m_Radius;

  /** Internal itkSpatialObject */
  EllipseSOType::Pointer   m_EllipseSO;

};

} // end namespace igstk

#endif // __igstkEllipsoidObject_h
