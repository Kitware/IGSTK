/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeGroupObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeGroupObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeGroupObject::TubeGroupObject():m_StateMachine(this)
{
}

/** Destructor */
TubeGroupObject::~TubeGroupObject()  
{
}

/** Return a pointer to a tube */
const TubeGroupObject::TubeObjectType * 
TubeGroupObject::GetTube(unsigned long itkNotUsed(id)) const
{
#ifdef USE_SPATIAL_OBJECT_DEPRECATED
  return dynamic_cast<const TubeObject*>(this->GetObject(id));
#else
  return NULL;
#endif
}

/** Print object information */
void TubeGroupObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
