/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemSetTransformResult.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateSystemSetTransformResult_h
#define __igstkCoordinateSystemSetTransformResult_h

#include "igstkCoordinateSystem.h"

namespace igstk
{

/**
 *  \class CoordinateSystemSetTransformResult
 *
 * \brief This class encapsulates the details of setting parent and
 * child relationships between coordinate systems.
 *
 * It is meant to be used as payload in an event that is created after a
 * successful call to RequestSetParentAndTransform(). 
 *
 *  \ingroup CoordinateSystem
 *
 */
class CoordinateSystemSetTransformResult
{
public:

  /** Constructor */
  CoordinateSystemSetTransformResult();

  /** Copy constructor */
  CoordinateSystemSetTransformResult(
      const CoordinateSystemSetTransformResult& in);

  /** Assignment operator */
  const CoordinateSystemSetTransformResult &operator = ( 
      const CoordinateSystemSetTransformResult& in);

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
          bool isAttaching);

  /** Sets the transform, source, and destination coordinate systems from two
   * given objects that respectively own coordinate systems. */
  template <class TSource, class TDestination>
  void Initialize(const Transform& transform, 
                  const TSource * sourceObject,
                  const TDestination * destinationObject ,
          bool isAttaching)
    {
    const CoordinateSystem * source =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( sourceObject );

    const CoordinateSystem * destination =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( destinationObject );

    this->Initialize( transform, source, destination , isAttaching);
    }

  /** Returns the computed transform. */
  const Transform & GetTransform() const;

  /** Returns the source coordinate system. */
  const CoordinateSystem * GetSource() const;

  /** Returns the destination coordinate system. */
  const CoordinateSystem * GetDestination() const;

  /**Returns whether this result is being used for attach or detach**/
  bool IsAttach() const;

private:

  Transform                        m_Transform;
  const CoordinateSystem       *   m_Source;
  const CoordinateSystem       *   m_Destination;
  bool                             m_IsAttach;

};

/** This event is invoked when RequestComputeTransformTo is called
 *  successfully.
 */
igstkLoadedEventMacro( CoordinateSystemSetTransformEvent, 
                  IGSTKEvent, CoordinateSystemSetTransformResult );

} // end namespace igstk

#endif 
