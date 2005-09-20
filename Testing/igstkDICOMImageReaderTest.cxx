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
#include "itkImageSeriesWriter.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkDICOMImageReaderTest( int argc, char* argv[] )
{

  if(argc < 3)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries OutputImage"<<std::endl;
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

  typedef short    PixelType;
  typedef igstk::DICOMImageReader< PixelType >    ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout<<"Reading the DICOM series : "<<argv[1]<<std::endl;

  reader->SetDirectory(argv[1]);
  reader->RequestImageRead();
  
  typedef short      OutputPixelType;
  const   unsigned int        OutputDimension = 3;

  typedef itk::Image< OutputPixelType, OutputDimension >   ImageOutputType;
  typedef itk::ImageFileWriter< ImageOutputType >  WriterType;

  /* Write the DICOM data as a 3D volume data */
  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName(argv[2]);
  writer->SetInput( reader->GetITKImageData() );
  writer->DebugOn();

  std::cout<<"Writing out the DICOM image into another format : "<<argv[2]<<std::endl;
  writer->Update();

  return EXIT_SUCCESS;
}

