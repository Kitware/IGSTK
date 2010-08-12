/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PETDicomImageReader.cxx
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

#include "itkPETDicomImageReader.h"

int main( int argc, char* argv[] )
{
  
  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef signed short PixelType;
  typedef itk::Image< PixelType, 3>                  ImageType;  
  typedef itk::PETDicomImageReader<ImageType>        ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();

  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  
  reader->SetDirectory( directoryName );
  reader->ReadImage();

  return EXIT_SUCCESS;
}
