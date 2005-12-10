/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCTImageSpatialObject.h"

namespace igstk
{ 

/** Constructor */
CTImageSpatialObject
::CTImageSpatialObject():m_StateMachine((Self *)this)
{

} 


/** Destructor */
CTImageSpatialObject
::~CTImageSpatialObject()
{

} 


/** Print Self function */
void 
CTImageSpatialObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

