/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCrossHairObject_h
#define __igstkCrossHairObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"

namespace igstk
{

/** \class CrossHairObject
 * 
 * \brief This class represents a Axes object. 
 *
 * This class displays the projection of the tool on the View. It is intended for providing
 * a visual reference of the orientation of the tool in space in the context of a 2D View.
 * 
 * \ingroup Object
 */

class CrossHairObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CrossHairObject, SpatialObject )

public:
  
  /** Set the size of each axis */
  void SetSize(double size);
  
  /** Get size of the tool */
  double GetSize() const;
 
protected:

  CrossHairObject( void );
  ~CrossHairObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     CrossHairSpatialObjectType;

  CrossHairObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  CrossHairSpatialObjectType::Pointer     m_CrossHairSpatialObject;

  double                             m_Size;

};

} // end namespace igstk

#endif // __igstkCrossHairObject_h
