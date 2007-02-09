/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ConeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
// As the name of class indicates, the \doxygen{ConeObject} 
// represents a cone in space.
//
// First, we include the header file:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkConeObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// We then declare the cone using standard smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::ConeObject ConeObjectType;
  ConeObjectType::Pointer cone = ConeObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The \doxygen{ConeObject} has two internal parameters, its radius 
// and its height, both expressed in mm. The radius represents the radius of
// the base of the cone.
// These two parameters can be set using \code{SetRadius()} and
// \code{SetHeight()}, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  cone->SetRadius(10.0);
  cone->SetHeight(20.0);
// Software Guide : EndCodeSnippet

  cone->Print(std::cout);

  return EXIT_SUCCESS;
}
