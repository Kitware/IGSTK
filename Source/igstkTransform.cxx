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
}


Transform
::~Transform()
{
}

void 
Transform
::SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Rotation    = rotation;
}


void 
Transform
::SetTranslation(
          const  VectorType & translation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
}


void 
Transform
::SetRotation(
          const  VersorType & rotation,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Rotation = rotation;
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



double 
Transform
::GetStartTime() const
{
  return m_TimeStamp.GetStartTime();
}



double 
Transform
::GetExpirationTime() const
{
  return m_TimeStamp.GetExpirationTime();
}


bool
Transform
::IsValidAtTime( double timeToCheckInMilliseconds ) const
{
  return m_TimeStamp.IsValidAtTime( timeToCheckInMilliseconds );
}


} // end namespace itk


