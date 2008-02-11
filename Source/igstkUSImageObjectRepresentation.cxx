/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkUSImageObjectRepresentation.h"

namespace igstk
{ 

/** Constructor */
USImageObjectRepresentation
::USImageObjectRepresentation():m_StateMachine(this)
{

}

/** Destructor */
USImageObjectRepresentation
::~USImageObjectRepresentation()
{

}

/** Method that clones this object */
USImageObjectRepresentation::Pointer
USImageObjectRepresentation
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::USImageObjectRepresentation\
                        ::Copy called...\n");

  Pointer newOR = Self::New();
  newOR->SetColor( this->GetRed(), this->GetGreen(), this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );

  return newOR;
}

/** Print Self function */
void USImageObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
