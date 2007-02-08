/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    DeckOfCardRobotRegistrationBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "DeckOfCardRobotRegistrationBase.h"

namespace igstk 
{

void DeckOfCardRobotRegistrationBase::
  PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Image: " << m_ITKImage << std::endl;
  os << indent << "Transform: " << m_Transform << std::endl;
}

} // end namespace igstk
