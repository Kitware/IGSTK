/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    AxesObject.cxx
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
// \index{igstk::AxesObject}
// This example describes how to use the \doxygen{AxesObject}.
// This class defines a 3-dimensional coordinate system in space. 
// It is intended to provide a visual reference of the orientation 
// of space in the context of the scene.
//
// The \doxygen{AxesObject} is useful when creating complex objects
// to make sure that the orientation within the scene is consistent.
// It is also very informative for debugging purposes.
//
// First, we include the appropriate header file:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkAxesObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// We then declare the object using smart pointers, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::AxesObject AxesObjectType;
  AxesObjectType::Pointer axes = AxesObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The size of each axis can be set using the
// \code{SetSize()} function:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  double sizex = 10;
  double sizey = 20;
  double sizez = 30;
  axes->SetSize(sizex,sizey,sizez);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// In case we need to retrieve the length of the axes
// we can use the \code{GetSizeX()},\code{GetSizeY()} and \code{GetSizeZ()} 
// functions.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::cout << "SizeX is: " << axes->GetSizeX() << std::endl;
  std::cout << "SizeY is: " << axes->GetSizeY() << std::endl;
  std::cout << "SizeZ is: " << axes->GetSizeZ() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
