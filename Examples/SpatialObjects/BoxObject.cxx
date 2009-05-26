/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    BoxObject.cxx
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
// \index{igstk::BoxObject}
// The \doxygen{BoxObject} represents a hexahedron in space. It is a useful
// building block to create a more complex object (see the 3D ultrasound
// probe in Chapter~\ref{Chapter:SpatialObjectRepresentation:UltrasoundProbeRepresentation} for reference).
//
// Let's include the appropriate header:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkBoxObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First, we declare the box using standard smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::BoxObject BoxObjectType;
  BoxObjectType::Pointer box = BoxObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Then, we set the size of the box in each dimension by
// using the \code{SetSize()} function:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  double sizex = 10;
  double sizey = 20;
  double sizez = 30;
  box->SetSize(sizex,sizey,sizez);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// If one needs to retrieve the size of the box,
// they can do so using the \code{GetSize()} function:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::cout << "SizeX is: " << box->GetSizeX() << std::endl;
  std::cout << "SizeY is: " << box->GetSizeY() << std::endl;
  std::cout << "SizeZ is: " << box->GetSizeZ() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
