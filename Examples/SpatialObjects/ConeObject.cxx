/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ConeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
//
// \index{igstk::ConeObject}
// The \doxygen{ConeObject} represents a Cone in space.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkConeObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First we declare the Cone using standard smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::ConeObject ConeObjectType;
  ConeObjectType::Pointer cone = ConeObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The cone has two parameters, the radius and the height.
// These two parameters can be set using SetRadius() and
// SetHeight() respectively.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cone->SetRadius(10);
  cone->SetHeight(20);
// Software Guide : EndCodeSnippet

  cone->Print(std::cout);

  return EXIT_SUCCESS;
}
