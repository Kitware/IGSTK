/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCylinderObject_h
#define __igstkCylinderObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkCylinderSpatialObject.h>

namespace igstk
{

/** \class CylinderObject
 * 
 * \brief This class represents a cylinder object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class CylinderObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef CylinderObject                 Self;
  typedef SpatialObject                  Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 
  typedef itk::CylinderSpatialObject     CylinderSpatialObjectType;

  /**  Run-time type information (and related methods). */
  itkTypeMacro( Self, SpatialObject );

  /** Method for creation of a reference counted object. */
  NewMacro( CylinderObject );


  /** Set the radius of the Cylinder */
  void SetRadius( double radius );
  
  /** Get the radius of the Cylinder */
  double GetRadius() const;
  
  /** Set the height of the Cylinder */
  void SetHeight( double height );
  
  /** Get the height of the Cylinder */
  double GetHeight() const;
  
protected:

  CylinderObject( void );
  ~CylinderObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::Pointer   m_CylinderSpatialObject;

};

} // end namespace igstk

#endif // __igstkCylinderObject_h

