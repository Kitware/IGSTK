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



#include "itkVector.h"
#include "itkVersor.h"

#include "vtkMatrix4x4.h"

#include "igstkTimeStamp.h"
#include "igstkMacros.h"

namespace igstk 
{
 
/** \class Transform 
 *  \brief Transform representing relative positions and orientations in 3D space.
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
 **/



class Transform
{
public:

  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Point<double, 3>     PointType;
  typedef ::itk::Versor<double>       VersorType;
  typedef double                      ErrorType;
  typedef TimeStamp::TimePeriodType   TimePeriodType;
  
public:

  /** Constructor and destructor */
  Transform();
  virtual ~Transform();

  /** Assign the values of one transform to another */
  const Transform & operator=( const Transform & inputTransform );

  /** Set Translation and Rotation simultaneously. This values will override
   * any previously set rotation and translation. The information will be
   * considered valid from the time of invokation of the method until that time
   * plus the millisecondsToExpiration value. */
  void SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration);


  /** Set only Rotation. This method should be used when the transform
   * represents only a rotation. Internally the translational part of the
   * transform will be set to zero. The assigned rotation will override any
   * previouly set rotation and will set to zero any previous translation.
   * The information will be considered valid from the time of invokation of
   * the method until that time plus the millisecondsToExpiration value. */
  void SetRotation( 
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration);

  /** Set only Translation. This method should be used when the transform
   * represents only a translation. Internally the rotational part of the
   * transform will be set to zero. The assigned translation will override any
   * previouly set translation and will set to zero any previous rotation.
   * The information will be considered valid from the time of invokation of
   * the method until that time plus the millisecondsToExpiration value. */
  void SetTranslation( 
          const  VectorType & translation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration);

  /** Returns the translational component. Users MUST check the validity time
   * of the transform before attempting to use this returned value. The content
   * of the transform may have expired. */
  igstkGetMacro( Translation, VectorType );
 
 
  /** Returns the rotational component. Users MUST check the validity time
   * of the transform before attempting to use this returned value. The content
   * of the transform may have expired. */
  igstkGetMacro( Rotation, VersorType ); 
       

  /** Returns the translational error of this transform. The correct
   * interpretation of this error my be tracker dependent and therefore must be
   * analyzed carefully. */
  igstkGetMacro( Error, ErrorType );
 

  /** Returns the time at which the validity of this information starts. The
   * data in this transform should not be used for scenes to be rendered
   * before that validity time. The time is returned in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  TimePeriodType GetStartTime() const;


  /** Returns the time at which the validity of this information expires. The
   * data in this transform should not be used for scenes to be rendered
   * after that validity time. The time is returned in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  TimePeriodType GetExpirationTime() const;


  /** Returns the validity status of the transform at the time passed as
   * argument. The transform values should not be used in a scene if the time
   * when the scene is to be rendered returned 'false' when passed to this
   * IsValid() function. The time is passed in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  bool IsValidAtTime( TimePeriodType timeToTestInMilliseconds ) const;


  /** Export the content of the transformation in the format of a vtkMatrix4x4.
   * Users must allocate the matrix first and then pass it by reference to this
   * method.  The current method will simply fill in the transform using the
   * current information of translation and rotation.
   */
  void ExportTransform( vtkMatrix4x4 & matrix ) const;


  /** Assign the values of one transform to another */
  bool operator==( const Transform & inputTransform );
  bool operator!=( const Transform & inputTransform );


  /** Resets the Transform to an Identity Transform */
  void SetToIdentity( TimePeriodType validityPeriodInMilliseconds );

  /** Method for printing the member variables of this class to an ostream */
  void Print(std::ostream& os, itk::Indent indent) const;

  /** Returns the Inverse Transform of the current one */
  Transform GetInverse() const;


protected:

  void PrintHeader(std::ostream& os, itk::Indent indent) const;

  void PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  TimeStamp       m_TimeStamp;
  VectorType      m_Translation;
  VersorType      m_Rotation;
  ErrorType       m_Error;

};


std::ostream& operator<<(std::ostream& os, const igstk::Transform& o);
}



#endif

