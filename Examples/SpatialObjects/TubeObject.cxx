/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    TubeObject.cxx
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
// \index{igstk::TubeObject}
// This example describes how to use the \doxygen{TubeObject}, which implements
// a 3-dimensional tubular structure in space.
// The tube is defined by a set of points representing its centerline.
// Each point has a position and an associated radius value.
//
// Let's start by including the appropriate header file:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkTubeObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// First, we declare the object using smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::TubeObject TubeObjectType;
  TubeObjectType::Pointer tube = TubeObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Points can be added sequentially in the tube using
// the \code{AddPoint()} function.
// Let's add two points - one at position (0,1,2) with a radius of $10mm$,
// and second one at (1,2,3) with a radius of $20mm$:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef TubeObjectType::PointType PointType;
  PointType pt;
  pt.SetPosition(0,1,2);
  pt.SetRadius(10);
  tube->AddPoint(pt);

  pt.SetPosition(1,2,3);
  pt.SetRadius(20);
  tube->AddPoint(pt);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Then, we can use the \code{GetNumberOfPoints()} function to
// get the number of points composing the tube.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  std::cout << "Number of points in the tube = "
            << tube->GetNumberOfPoints() << std::endl;
// Software Guide : EndCodeSnippet
  std::cout << std::endl;
// Software Guide : BeginLatex
//
// There are two main functions to get points from the tube.
// The first one is \code{GetPoint(unsigned int id)}, which returns
// a pointer to the corresponding point. 
// Note that if the index does not exist, the function returns
// a null pointer. The command is as follows:
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  const PointType* outPt = tube->GetPoint(0);
  outPt->Print(std::cout);
// Software Guide : EndCodeSnippet
  std::cout << std::endl;
// Software Guide : BeginLatex
//
// Instead, the second \code{GetPoints()} function should be 
// used. It is safer because it returns the internal list of points, as follows:
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  typedef TubeObjectType::PointListType PointListType;
  PointListType points = tube->GetPoints();
  PointListType::const_iterator it = points.begin();
  while(it != points.end())
    {
    (*it).Print(std::cout);
    std::cout << std::endl;
    it++;
    }
// Software Guide : EndCodeSnippet
   
// Software Guide : BeginLatex
//
// The \code{Clear()} function can be used to remove all 
// points from the tube:
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  tube->Clear();
  std::cout << "Number of points in the tube after Clear() = "
            << tube->GetNumberOfPoints() << std::endl;
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
