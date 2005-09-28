/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReaderTest.cxx
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

#include "igstkDICOMImageReader.h"
#include "igstkImageSpatialObject.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkDICOMImageReaderTest( int argc, char* argv[] )
{

  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef short      PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  

  typedef igstk::DICOMImageReader< ImageSpatialObjectType >    ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
 
  return EXIT_SUCCESS;
}

