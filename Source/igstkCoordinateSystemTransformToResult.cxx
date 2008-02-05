/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTransformToResult.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateSystemTransformToResult.h"

namespace igstk
{
CoordinateReferenceSystemTransformToResult
::CoordinateReferenceSystemTransformToResult()
{
  m_Source = NULL;
  m_Destination = NULL;
}

CoordinateReferenceSystemTransformToResult
::CoordinateReferenceSystemTransformToResult(
      const CoordinateReferenceSystemTransformToResult& in)
{
  m_Transform = in.m_Transform;
  m_Source = in.m_Source;
  m_Destination = in.m_Destination;
}

const CoordinateReferenceSystemTransformToResult &
CoordinateReferenceSystemTransformToResult
::operator = ( 
      const CoordinateReferenceSystemTransformToResult& in)
{
  if (this != &in)
    {
    m_Transform = in.m_Transform;
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    }
  return *this;
}

void
CoordinateReferenceSystemTransformToResult
::Clear()
{
  m_Transform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  m_Source = NULL;
  m_Destination = NULL;
}

void
CoordinateReferenceSystemTransformToResult
::Initialize(const Transform& trans, 
             const CoordinateReferenceSystem* src,
             const CoordinateReferenceSystem* dst)
{
  m_Transform = trans;
  m_Source = src;
  m_Destination = dst;
}

const Transform & 
CoordinateReferenceSystemTransformToResult
::GetTransform() const
{
  return this->m_Transform;
}

const CoordinateReferenceSystem * 
CoordinateReferenceSystemTransformToResult
::GetSource() const
{
  return this->m_Source;
}

const CoordinateReferenceSystem *
CoordinateReferenceSystemTransformToResult
::GetDestination() const
{
  return this->m_Destination;
}

} // end namespace igstk
