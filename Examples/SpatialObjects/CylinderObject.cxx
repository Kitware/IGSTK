/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    CylinderObject.cxx
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
// \index{igstk::CylinderObject}
// The \doxygen{CylinderObject} represents a cylinder in space.
// 
// Let's start by including the appropriate header:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkCylinderObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First, we declare the cylinder using standard smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::CylinderObject CylinderObjectType;
  CylinderObjectType::Pointer cylinder = CylinderObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The \doxygen{CylinderObject} has two parameters, its radius and its height
// both expressed in $mm$.
// These two parameters can be set using \code{SetRadius()} and
// \code{SetHeight()}, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cylinder->SetRadius(10.0);
  cylinder->SetHeight(20.0);
// Software Guide : EndCodeSnippet

  cylinder->Print(std::cout);

  return EXIT_SUCCESS;
}
