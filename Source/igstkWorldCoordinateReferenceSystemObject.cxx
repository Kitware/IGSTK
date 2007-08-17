/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWorldCoordinateReferenceSystemObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkWorldCoordinateReferenceSystemObject.h"

namespace igstk
{ 

/** Constructor */
WorldCoordinateReferenceSystemObject
::WorldCoordinateReferenceSystemObject():m_StateMachine(this)
{
  this->RequestAttachToSpatialObjectParent( this );

  double validityTimeInMilliseconds = 
    TimeStamp::GetLongestPossibleTime();

  Transform::VectorType translation;
  translation[0] = 0.0;
  translation[1] = 0.0;
  translation[2] = 0.0;

  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );

  igstk::Transform::ErrorType errorValue = 0.001;  // one micron

  // Initialize the Identity Transform
  this->m_IdentityTransformToWorld.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  this->RequestSetTransformToSpatialObjectParent( 
    this->m_IdentityTransformToWorld );
} 

/** Destructor */
WorldCoordinateReferenceSystemObject
::~WorldCoordinateReferenceSystemObject()  
{
}

/** Compute the transform from the current object to the world coordinate system.
 *  Since this object is the WorldCoordinateReferenceSystem, the transform is an
 *  identity, and it is the end point of the recursion of this method.
 */
const Transform &
WorldCoordinateReferenceSystemObject
::ComputeTransformToWorld() const
{
  std::cout << "WorldCoordinateReferenceSystemObject::ComputeTransformToWorld() " << std::endl;
  std::cout << "Returning the Identity Transform = " << std::endl;
  std::cout << m_IdentityTransformToWorld << std::endl;
  return this->m_IdentityTransformToWorld;
}


/** Print object information */
void WorldCoordinateReferenceSystemObject::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
