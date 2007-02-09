/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ModelImageGenerateTest.cxx
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
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "FiducialModel.h"

int main(int argc , char * argv [] )
{
  if ( argc < 2 ) 
    {
    std::cerr << "Usage" << argv[0] 
              << "Fiducal_output_image_name" << std::endl;
    return EXIT_FAILURE;
    } 
  typedef igstk::FiducialModel  FiducialModelType;

  FiducialModelType::Pointer  fiducialModel = FiducialModelType::New();
    
  typedef itk::Image<double, 3 >             ImageType; 
  ImageType::Pointer image = ImageType::New();

  fiducialModel->GenerateModelImage();

  image = fiducialModel->GetModelImage();

  typedef itk::ImageFileWriter< ImageType > ImageFileWriteType;

  ImageFileWriteType::Pointer   writer = ImageFileWriteType::New();

  writer->SetFileName ( argv[1] );
  writer->SetInput ( image ); 
  writer->Update();  

  return EXIT_SUCCESS;
}
