/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeGroupObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTubeGroupObject_h
#define __igstkTubeGroupObject_h

#include "igstkMacros.h"
#include "igstkGroupObject.h"
#include "igstkTubeObject.h"

namespace igstk
{

/** \class TubeGroup
 * \brief Implements the 3-dimensional TubeGroup structure.
 *
 * \par Overview
 * TubeGroup implements the 3-dimensional TubeGroup structure. 
 *
 *
 * \inTubeGroup Object
 */

class TubeGroupObject 
: public GroupObject
{

public:

  /** Typedefs */
  typedef TubeGroupObject                           Self;
  typedef GroupObject                               Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;
  typedef TubeObject                                TubeObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( TubeGroupObject, SpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( TubeGroupObject );

  /** Get the object */
  const TubeObjectType * GetTube(unsigned long id) const;
  
protected:

  /** Constructor */
  TubeGroupObject( void );

  /** Destructor */
  ~TubeGroupObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:


};

} // end namespace igstk

#endif // __igstkTubeGroupObject_h
