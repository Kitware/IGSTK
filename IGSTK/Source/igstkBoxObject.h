/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBoxObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkBoxObject_h
#define __igstkBoxObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkGroupSpatialObject.h>
#include <itkFixedArray.h>

namespace igstk
{

/** \class BoxObject
 * 
 * \brief This class represents a Box object. 
 *
 * The parameters of the object are the height of the object, and the radius.
 * Default representation axis is Z.
 * 
 * \ingroup Object
 */

class BoxObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( BoxObject, SpatialObject )

public:
  
  /** Typedefs */
  typedef itk::FixedArray<double,3> ArrayType;

  /** Set the size of each axis */
  void SetSize(double x, double y, double z);

  /** Set the size given an array */
  igstkSetMacro( Size , ArrayType );

  /** Get the size as an array */
  igstkGetMacro( Size , ArrayType );
  
  /** Get size of the X axis */
  double GetSizeX() const;
 
  /** Get size of the Y axis */
  double GetSizeY() const;
 
  /** Get size of the Z axis */
  double GetSizeZ() const;

protected:

  BoxObject( void );
  ~BoxObject( void );

  BoxObject(const Self&);            //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  
  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     BoxSpatialObjectType;

  BoxSpatialObjectType::Pointer m_BoxSpatialObject;
  ArrayType                     m_Size;

};

} // end namespace igstk

#endif // __igstkBoxObject_h
