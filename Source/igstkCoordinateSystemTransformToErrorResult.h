/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTransformToErrorResult.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystemTransformToErrorResult_h
#define __igstkCoordinateReferenceSystemTransformToErrorResult_h

#include "igstkCoordinateReferenceSystem.h"

namespace igstk
{

/** \class CoordinateReferenceSystemTransformToErrorResult
 *
 * \brief
 *
 */
class CoordinateReferenceSystemTransformToErrorResult
{
public:

  CoordinateReferenceSystemTransformToErrorResult()
    {

    }

  CoordinateReferenceSystemTransformToErrorResult(
      const CoordinateReferenceSystemTransformToErrorResult& in)
    {
    m_Source = in.m_Source;
    m_Destination = in.m_Destination;
    }

  CoordinateReferenceSystemTransformToErrorResult &operator = ( 
      const CoordinateReferenceSystemTransformToErrorResult& in)
    {
    if (this != &in)
      {
      m_Source = in.m_Source;
      m_Destination = in.m_Destination;
      }
    return *this;
    }

  inline void Initialize(const CoordinateReferenceSystem* src,
                  const CoordinateReferenceSystem* dst)
    {
    m_Source = src;
    m_Destination = dst;
    }

  CoordinateReferenceSystem::ConstPointer   m_Source;
  CoordinateReferenceSystem::ConstPointer   m_Destination;
};


//  
//   Macros defining events related to the 
//   CoordinateReferenceSystemTransformToErrorResult.
//
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToNullTargetEvent,
                       IGSTKEvent,
                       CoordinateReferenceSystemTransformToErrorResult );
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToDisconnectedEvent,
                       IGSTKEvent,
                       CoordinateReferenceSystemTransformToErrorResult );


} // end namespace igstk

#endif 
