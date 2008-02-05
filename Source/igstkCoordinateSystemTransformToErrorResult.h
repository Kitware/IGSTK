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
 * \brief This class is used a result of RequestComputeTransformTo
 * when there is an error. It encapsulates information about the 
 * source and destination coordinate systems.
 *
 * It is meant to be used as payload in an event that is created after a
 * error during a call to RequestTransformTo(). 
 *
 *  \ingroup CoordinateReferenceSystem
 *
 */
class CoordinateReferenceSystemTransformToErrorResult
{
public:

  CoordinateReferenceSystemTransformToErrorResult();

  CoordinateReferenceSystemTransformToErrorResult(
      const CoordinateReferenceSystemTransformToErrorResult& in);

  CoordinateReferenceSystemTransformToErrorResult &operator = ( 
      const CoordinateReferenceSystemTransformToErrorResult& in);

  void Initialize(const CoordinateReferenceSystem* src,
                  const CoordinateReferenceSystem* dst);

  /** Clears the pointers that the event is holding. This 
   *  should be called after the event is received to
   *  remove unnecessary smart pointer references to 
   *  coordinate systems.
   */
  void Clear();

  /** Returns the source coordinate system. */
  const CoordinateReferenceSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateReferenceSystem * GetDestination() const;

private:

  CoordinateReferenceSystem::ConstPointer   m_Source;
  CoordinateReferenceSystem::ConstPointer   m_Destination;
};

/** This event is invoked when RequestComputeTransformTo is called with
 *  a NULL target coordinate system.
 */
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToNullTargetEvent,
                       TransformNotAvailableEvent,
                       CoordinateReferenceSystemTransformToErrorResult );

/** This event is invoked when RequestComputeTransformTo is called with
 *  a destination coordinate system that is not reachable, i.e. not connected,
 *  to the source coordinate system.
 */
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToDisconnectedEvent,
                       TransformNotAvailableEvent,
                       CoordinateReferenceSystemTransformToErrorResult );

} // end namespace igstk

#endif 
