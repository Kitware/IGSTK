/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ImageObjects.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
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
// In this example we show the main features of the ImageObject classes.
// IGSTK implements one class per modality for CT, MRI, and ultrasound, as 
// follows:
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
// First, we declare an empty CT image using smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::CTImageSpatialObject CTImageSpatialObject;
  CTImageSpatialObject::Pointer ctImage = CTImageSpatialObject ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Then, for a given point in physical space, we can ask if this particular
// point is inside (or outside) the image using the \code{IsInside()} function:
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
// into an index or a continuous index in the image reference frame.
// This is achieved using the \code{TransformPhysicalPointToIndex()} and
// \code{TransformPhysicalPointToContinuousIndex()} functions, as follows:
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
// We can also check if the image is empty by using the \code{IsEmpty()}
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
