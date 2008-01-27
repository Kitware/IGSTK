/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTransformToResult.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystemTransformToResult_h
#define __igstkCoordinateReferenceSystemTransformToResult_h

#include "igstkCoordinateReferenceSystem.h"

namespace igstk
{

/**
 *  \class CoordinateReferenceSystemTransformToResult
 *
 * \brief This class encapsulates the results of asking the coordinate
 * reference system for a transform to another coordinate reference system. 
 *
 * It is meant to be used as payload in an event that is created after a
 * successful call to RequestTransformTo(). 
 *
 *  \ingroup CoordinateReferenceSystem
 *
 */
class CoordinateReferenceSystemTransformToResult
{
public:

  CoordinateReferenceSystemTransformToResult()
    {
    m_Source = NULL;
    m_Destination = NULL;
    }

  CoordinateReferenceSystemTransformToResult(
      const CoordinateReferenceSystemTransformToResult& in)
    {
    m_Transform = in.m_Transform;
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    }

  const CoordinateReferenceSystemTransformToResult &operator = ( 
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

  inline void Clear()
    {
    m_Transform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
    m_Source = NULL;
    m_Destination = NULL;
    }

  inline void Initialize(const Transform& trans, 
                  const CoordinateReferenceSystem* src,
                  const CoordinateReferenceSystem* dst)
    {
    m_Transform = trans;
    m_Source = src;
    m_Destination = dst;
    }

  const Transform & GetTransform() const
    {
    return this->m_Transform;
    }
  const CoordinateReferenceSystem * GetSource() const
    {
    return this->m_Source;
    }
  const CoordinateReferenceSystem * GetDestination() const
    {
    return this->m_Destination;
    }

private:

  Transform                                 m_Transform;
  CoordinateReferenceSystem::ConstPointer   m_Source;
  CoordinateReferenceSystem::ConstPointer   m_Destination;

};

igstkLoadedEventMacro( CoordinateReferenceSystemTransformToEvent, 
                  IGSTKEvent, CoordinateReferenceSystemTransformToResult );

} // end namespace igstk

#endif 
