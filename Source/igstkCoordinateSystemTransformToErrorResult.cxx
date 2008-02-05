/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTransformToErrorResult.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "igstkCoordinateSystemTransformToErrorResult.h"

namespace igstk
{
CoordinateReferenceSystemTransformToErrorResult
::CoordinateReferenceSystemTransformToErrorResult()
{

}

CoordinateReferenceSystemTransformToErrorResult
::CoordinateReferenceSystemTransformToErrorResult(
      const CoordinateReferenceSystemTransformToErrorResult& in)
{
  m_Source = in.m_Source;
  m_Destination = in.m_Destination;
}

CoordinateReferenceSystemTransformToErrorResult&
CoordinateReferenceSystemTransformToErrorResult
::operator = ( 
      const CoordinateReferenceSystemTransformToErrorResult& in)
{
  if (this != &in)
    {
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    }
  return *this;
}

void
CoordinateReferenceSystemTransformToErrorResult
::Initialize(const CoordinateReferenceSystem* src,
             const CoordinateReferenceSystem* dst)
{
  m_Source = src;
  m_Destination = dst;
}

const CoordinateReferenceSystem * 
CoordinateReferenceSystemTransformToErrorResult
::GetSource() const
{
  return this->m_Source;
}

const CoordinateReferenceSystem *
CoordinateReferenceSystemTransformToErrorResult
::GetDestination() const
{
  return this->m_Destination;
}

void
CoordinateReferenceSystemTransformToErrorResult
::Clear()
{
  m_Source = NULL;
  m_Destination = NULL;
}

} // end namespace igstk
