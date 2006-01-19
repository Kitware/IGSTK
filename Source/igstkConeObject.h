/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkConeObject_h
#define __igstkConeObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkGroupSpatialObject.h>

namespace igstk
{

/** \class ConeObject
 * 
 * \brief This class represents a cone object.
 *
 * The parameters of the object are the height of the object, and the radius.
 * Default representation axis is X.
 * 
 * \ingroup Object
 */

class ConeObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ConeObject, SpatialObject )

public:

  /** Set the radius of the Cone */
  void SetRadius( double radius );
  
  /** Get the radius of the Cone */
  double GetRadius() const;
  
  /** Set the height of the Cone */
  void SetHeight( double height );
  
  /** Get the height of the Cone */
  double GetHeight() const;
  
protected:

  ConeObject( void );
  ~ConeObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     ConeSpatialObjectType;

  /** Internal itkSpatialObject */
  ConeSpatialObjectType::Pointer   m_ConeSpatialObject;

  double m_Radius;
  double m_Height;

};

} // end namespace igstk

#endif // __igstkConeObject_h

