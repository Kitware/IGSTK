/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkEllipsoidObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkEllipsoidObjectRepresentation_h
#define __igstkEllipsoidObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkEllipsoidObject.h"
#include "vtkSuperquadricSource.h"

namespace igstk
{

/** \class EllipsoidObjectRepresentation
 * 
 * \brief This class represents an ellipsoid object. The parameters of the object
 * are the three radii defining the lenght of the principal axes of the ellipsoid.
 * The axes of the ellipsoid are orthogonal.
 * 
 * \ingroup Object
 */

class EllipsoidObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef EllipsoidObjectRepresentation  Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef EllipsoidObject                EllipsoidObjectType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, ObjectRepresentation );

  /** Method for creation of a reference counted object. */
  NewMacro( EllipsoidObjectRepresentation );

  /** Create the VTK actors */
  void CreateActors();

  /** Connect this representation class to the spatial object */
  void SetEllipsoid( const EllipsoidObjectType * ellipsoidObject );

protected:

  EllipsoidObjectRepresentation( void );
  ~EllipsoidObjectRepresentation( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  EllipsoidObjectType::ConstPointer   m_EllipsoidObject;

  /** VTK class that generates the geometrical representation of the cylinder */
  vtkSuperquadricSource * m_EllipsoidSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationFromGeometry();

};

} // end namespace igstk

#endif // __igstkEllipsoidObjectRepresentation_h

