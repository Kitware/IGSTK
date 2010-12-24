/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PETDicomImageSUVComputation.cxx
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

#include <iostream>

#include "itkImage.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageFileReader.h"
#include "itkNumericTraits.h"

#include "vnl/vnl_math.h"

#ifdef ITK_PRE4_VERSION
#include "itkPETDicomImageReader.h"
#else
#include "itkPETDicomImageReader2.h"
#endif

int main(int argc, char* argv [] )
{
  if( argc < 2 )
  {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage " << std::endl;
    return EXIT_FAILURE;
  }

  typedef float PixelType;
  typedef itk::Image< PixelType, 3>                  ImageType;  

#ifdef ITK_PRE4_VERSION
  typedef itk::PETDicomImageReader<ImageType>        ReaderType;
#else
  typedef itk::PETDicomImageReader2<ImageType>        ReaderType;
#endif

  ReaderType::Pointer   reader = ReaderType::New();

  std::cout << "Reading the PET DICOM data series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  
  reader->SetDirectory( directoryName );
  reader->ReadImage();
  reader->GetParametersFromDicomHeader();

  /** Print out various dicom parameters */
  std::cout << "Weight Units:\t" << reader->GetWeightUnits() << std::endl;
  std::cout << "DoseRadioactivityUnits:\t" << reader->GetDoseRadioactivityUnits() << std::endl;   
  std::cout << "Patient name \t" << reader->GetPatientName() << std::endl;
  std::cout << "Study date \t" << reader->GetStudyDate() << std::endl;
  std::cout << "RadiopharmaceuticalStartTime \t" << reader->GetRadiopharmaceuticalStartTime() << std::endl;
  std::cout << "DecayCorrection:\t" << reader->GetDecayCorrection() << std::endl;
  std::cout << "DecayFactor :\t" << reader->GetDecayFactor() << std::endl;
  std::cout << "FrameReferenceTime :\t" << reader->GetFrameReferenceTime() << std::endl;
  std::cout << "RadioNuclide Half time:\t" << reader->GetRadionuclideHalfLife() << std::endl;
  std::cout << "Series time \t" << reader->GetSeriesTime() << std::endl;
  std::cout << "Philips SUV factor \t" << reader->GetPhilipsSUVFactor() << std::endl;
  std::cout << "Calibration factor \t" << reader->GetCalibrationFactor() << std::endl;


  //Generate a label image. Image as big as the input PET image and with all pixels having
  // intensity value 1. Homogeneous image
  //
  typedef itk::Image< unsigned int, 3 > LabelImageType;

  // Then the image object can be created
  LabelImageType::Pointer labelImage = LabelImageType::New();

  LabelImageType::RegionType region;
  region.SetSize(reader->GetOutput()->GetLargestPossibleRegion().GetSize());
  region.SetIndex(reader->GetOutput()->GetLargestPossibleRegion().GetIndex());
  labelImage->SetRegions( region );
  labelImage->SetOrigin(reader->GetOutput()->GetOrigin());
  labelImage->SetSpacing(reader->GetOutput()->GetSpacing());
  labelImage->Allocate();
  LabelImageType::PixelType  labelValue  = 1;
  labelImage->FillBuffer(labelValue);

  typedef itk::LabelStatisticsImageFilter< ImageType, LabelImageType > FilterType;

  FilterType::Pointer filter = FilterType::New();

  filter->SetInput (      reader->GetOutput() );
  filter->SetLabelInput ( labelImage );
  filter->UseHistogramsOn();
  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught ! " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef FilterType::RealType          RealType;
  typedef FilterType::BoundingBoxType   BoundingBoxType;
  typedef FilterType::RegionType        RegionType;
  typedef FilterType::LabelPixelType    LabelPixelType;

  const RealType min      =  filter->GetMinimum( labelValue );
  const RealType max      =  filter->GetMaximum( labelValue );
  const RealType mean     =  filter->GetMean( labelValue );
  const RealType sigma    =  filter->GetSigma( labelValue );
  const RealType variance =  filter->GetVariance( labelValue );
  const BoundingBoxType box = filter->GetBoundingBox( labelValue );
  
  std::cout << "Minimum   = " << min      << std::endl;
  std::cout << "Maximum   = " << max      << std::endl;
  std::cout << "Mean      = " << mean     << std::endl;
  std::cout << "Sigma     = " << sigma    << std::endl;
  std::cout << "Variance  = " << variance << std::endl;

  BoundingBoxType::const_iterator itr = box.begin();
  while( itr != box.end() )
    {
    std::cout << "Index = " << *itr << std::endl;
    ++itr;
    }
  return EXIT_SUCCESS;
}

