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
CoordinateSystemTransformToResult
::CoordinateSystemTransformToResult()
{
  m_Source = NULL;
  m_Destination = NULL;
  m_CommonAncestor = NULL;
}

CoordinateSystemTransformToResult
::CoordinateSystemTransformToResult(
      const CoordinateSystemTransformToResult& in)
{
  m_Transform = in.m_Transform;
  m_Source = in.m_Source;
  m_Destination = in.m_Destination;
  m_CommonAncestor = in.m_CommonAncestor;
}

const CoordinateSystemTransformToResult &
CoordinateSystemTransformToResult
::operator = ( 
      const CoordinateSystemTransformToResult& in)
{
  if (this != &in)
    {
    m_Transform = in.m_Transform;
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    m_CommonAncestor = in.m_CommonAncestor;
    }
  return *this;
}

void
CoordinateSystemTransformToResult
::Clear()
{
  m_Transform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  m_Source = NULL;
  m_Destination = NULL;
  m_CommonAncestor = NULL;
}

void
CoordinateSystemTransformToResult
::Initialize(const Transform& trans, 
             const CoordinateSystem* src,
             const CoordinateSystem* dst,
       const CoordinateSystem* ancestor)
{
  m_Transform = trans;
  m_Source = src;
  m_Destination = dst;
  m_CommonAncestor = ancestor;
}

void
CoordinateSystemTransformToResult
::Initialize(const Transform& trans, 
             const CoordinateSystem* src,
             const CoordinateSystem* dst)
{
  m_Transform = trans;
  m_Source = src;
  m_Destination = dst;
}

const Transform & 
CoordinateSystemTransformToResult
::GetTransform() const
{
  return this->m_Transform;
}

const CoordinateSystem * 
CoordinateSystemTransformToResult
::GetSource() const
{
  return this->m_Source;
}

const CoordinateSystem *
CoordinateSystemTransformToResult
::GetDestination() const
{
  return this->m_Destination;
}

const CoordinateSystem *
CoordinateSystemTransformToResult
::GetCommonAncestor() const
{
  return this->m_CommonAncestor;
}

} // end namespace igstk
