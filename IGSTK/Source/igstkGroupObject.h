/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGroupObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkGroupObject_h
#define __igstkGroupObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"

namespace igstk
{

/** \class GroupObject
 * \brief Implements the 3-dimensional Group structure.
 *
 * \par Overview
 * GroupObject implements the 3-dimensional Group structure. This spatial
 * object keeps reference to its children, so it is possible to navigate the
 * scene graph downwards.
 *
 * \ingroup Object
 */

class GroupObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( GroupObject, SpatialObject )

public:

  /** Type of the internal GroupSpatialObject. */
  typedef itk::GroupSpatialObject<3>          GroupSpatialObjectType;

  /** Return the number of objects in the group */
  unsigned long GetNumberOfChildren() const;
  
  /** Request Adding a SpatialObject to the list of children.  Note that this
   * method invoke the reciprocal RequestSetTransformAndParent(transform,parent)
   * method on the child. */
  void RequestAddChild( const Transform & transform, SpatialObject * child );

  /** Request to get the child identified with a given childId number */
  void RequestGetChild( unsigned long childId );

protected:

  /** Constructor */
  GroupObject( void );

  /** Destructor */
  ~GroupObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkGroupSpatialObject */
  GroupSpatialObjectType::Pointer   m_GroupSpatialObject;

  typedef std::vector< SpatialObject::Pointer >  ChildrenContainerType;

  ChildrenContainerType             m_ChildrenArray;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( AddChildValid );
  igstkDeclareInputMacro( AddChildInvalid );
  igstkDeclareInputMacro( GetChildValid );
  igstkDeclareInputMacro( GetChildInvalid );

  /** States for the State Machine */
  igstkDeclareStateMacro( EmptyGroup );
  igstkDeclareStateMacro( NonEmptyGroup );

  /** Action methods to be invoked only by the state machine */
  void AddChildProcessing();
  void GetChildProcessing();
  void ReportNoChildAvailableProcessing();
  
  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Report when a request has been made at an incorrect time. */
  void ReportInvalidRequestProcessing();

  SpatialObject  *   m_ChildToAdd;
  Transform          m_TransformToAdd;
  unsigned long      m_ChildIdToGet;
};

} // end namespace igstk

#endif // __igstkGroupObject_h
