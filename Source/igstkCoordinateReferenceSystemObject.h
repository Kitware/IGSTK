/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystemObject_h
#define __igstkCoordinateReferenceSystemObject_h

#include "igstkMacros.h"
#include "igstkAxesObject.h"

namespace igstk
{

/** \class CoordinateReferenceSystemObject
 * 
 * \brief This class represents the frame of a coordinate reference system.
 *
 * The class is intended to be used as an anchor relative to which position
 * Spatial Objects in the graph scene. Since this object derives from the
 * AxesObject, it can be rendere in the screen by using the
 * AxesSpatialObjectRepresentation class.
 * 
 * \ingroup Object
 */

class CoordinateReferenceSystemObject 
: public AxesObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateReferenceSystemObject, AxesObject )

protected:

  CoordinateReferenceSystemObject( void );
  ~CoordinateReferenceSystemObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  CoordinateReferenceSystemObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

};

} // end namespace igstk

#endif // __igstkCoordinateReferenceSystemObject_h
