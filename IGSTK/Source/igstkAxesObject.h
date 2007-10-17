/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAxesObject_h
#define __igstkAxesObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"

namespace igstk
{

/** \class AxesObject
 * 
 * \brief This class represents a Axes object. 
 *
 * This class display a coordinate axis in a View. It is intended for providing
 * a visual reference of the orientation of space in the context of the scene.
 * 
 * \ingroup Object
 */

class AxesObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AxesObject, SpatialObject )

public:
  
  /** Set the size of each axis */
  void SetSize(double x, double y, double z);
  
  /** Get size of the X axis */
  double GetSizeX() const;
 
  /** Get size of the Y axis */
  double GetSizeY() const;
 
  /** Get size of the Z axis */
  double GetSizeZ() const;

protected:

  AxesObject( void );
  ~AxesObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     AxesSpatialObjectType;

  AxesObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  AxesSpatialObjectType::Pointer     m_AxesSpatialObject;

  double                             m_Size[3];

};

} // end namespace igstk

#endif // __igstkAxesObject_h
