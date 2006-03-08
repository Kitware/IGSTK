/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVascularNetworkObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
VascularNetworkObject::VascularNetworkObject():m_StateMachine(this)
{
}

/** Destructor */
VascularNetworkObject::~VascularNetworkObject()  
{
}

/** Return a pointer to a tube */
const VascularNetworkObject::VesselObjectType * 
VascularNetworkObject::GetVessel(unsigned long id) const
{
  return dynamic_cast<const VesselObjectType*>(this->GetObject(id));
}

/** Print object information */
void VascularNetworkObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

