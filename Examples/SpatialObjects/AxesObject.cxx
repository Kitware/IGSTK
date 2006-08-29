/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    AxesObject.cxx
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
// \index{igstk::AxesObject}
// This example describes how to use the \doxygen{AxesObject}.
// This class display a coordinate axis in a View. It is intended for providing
// a visual reference of the orientation of space in the context of the scene.
// First we include the appropriate header file.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkAxesObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// First we declare the object using smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::AxesObject AxesObjectType;
  AxesObjectType::Pointer axes = AxesObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Then we can set the size of each axis using the
// SetSize() function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  double sizex = 10;
  double sizey = 20;
  double sizez = 30;
  axes->SetSize(sizex,sizey,sizez);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// In case, we need to retreive the length of the axes
// we can use the GetSize() functions.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::cout << "SizeX is: " << axes->GetSizeX() << std::endl;
  std::cout << "SizeY is: " << axes->GetSizeY() << std::endl;
  std::cout << "SizeZ is: " << axes->GetSizeZ() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
