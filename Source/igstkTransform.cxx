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
          double tx, double ty, double tz,
          double qx, double qy, double qz, double qw,
          double millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation[0] = tx;
  m_Translation[1] = ty;
  m_Translation[2] = tz;
  m_Rotation.Set( qx, qy, qz, qw );
}


} // end namespace itk


