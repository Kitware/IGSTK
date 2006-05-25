/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObjectTest.cxx
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

#include "igstkUSImageObject.h"


int igstkUSImageObjectTest( int , char* [] )
{
  igstk::RealTimeClock::Initialize();
  typedef igstk::USImageObject     ImageSpatialObjectType;

  // Instantiate one US image
  ImageSpatialObjectType::Pointer usImage =  ImageSpatialObjectType::New();

  usImage->Print( std::cout );

  return EXIT_SUCCESS;
}
