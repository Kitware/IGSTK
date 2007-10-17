/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  igstkSetMacro(Radius,double);
  
  /** Get the radius of the Cone */
  igstkGetMacro(Radius,double);
  
  /** Set the height of the Cone */
  igstkSetMacro(Height,double); 

  /** Get the height of the Cone */
  igstkGetMacro(Height,double);
 
protected:

  ConeObject( void );
  ~ConeObject( void );

  ConeObject(const Self&);            //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

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
