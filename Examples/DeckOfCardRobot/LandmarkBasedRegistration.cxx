/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    LandmarkBasedRegistration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "LandmarkBasedRegistration.h"


LandmarkBasedRegistration::LandmarkBasedRegistration()
{
}

void LandmarkBasedRegistration::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool LandmarkBasedRegistration::Execute()
{   
  return true;
}
