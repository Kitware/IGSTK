/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPETImageSpatialObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPETImageSpatialObjectRepresentation.h"

namespace igstk
{ 

/** Constructor */
PETImageSpatialObjectRepresentation
::PETImageSpatialObjectRepresentation():m_StateMachine(this)
{

} 

/** Method that clones this object */
PETImageSpatialObjectRepresentation::Pointer
PETImageSpatialObjectRepresentation
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::PETImageSpatialObjectRepresentation\
                        ::Copy called...\n");

  Pointer newOR = Self::New();
  newOR->SetColor( this->GetRed(), this->GetGreen(), this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );

  return newOR;
}

/** Print Self function */
void PETImageSpatialObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
