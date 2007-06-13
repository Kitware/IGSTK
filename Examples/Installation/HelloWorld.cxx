/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    HelloWorld.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//  The following code is an implementation of a small igstk
//  program. It tests including header files and linking with IGSTK
//  library.
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkImageReader.h"
#include "igstkImageSpatialObject.h"

int main( int , char* [] )
{

  igstk::RealTimeClock::Initialize();

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< PixelType, Dimension > 
                                                 ImageSpatialObjectType;
  
  class myTestReader : 
    public igstk::ImageReader< ImageSpatialObjectType >
    {
  public:
    typedef myTestReader                                 Self;
    typedef igstk::ImageReader< ImageSpatialObjectType > Superclass;
    igstkNewMacro( Self );
  private:
    typedef Superclass::ImageType ImageType;
    virtual const ImageType * GetITKImage() const { return NULL; }
    };

  typedef myTestReader   ReaderType;

  ReaderType::Pointer  reader = ReaderType::New();

  std::cout << "IGSTK Hello !" << std::endl;
  
  return EXIT_SUCCESS;
}
