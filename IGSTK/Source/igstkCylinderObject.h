/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
 * \brief This class represents a cylinder object. The parameters of the 
 * object are the height of the object, and the radius. 
 * Default representation axis is X.
 * 
 * \ingroup Object
 */

class CylinderObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CylinderObject, SpatialObject )

public:

  /** Typedefs */
  typedef itk::CylinderSpatialObject     CylinderSpatialObjectType;

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

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  CylinderSpatialObjectType::Pointer   m_CylinderSpatialObject;

};

} // end namespace igstk

#endif // __igstkCylinderObject_h
