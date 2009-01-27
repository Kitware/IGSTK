/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTransform_h
#define __igstkTransform_h

#include "igstkTransformBase.h"

#include "itkVector.h"
#include "itkVersor.h"

#include "vtkMatrix4x4.h"

#include "igstkTimeStamp.h"
#include "igstkMacros.h"

namespace igstk 
{
 
/** \class Transform 
 *  \brief A class representing a 3D Rigid transform
 * 
 * This class represents relative positions and orientations in 3D space. It is
 * intended to be used for positioning objects in the scene and as a
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
 * \sa TimeStamp
 *
 * */
  class Transform : public TransformBase
{
public:

  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Point<double, 3>     PointType;
  typedef ::itk::Versor<double>       VersorType;
  
public:

  /** Constructor and destructor */
  Transform();
  Transform( const Transform & t );
  virtual ~Transform();

  /** Transform composition method */
  static Transform TransformCompose( Transform leftTransform, 
                                     Transform rightTransform );

  /** Assign the values of one transform to another */
  const Transform & operator=( const Transform & inputTransform );

  /** Set Translation and Rotation simultaneously. This values will override
   * any previously set rotation and translation. The information will be
   * considered valid from the time of invokation of the method until that time
   * plus the millisecondsToExpiration value. */
  void SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
          TransformBase::ErrorType errorValue,
          TimeStamp::TimePeriodType millisecondsToExpiration );


  /** Set only Rotation. This method should be used when the transform
   * represents only a rotation. Internally the translational part of the
   * transform will be set to zero. The assigned rotation will override any
   * previously set rotation and will set to zero any previous translation.
   * The information will be considered valid from the time of invokation of
   * the method until that time plus the millisecondsToExpiration value. */
  void SetRotation( 
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration );

  /** Set only Translation. This method should be used when the transform
   * represents only a translation. Internally the rotational part of the
   * transform will be set to zero. The assigned translation will override any
   * previously set translation and will set to zero any previous rotation.
   * The information will be considered valid from the time of invokation of
   * the method until that time plus the millisecondsToExpiration value. */
  void SetTranslation( 
          const  VectorType & translation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration );

  /** Returns the translational component. Users MUST check the validity time
   * of the transform before attempting to use this returned value. The content
   * of the transform may have expired. */
  igstkGetMacro( Translation, VectorType );
 
 
  /** Returns the rotational component. Users MUST check the validity time
   * of the transform before attempting to use this returned value. The content
   * of the transform may have expired. */
  igstkGetMacro( Rotation, VersorType ); 
 

  /** Export the content of the transformation in the format of a vtkMatrix4x4.
   * Users must allocate the matrix first and then pass it by reference to this
   * method.  The current method will simply fill in the transform using the
   * current information of translation and rotation.
   */
  void ExportTransform( vtkMatrix4x4 & matrix ) const;

  /** Set the content of the transformation from a vtkMatrix4x4.
   */
  void ImportTransform( vtkMatrix4x4 & matrix );

  /** Compare two transforms for equivalence, in the 
   *  sense that these are the same objects in memory.
   */
  bool operator==( const Transform & inputTransform );
  bool operator!=( const Transform & inputTransform );

  /** Compare two transforms for equivalence. */
  bool IsNumericallyEquivalent( 
    const Transform& inputTransform, double tol = vnl_math::eps ) const;

  /** Evaluate if the Transform is an Identity upto a given tolerance */
  bool IsIdentity( double tol = vnl_math::eps ) const;

  /** Resets the Transform to an Identity Transform */
  void SetToIdentity( TimePeriodType validityPeriodInMilliseconds );

  /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;

  /** Returns the Inverse Transform of the current one */
  Transform GetInverse() const;


protected:

  void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  void PrintTrailer( std::ostream& itkNotUsed(os), 
                     itk::Indent itkNotUsed(indent) ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  VectorType      m_Translation;
  VersorType      m_Rotation;

};

std::ostream& operator<<( std::ostream& os, const igstk::Transform& o );
}

#endif
