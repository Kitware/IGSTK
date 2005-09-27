/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectTest.cxx
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

#include "igstkCTImageSpatialObject.h"


int igstkCTImageSpatialObjectTest( int , char* [] )
{

  typedef igstk::CTImageSpatialObject         ImageSpatialObjectType;

  /* Instantiate one CT image */
  ImageSpatialObjectType::Pointer ctImage =  ImageSpatialObjectType::New();

  return EXIT_SUCCESS;
}

