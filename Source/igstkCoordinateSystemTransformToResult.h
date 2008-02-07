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

  /** Constructor */
  CoordinateReferenceSystemTransformToResult();

  /** Copy constructor */
  CoordinateReferenceSystemTransformToResult(
      const CoordinateReferenceSystemTransformToResult& in);

  /** Assignment operator */
  const CoordinateReferenceSystemTransformToResult &operator = ( 
      const CoordinateReferenceSystemTransformToResult& in);

  /** Clears the pointers that the event is holding. This 
   *  should be called after the event is received to
   *  remove unnecessary smart pointer references to 
   *  coordinate systems.
   */
  void Clear();

  /** Sets the transform, source, and destination coordinate systems. */
  void Initialize(const Transform& trans, 
                  const CoordinateReferenceSystem* src,
                  const CoordinateReferenceSystem* dst);

  /** Returns the computed transform. */
  const Transform & GetTransform() const;

  /** Returns the source coordinate system. */
  const CoordinateReferenceSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateReferenceSystem * GetDestination() const;

private:

  Transform                                 m_Transform;
  const CoordinateReferenceSystem       *   m_Source;
  const CoordinateReferenceSystem       *   m_Destination;

};

/** This event is invoked when RequestComputeTransformTo is called
 *  successfully.
 */
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToEvent, 
                  IGSTKEvent, CoordinateReferenceSystemTransformToResult );

} // end namespace igstk

#endif 
