/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IdentifyPETImageVolme.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageFileReader.h"
#include "itkGDCMImageIO.h"
#include "itkImageIOBase.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"

#include "gdcmGlobal.h"

#include <itksys/SystemTools.hxx>

#if GDCM_MAJOR_VERSION < 2
#include "gdcmDictSet.h"
#endif

// Software Guide : BeginLatex
// Software Guide : EndLatex

int main( int argc, char* argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << argv[0] << " DicomFile\t GateNumber\t OutpuDirectory" << std::endl;
    return EXIT_FAILURE;
    }

  // Software Guide : BeginLatex
  // 
  //  We instantiate the type to be used for storing the image once it is read
  //  into memory.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef signed short       PixelType;
  const unsigned int         Dimension = 2;
  
  typedef itk::Image< PixelType, Dimension >      ImageType;
  typedef itk::ImageFileReader< ImageType >     ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  typedef itk::GDCMImageIO       ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  // 
  // Here we override the gdcm default value of 0xfff with a value of 0xffff
  // to allow the loading of long binary stream in the DICOM file.
  // This is particularly useful when reading the private tag: 0029,1010
  // from Siemens as it allows to completely specify the imaging parameters
  dicomIO->SetMaxSizeLoadEntry(0xffff);
  reader->SetFileName( argv[1] );
  reader->SetImageIO( dicomIO );
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  unsigned int gateNumber = atoi( argv[2] );
  std::string directoryName = argv[3];

  std::string tagkey = "0054|1330";
  std::string labelId;
  std::string value;
  if( itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId ) )
    {
    std::cout << labelId << " (" << tagkey << "): ";
    if( dicomIO->GetValueFromTag(tagkey, value) )
      {
      std::cout << value;
      }
    else
      {
      std::cout << "(No Value Found in File)";
      }
    std::cout << std::endl;
    }
  else
    {
    std::cerr << "Trying to access inexistant DICOM tag." << std::endl;
    return 0;
    }

  unsigned int imageIndex = atoi( value.c_str() );

  tagkey = "0054|0081";
  if( itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId ) )
    {
    std::cout << labelId << " (" << tagkey << "): ";
    if( dicomIO->GetValueFromTag(tagkey, value) )
      {
      std::cout << value;
      }
    else
      {
      std::cout << "(No Value Found in File)";
      }
    std::cout << std::endl;
    }
  else
    {
    std::cerr << "Trying to access inexistant DICOM tag." << std::endl;
    }

  unsigned int numberOfSlices = atoi ( value.c_str() );

  // compute gate number
  
  unsigned int gateComputed = ceil( ((1.0 * imageIndex)/numberOfSlices));

  std::cout << "Computed gate index: " << gateComputed << std::endl;

  if( gateComputed == gateNumber )
    {
    std::cout << "\t This dicom slice belongs to this gate " << std::endl;
    itksys::SystemTools::CopyFileAlways( argv[1], argv[3] );
    }

  return EXIT_SUCCESS;
}
