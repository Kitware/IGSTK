/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageReaderTest.cxx
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

#include "igstkCTImageReader.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace CTImageReaderTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( DICOMImageInvalidError, 
    ::igstk::DICOMImageReadingErrorEvent, ::igstk::EventHelperType::StringType)
}


int igstkCTImageReaderTest( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();


  if( argc < 2 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage "<<"  Non-CTImage " <<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout<<"Reading CT image : "<<argv[1]<<std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  reader->Print( std::cout );

  // Attach an observer
  typedef CTImageReaderTest::CTImageObserver CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  try
    {
    reader->RequestReadImage();
    }
  catch( ... )
    {
    std::cerr << "ERROR: An exception was thrown while reading the CT dataset"
              << std::endl;
    std::cerr << "This should not have happened. The State Machine should have" 
              << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input " 
              << std::endl;
    return EXIT_FAILURE;
    }

  reader->RequestGetImage();

  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::CTImageSpatialObject::Pointer ctImage = ctImageObserver->GetCTImage();

  // Details of the loaded image
  ctImage->Print( std::cout );


  /* Now try to load a non-MR image */
  std::cout<<"Reading non-CT image : "<<argv[2]<<std::endl;

  directoryName = argv[2];

  reader->RequestSetDirectory( directoryName );

  reader->Print( std::cout );

  // Attach an error observer
  CTImageReaderTest::DICOMImageInvalidErrorObserver::Pointer errorObs = 
    CTImageReaderTest::DICOMImageInvalidErrorObserver::New();
  reader->AddObserver( igstk::DICOMImageReadingErrorEvent(), errorObs );

  reader->AddObserver(::igstk::DICOMImageReadingErrorEvent(), errorObs);

  try
  {
    reader->RequestReadImage();
  }
  catch( ... )
  {
    std::cerr << "ERROR: An exception was thrown while reading the a image"
      << std::endl;
    std::cerr << "This should not have happened. The State Machine should have"
      << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input"
      << std::endl;
    return EXIT_FAILURE;
  }

  if(!errorObs->GotDICOMImageInvalidError())
  {
    std::cout << "Failed to report invalid image" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
