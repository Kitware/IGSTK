/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWorldCoordinateReferenceSystemObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWorldCoordinateReferenceSystemObject_h
#define __igstkWorldCoordinateReferenceSystemObject_h

#include "igstkMacros.h"
#include "igstkCoordinateReferenceSystemObject.h"

namespace igstk
{

/** \class WorldCoordinateReferenceSystemObject
 * 
 * \brief This class represents the frame of the world coordinate reference system.
 *
 * The class is intended to be used as an anchor relative to which position
 * Spatial Objects in the graph scene. Since this object derives from the
 * CoordinateReferenceSystemObject, it can be rendered in the screen by using
 * the AxesSpatialObjectRepresentation class. Normally there should be only one
 * instance of this object in an application. All spatial object in a scene are
 * directly or indirectly attached to this one.
 * 
 * \ingroup Object
 */

class WorldCoordinateReferenceSystemObject 
: public CoordinateReferenceSystemObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( WorldCoordinateReferenceSystemObject, 
    CoordinateReferenceSystemObject )

protected:

  WorldCoordinateReferenceSystemObject( void );
  ~WorldCoordinateReferenceSystemObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Terminal point of the recursive invocation that computes the transform of
   * an object to the world coordinate system. */
  virtual const Transform & ComputeTransformToWorld() const;
  
private:

  WorldCoordinateReferenceSystemObject(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  /** Cache variable for finishing the recursion of the
   * ComputeTransformToWorld() method */
  mutable Transform      m_IdentityTransformToWorld;
};

} // end namespace igstk

#endif // __igstkWorldCoordinateReferenceSystemObject_h
