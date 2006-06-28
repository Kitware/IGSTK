/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ModelImageGenerateTest.cxx
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
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "itkImageFileWriter.h"
#include "FiducialModel.h"

int main(int argc , char * argv [] )
{
  typedef FiducialModel  FiducialModelType;

  FiducialModelType::Pointer  fiducialModel = FiducialModelType::New();
    
  typedef igstk::CTImageSpatialObject::ImageType            ImageType; 
  ImageType::Pointer image = ImageType::New();

  fiducialModel->GenerateModelImage();

  image = fiducialModel->GetModelImage();

  typedef itk::ImageFileWriter< ImageType > ImageFileWriteType;

  ImageFileWriteType::Pointer   writer = ImageFileWriteType::New();

  writer->SetFileName ( argv[1] ) ;
  writer->SetInput ( image ); 
  writer->Update();  

  return EXIT_SUCCESS;
}

