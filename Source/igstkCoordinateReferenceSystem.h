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

  /** Returns the unique identifier of this node. 
   *  This method should not be in the API. Instead
   *  a RequestGetIdentifier() should be available.
   */
  IdentifierType GetIdentifier() const;

  /** Returns the unique identifier of the root node in the scene graph to
   * which this node may be attached. If the node is not attached to a parent,
   * the identifier returned here will be equal to the one returned by
   * GetIdentifier(). 
   *
   * THIS METHOD SHOULD BE DEPRECATED. There is no explicit root in the 
   * new design.
   */
  // IdentifierType GetRootIdentifier() const;

  /** Connects a coordinate system as a child of another coordinate system,
   * given a transform. This call is used for constructing a scene graph.
   *
   * This method should be private and a RequestSetTransformAndParent 
   * available instead.
   *
   **/
  void SetTransformAndParent( const Transform & transform, const CoordinateReferenceSystem * parent );

  /** Computes and returns the transform that relates this node to the root node
   *  of the scene graph 
   *
   * THIS METHOD SHOULD BE DEPRECATED. There is no explicit root in the 
   * new design.
   */
  //const Transform & ComputeTransformToRoot() const;

  /** Returns a pointer to the parent.
   *  This method should be private and a RequestGetParent() 
   *  should be the public API.
   */
  const CoordinateReferenceSystem* GetParent() const
    {
    return this->m_Parent;
    }

  /** DEVELOPMENT ONLY
   *  Allows a char* name to be set on the coordinate system.
   */
  void SetName(const char* name);

  /** DEVELOPMENT ONLY  
   *  Returns the name of the coordinate system.
   */
  const char* GetName() const
    {
    return m_Name;
    }

  /** This method finds the ancestor between two coordinate reference systems
   *  which is lowest in the graph (directed, acyclic, hopefully a tree) 
   *  relating CoordinateReferenceSystems. Note that this method is static.
   *
   *  The current implementation returns a NULL pointer if there is no common
   *  ancestor. This probably should be changed to throw an exception, or 
   *  if we use a local state machine and make the public API a Request based
   *  method, we could handle the error condition in an error state.
   *
   */
  static const CoordinateReferenceSystem*  
    GetLowestCommonAncestor(const CoordinateReferenceSystem* A, const CoordinateReferenceSystem* B);

  /** This method computes the transform between two 
   *  CoordinateReferenceSystems. Instead of being in the public API,
   *  this method should be private and a request based method should
   *  be available in its place.
   *
   *  GetTransformBetween computes the transformation between 
   *  arbitrary coordinate systems. If the destination coordinate
   *  system is not reachable from the source, the returned transform
   *  will have a valid time that has already expired.
   */ 
  static Transform 
    GetTransformBetween(const CoordinateReferenceSystem* source, const CoordinateReferenceSystem* destination);

  /** This method computes the transform between the current 
   *  CoordinateReferenceSystem and an ancestor 
   *  CoordinateReferenceSystem. In other words, this method
   *  can only compute a transformation to a coordinate system
   *  that is reachable by following links through parents.
   *
   *  If the transform to an arbitrary coordinate system is
   *  needed, use GetTransformBetween(). 
   */
  Transform ComputeTransformTo(const CoordinateReferenceSystem* ancestor) const;

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
  // mutable Transform   m_TransformToRoot;      // Transform relating this node to root node
  

  char*               m_Name; // DEBUG/DEVEL ONLY
};


namespace Friends 
{

class CoordinateReferenceSystemHelper
{
public:
  template <class T>
  static const CoordinateReferenceSystem & GetCoordinateReferenceSystem( const T * input )
    {
    return input.GetCoordinateReferenceSystem();  // private
    }
};

} // end of Friends namespace 

} // end namespace igstk

#endif // __igstkCoordinateReferenceSystem_h
