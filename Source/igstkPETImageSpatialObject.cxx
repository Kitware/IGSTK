/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPETImageSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPETImageSpatialObject.h"

namespace igstk
{ 

/** Constructor */
PETImageSpatialObject
::PETImageSpatialObject():m_StateMachine(this)
{

} 


/** Destructor */
PETImageSpatialObject
::~PETImageSpatialObject()
{

} 


/** Print Self function */
void 
PETImageSpatialObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
