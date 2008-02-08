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

#ifndef __igstkCoordinateSystemTransformToErrorResult_h
#define __igstkCoordinateSystemTransformToErrorResult_h

#include "igstkCoordinateSystem.h"

namespace igstk
{

/** \class CoordinateSystemTransformToErrorResult
 *
 * \brief This class is used a result of RequestComputeTransformTo
 * when there is an error. It encapsulates information about the 
 * source and destination coordinate systems.
 *
 * It is meant to be used as payload in an event that is created after a
 * error during a call to RequestTransformTo(). 
 *
 *  \ingroup CoordinateSystem
 *
 */
class CoordinateSystemTransformToErrorResult
{
public:

  CoordinateSystemTransformToErrorResult();

  CoordinateSystemTransformToErrorResult(
      const CoordinateSystemTransformToErrorResult& in);

  CoordinateSystemTransformToErrorResult &operator = ( 
      const CoordinateSystemTransformToErrorResult& in);

  void Initialize(const CoordinateSystem* src,
                  const CoordinateSystem* dst);

  /** Clears the pointers that the event is holding. This 
   *  should be called after the event is received to
   *  remove unnecessary smart pointer references to 
   *  coordinate systems.
   */
  void Clear();

  /** Returns the source coordinate system. */
  const CoordinateSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateSystem * GetDestination() const;

private:

  CoordinateSystem::ConstPointer   m_Source;
  CoordinateSystem::ConstPointer   m_Destination;
};

/** This event is invoked when RequestComputeTransformTo is called with
 *  a NULL target coordinate system.
 */
igstkLoadedEventMacro( CoordinateSystemTransformToNullTargetEvent,
                       TransformNotAvailableEvent,
                       CoordinateSystemTransformToErrorResult );

/** This event is invoked when RequestComputeTransformTo is called with
 *  a destination coordinate system that is not reachable, i.e. not connected,
 *  to the source coordinate system.
 */
igstkLoadedEventMacro( CoordinateSystemTransformToDisconnectedEvent,
                       TransformNotAvailableEvent,
                       CoordinateSystemTransformToErrorResult );

} // end namespace igstk

#endif 
