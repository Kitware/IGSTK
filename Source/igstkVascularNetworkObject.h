/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVascularNetworkObject_h
#define __igstkVascularNetworkObject_h

#include "igstkMacros.h"
#include "igstkGroupObject.h"
#include "igstkVesselObject.h"

namespace igstk
{

/** \class TubeGroup
 * \brief Implements the 3-dimensional VascularNetworkObject structure.
 *
 * \par Overview
 * VascularNetworkObject implements the 3-dimensional 
 * VascularNetworkObject structure. 
 *
 *
 * \inTubeGroup Object
 */

class VascularNetworkObject 
: public GroupObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VascularNetworkObject, GroupObject )

public:
    
  /** Typedefs */
  typedef VesselObject                      VesselObjectType;

  /** Get the object */
  const VesselObjectType * GetVessel(unsigned long id) const;
  
protected:

  /** Constructor */
  VascularNetworkObject( void );

  /** Destructor */
  ~VascularNetworkObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:


};

} // end namespace igstk

#endif // __igstkTubeGroupObject_h
