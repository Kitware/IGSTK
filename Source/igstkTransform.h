/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkTransform_H
#define __igstkTransform_H


#include "igstkTimeStamp.h"

#include "itkVector.h"
#include "itkVersor.h"


/** \brief Transform representing relative positions and orientations in 3D space.
 * 
 * This class represents relative positions and orientations in 3D space. It is
 * intended to be used for positioning objects in the Scene and as a
 * communication vehicle between trackers and spatial objects. The class
 * contains a TimeStamp defining the validity period for the information in the
 * transform. 
 * 
 * This class can be used for storing Translation and Rotation simultaneously,
 * or only Translation, or only Rotation. When only Translation or only
 * Rotation are used, the other component of the transform is set to an
 * identity. If the user intends to use both rotations and translation, she
 * must invoke the SetTranslationAndRotation() method.
 * 
 * All the set methods require an argument that defines the number of
 * milliseconds for which the stored information is considered to be valid.
 * The validity period will be counted from the moment the Set method was
 * invoked.
 *
 **/

namespace igstk 
{
 
class Transform
{

public:

  /** Constructor and destructor */
  Transform();
  ~Transform();

  /** Set Translation and Rotation simultaneously */
  void SetTranslationAndRotation(
          double tx, double ty, double tz,
          double qx, double qy, double qz, double qw,
          double millisecondsToExpiration);

private:

  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;
  
  TimeStamp       m_TimeStamp;
  VectorType      m_Translation;
  VersorType      m_Rotation;

};


}

#endif

