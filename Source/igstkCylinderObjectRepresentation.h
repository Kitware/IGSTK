/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCylinderObjectRepresentation_h
#define __igstkCylinderObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "vtkCylinderSource.h"

namespace igstk
{

/** \class CylinderObjectRepresentation
 * 
 * \brief This class represents a cylinder object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class CylinderObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef CylinderObjectRepresentation                 Self;
  typedef ObjectRepresentation           Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef CylinderObject                 CylinderSpatialObjectType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, ObjectRepresentation );

  /** Method for creation of a reference counted object. */
  NewMacro( CylinderObjectRepresentation );

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void SetCylinder( const CylinderSpatialObjectType * cylinderObject );

protected:

  CylinderObjectRepresentation( void );
  ~CylinderObjectRepresentation( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::ConstPointer   m_CylinderSpatialObject;

  /** VTK class that generates the geometrical representation of the cylinder */
  vtkCylinderSource * m_CylinderSource;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationFromGeometry();

};

} // end namespace igstk

#endif // __igstkCylinderObjectRepresentation_h

