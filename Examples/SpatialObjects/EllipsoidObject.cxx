/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    EllipsoidObject.cxx
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
// \index{igstk::EllipsoidObject}
// The \doxygen{EllipsoidObject} represents an ellipsoid in space. 
//
// Let's start by including the appropriate header:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkEllipsoidObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First, we declare the ellipsoid using standard smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::EllipsoidObject EllipsoidObjectType;
  EllipsoidObjectType::Pointer ellipsoid = EllipsoidObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The radius of the ellipse can be adjusted in each dimension 
// using the two \code{SetRadius()} functions.
// The easiest way is to use the 
// \code{SetRadius(double,double,double)} function, as follows:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  ellipsoid->SetRadius(10,20,30);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
// However, in some cases, the use of an array might be appropriate.
// The array is defined using standard type definition, then
// passed to the ellipsoid, as follows:
// Software Guide : BeginCodeSnippet
  typedef EllipsoidObjectType::ArrayType   ArrayType;
  ArrayType radii;
  radii[0] = 10;
  radii[1] = 20;
  radii[2] = 30;
  ellipsoid->SetRadius(radii);
// Software Guide : EndCodeSnippet

  ellipsoid->Print(std::cout);

  return EXIT_SUCCESS;
}
