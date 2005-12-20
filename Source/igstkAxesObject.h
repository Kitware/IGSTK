/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
 * \brief This class represents a Axes object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is Z.
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
  
  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     AxesSpatialObjectType;

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
  
  AxesSpatialObjectType::Pointer m_AxesSpatialObject;
  double                         m_Size[3];

};

} // end namespace igstk

#endif // __igstkAxesObject_h

