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

#ifndef __igstkCoordinateSystemTransformToResult_h
#define __igstkCoordinateSystemTransformToResult_h

#include "igstkCoordinateSystem.h"

namespace igstk
{

/**
 *  \class CoordinateSystemTransformToResult
 *
 * \brief This class encapsulates the results of asking the coordinate
 * reference system for a transform to another coordinate reference system. 
 *
 * It is meant to be used as payload in an event that is created after a
 * successful call to RequestTransformTo(). 
 *
 *  \ingroup CoordinateSystem
 *
 */
class CoordinateSystemTransformToResult
{
public:

  /** Constructor */
  CoordinateSystemTransformToResult();

  /** Copy constructor */
  CoordinateSystemTransformToResult(
      const CoordinateSystemTransformToResult& in);

  /** Assignment operator */
  const CoordinateSystemTransformToResult &operator = ( 
      const CoordinateSystemTransformToResult& in);

  /** Clears the pointers that the event is holding. This 
   *  should be called after the event is received to
   *  remove unnecessary smart pointer references to 
   *  coordinate systems.
   */
  void Clear();

  /** Sets the transform, source, and destination coordinate systems. */
  void Initialize(const Transform& trans, 
                  const CoordinateSystem* src,
                  const CoordinateSystem* dst);

  /** Returns the computed transform. */
  const Transform & GetTransform() const;

  /** Returns the source coordinate system. */
  const CoordinateSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateSystem * GetDestination() const;

private:

  Transform                        m_Transform;
  const CoordinateSystem       *   m_Source;
  const CoordinateSystem       *   m_Destination;

};

/** This event is invoked when RequestComputeTransformTo is called
 *  successfully.
 */
igstkLoadedEventMacro( CoordinateSystemTransformToEvent, 
                  IGSTKEvent, CoordinateSystemTransformToResult );

} // end namespace igstk

#endif 
