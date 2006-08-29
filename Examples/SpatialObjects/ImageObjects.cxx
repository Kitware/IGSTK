/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ImageObjects.cxx
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
// \index{igstk::CTImageSpatialObject}
// In this example we show the main functionalities of the igstk::ImageObject classes.
// IGSTK defines different classes for each modality, CT, MR and US.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkCTImageSpatialObject.h"
#include "igstkMRImageSpatialObject.h"
#include "igstkUSImageObject.h"
// Software Guide : EndCodeSnippet

int main( int argc, char * argv[] )
{
// Software Guide : BeginLatex
//
// First we declare an empty CT image using smart pointers
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::CTImageSpatialObject CTImageSpatialObject;
  CTImageSpatialObject::Pointer ctImage = CTImageSpatialObject ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// For a given point in physical space, we can ask if this particular
// point is inside the image or not using the IsInside() function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef CTImageSpatialObject::PointType PointType;
  PointType pt;
  pt[0] = 10; 
  pt[1] = 10;
  pt[2] = 10;
  if(ctImage->IsInside(pt))
    {
    std::cout << "The point " << pt 
              << " is inside the image" << std::endl;
    }
  else
    {
    std::cout << "The point " << pt 
              << " is outside the image" << std::endl;
    }
// Software Guide : EndCodeSnippet
 
// Software Guide : BeginLatex
//
// If the point is inside the image, we can convert the physical point
// into and index or continuous index point in the image reference frame.
// This is achieve using the TransformPhysicalPointToIndex() and
// TransformPhysicalPointToContinuousIndex() functions.
//
// Software Guide : EndLatex  
  
// Software Guide : BeginCodeSnippet
 if(ctImage->IsInside(pt))
   {
   typedef CTImageSpatialObject::IndexType IndexType;
   IndexType index;
   ctImage->TransformPhysicalPointToIndex (pt , index );

   std::cout << "Index is = " << index << std::endl;
    
   typedef CTImageSpatialObject::ContinuousIndexType ContinuousIndexType;
   ContinuousIndexType cindex;
   ctImage->TransformPhysicalPointToContinuousIndex (pt, cindex);
   std::cout << "Continuous index is = " << cindex << std::endl;
   }
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We can check if the image is empty by using the IsEmpty()
// function.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  if(ctImage->IsEmpty())
    {
    std::cout << "The image is empty" << std::endl;
    }
  else
    {
    std::cout << "The image has some non black pixel" << std::endl;
    }
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
