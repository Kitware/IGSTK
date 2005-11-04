/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGroupObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

/** \class Group
 * \brief Implements the 3-dimensional Group structure.
 *
 * \par Overview
 * Group implements the 3-dimensional Group structure. 
 *
 *
 * \ingroup Object
 */

class GroupObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef GroupObject                               Self;
  typedef SpatialObject                             Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  /** Type of the internal GroupSpatialObject. */
  typedef itk::GroupSpatialObject<3>          GroupSpatialObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( GroupObject, SpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( GroupObject );

  /** Return the number of objects in the group */
  unsigned long GetNumberOfObjects() const;
  
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

};

} // end namespace igstk

#endif // __igstkGroupObject_h
