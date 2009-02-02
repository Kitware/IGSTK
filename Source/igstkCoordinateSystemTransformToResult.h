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
  void Initialize(const Transform& transform, 
                  const CoordinateSystem* source,
                  const CoordinateSystem* destination,
          const CoordinateSystem* commonAncestor);

  /** Sets the transform, source, and destination coordinate systems.
    * Overloaded Method.*/
  void Initialize(const Transform& transform, 
                  const CoordinateSystem* source,
                  const CoordinateSystem* destination);
          
  /** Sets the transform, source, and destination coordinate systems from two
   * given objects that respectively own coordinate systems. */
  template <class TSource, class TDestination>
  void Initialize(const Transform& transform, 
                  const TSource * sourceObject,
                  const TDestination * destinationObject )
    {
    const CoordinateSystem * source =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( sourceObject );

    const CoordinateSystem * destination =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( destinationObject );

    this->Initialize( transform, source, destination );
    }

  /** Returns the computed transform. */
  const Transform & GetTransform() const;

  /** Returns the source coordinate system. */
  const CoordinateSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateSystem * GetDestination() const;

  /** Returns the common ancestor coordinate system for Scene Graph. */
  const CoordinateSystem * GetCommonAncestor() const;
  
private:

  Transform                        m_Transform;
  const CoordinateSystem       *   m_Source;
  const CoordinateSystem       *   m_Destination;
  const CoordinateSystem       *   m_CommonAncestor;

};

/** This event is invoked when RequestComputeTransformTo is called
 *  successfully.
 */
igstkLoadedEventMacro( CoordinateSystemTransformToEvent, 
                  IGSTKEvent, CoordinateSystemTransformToResult );

} // end namespace igstk

#endif 
