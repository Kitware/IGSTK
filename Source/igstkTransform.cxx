/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTransform.h"


namespace igstk
{

Transform
::Transform()
{
  // Error is NEVER zero. In the best situation is on the range of the smaller
  // non-zero epsilon that can be represented with the ErrorType. 
  m_Error = itk::NumericTraits< ErrorType >::min();
}


Transform
::~Transform()
{
}


const Transform &
Transform
::operator=( const Transform & inputTransform )
{
  m_Translation = inputTransform.m_Translation; 
  m_Rotation    = inputTransform.m_Rotation;
  m_Error       = inputTransform.m_Error;
  m_TimeStamp   = inputTransform.m_TimeStamp;
  return *this;
}


void 
Transform
::SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Rotation    = rotation;
  m_Error       = errorValue;
}


void 
Transform
::SetTranslation(
          const  VectorType & translation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Error       = errorValue;
}


void 
Transform
::SetRotation(
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Rotation = rotation;
  m_Error       = errorValue;
}


const Transform::VectorType &
Transform
::GetTranslation() const
{
  return m_Translation;
}



const Transform::VersorType &
Transform
::GetRotation() const
{
  return m_Rotation;
}


const Transform::ErrorType &
Transform
::GetError() const
{
  return m_Error;
}




Transform::TimePeriodType 
Transform
::GetStartTime() const
{
  return m_TimeStamp.GetStartTime();
}



Transform::TimePeriodType 
Transform
::GetExpirationTime() const
{
  return m_TimeStamp.GetExpirationTime();
}


bool
Transform
::IsValidAtTime( TimePeriodType timeToCheckInMilliseconds ) const
{
  return m_TimeStamp.IsValidAtTime( timeToCheckInMilliseconds );
}


void 
Transform
::ExportTransform( ::vtkMatrix4x4 & outmatrix )
{
  VersorType::MatrixType inmatrix = m_Rotation.GetMatrix();
 
  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      outmatrix.SetElement(i,j,inmatrix.GetVnlMatrix().get(i,j));   
      }

    outmatrix.SetElement(i,3,m_Translation[i]);
    }
}


bool
Transform
::operator==( const Transform & inputTransform )
{
  // 
  // FIXME Discuss how the == operator should be defined for the Transform.
  // 
  // In the meantime, let's go safe and consider two transforms equal
  // only if they are the same object in memory. It is arguable whether this
  // should be a method based on how close the numerical content of the two
  // transforms is, based on some tolerance.
  return (this==&inputTransform);
}



bool
Transform
::operator!=( const Transform & inputTransform )
{
  return !(this->operator==( inputTransform ));
}



void
Transform
::SetToIdentity( TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation.Fill( 0.0 );
  m_Rotation.SetIdentity();
  m_Error = itk::NumericTraits< ErrorType >::min();
}



} // end namespace itk


