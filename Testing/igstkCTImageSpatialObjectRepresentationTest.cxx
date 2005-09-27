/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectRepresentationTest.cxx
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

#include "igstkCTImageSpatialObjectRepresentation.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkCTImageSpatialObjectRepresentationTest( int , char* [] )
{


  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::CTImageSpatialObjectRepresentation    RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();


  return EXIT_SUCCESS;
}

