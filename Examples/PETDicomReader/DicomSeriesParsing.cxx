/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DicomSeriesReadImageWrite2.cxx
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

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itksys/SystemTools.hxx"

int main( int argc, char* argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " DicomDirectory  OutputDirectoryName [seriesName]" 
              << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetUseSeriesDetails( true );
  nameGenerator->AddSeriesRestriction("0008|0021" );
  nameGenerator->SetDirectory( argv[1] );
  

  try
    {
    std::cout << std::endl << "The directory: " << std::endl;
    std::cout << std::endl << argv[1] << std::endl << std::endl;
    std::cout << "Contains the following DICOM Series: ";
    std::cout << std::endl << std::endl;


    typedef std::vector< std::string >    SeriesIdContainer;
    
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    while( seriesItr != seriesEnd )
      {
      std::cout << seriesItr->c_str() << std::endl;
      seriesItr++;
      }
    std::string seriesIdentifier;

    if( argc > 3 ) // If no optional series identifier
      {
      seriesIdentifier = argv[3];
      }
    else
      {
      seriesIdentifier = seriesUID.begin()->c_str();
      }

    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;

    fileNames = nameGenerator->GetFileNames( seriesIdentifier );

    std::string outputDirectory = argv[2];

    try
      {
      //Copy the files to the output directory
      FileNamesContainer::const_iterator filesItr = fileNames.begin();
      FileNamesContainer::const_iterator filesEnd = fileNames.end();
      while( filesItr != filesEnd )
        {
        std::cout << filesItr->c_str() << std::endl;
        if ( itksys::SystemTools::FileExists( outputDirectory.c_str()))
          {
          itksys::SystemTools::CopyFileIfDifferent( filesItr->c_str(), outputDirectory.c_str() );
          }
        filesItr++;
        }
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
