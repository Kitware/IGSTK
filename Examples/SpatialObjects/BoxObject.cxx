/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    BoxObject.cxx
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
// \index{igstk::BoxObject}
// The \doxygen{BoxObject} represents a box in space.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkBoxObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First we declare the box using standard smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::BoxObject BoxObjectType;
  BoxObjectType::Pointer box = BoxObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Then we can set the size of the box in each dimension by
// using the SetSize() function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  double sizex = 10;
  double sizey = 20;
  double sizez = 30;
  box->SetSize(sizex,sizey,sizez);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// In case, we need to retreive the size of the box
// we can use the GetSize() functions.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::cout << "SizeX is: " << box->GetSizeX() << std::endl;
  std::cout << "SizeY is: " << box->GetSizeY() << std::endl;
  std::cout << "SizeZ is: " << box->GetSizeZ() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
