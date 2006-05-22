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

#include "igstkGroupObject.h"
#include "igstkVesselObject.h"

namespace igstk
{

/** \class VascularNetworkObject
 * \brief Implements the 3-dimensional vascular network structure.
 *
 * \ingroup GroupObject
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

  /** These two methods must be declared and note be implemented
  *  in order to enforce the protocol of smart pointers. */
  VascularNetworkObject(const Self&);          //purposely not implemented
  void operator=(const Self&);                 //purposely not implemented


};

} // end namespace igstk

#endif // __igstkTubeGroupObject_h
