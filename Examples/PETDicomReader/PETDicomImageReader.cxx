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
  
  if(argc < 3)
    {
    std::cerr<<"Usage: "<<argv[0] <<"  DICOMSeries ConvertedMetaImageFileName"<<std::endl;
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

  
  std::cout << "Convert the dicom series to a meta image format...." << std::endl;
  reader->SetMetaImageFilename( argv[2] );
  reader->ConvertDICOMDataToMetaImage();

  return EXIT_SUCCESS;
}
