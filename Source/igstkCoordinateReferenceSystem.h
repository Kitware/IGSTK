/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystem.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystem_h
#define __igstkCoordinateReferenceSystem_h

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"

namespace igstk
{

/** \class CoordinateReferenceSystem
 * 
 * \brief This class represents the frame of a coordinate reference system.
 *
 * The class is intended to be used as an anchor relative to which you can 
 * position Spatial Objects in the graph scene. 
 *
 *  
 * \ingroup Object
 */

class CoordinateReferenceSystem : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateReferenceSystem, Object )

  /** Type used to uniquely identify every coordinate system */
  typedef Token::IdentifierType    IdentifierType;

  /** Returns the unique identifier of this node. */
  IdentifierType GetIdentifier() const;

  /** Returns the unique identifier of the root node in the scene graph to
   * which this node may be attached. If the node is not attached to a parent,
   * the identifier returned here will be equal to the one regurned by
   * GetIdentifier(). */
  IdentifierType GetRootIdentifier() const;

  /** Connects a coordinate system as a child of another coordinate system,
   * given a transform. This call is used for constructing a scene graph.
   **/
  void SetTransformAndParent( const Transform & transform, const Self * object );

  /** Computes and returns the transform that relates this node to the root node
   *  of the scene graph 
   */
  const Transform & ComputeTransformToRoot() const;

protected:

  CoordinateReferenceSystem( void );
  ~CoordinateReferenceSystem( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  CoordinateReferenceSystem(const Self&);   //purposely not implemented
  void operator=(const Self&);              //purposely not implemented

  Token               m_Token;                // provides a unique identifier
  Self::ConstPointer  m_Parent;               // Parent node in the scene graph
  Transform           m_TransformToParent;    // Transform relating this node to the parent
  mutable Transform   m_TransformToRoot;      // Transform relating this node to root node

};

} // end namespace igstk

#endif // __igstkCoordinateReferenceSystem_h
