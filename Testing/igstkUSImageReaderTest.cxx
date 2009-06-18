/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageReaderTest.cxx
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

#include "igstkUSImageReader.h"
#include "itkImageFileWriter.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace USImageReaderTest
{
igstkObserverObjectMacro(USImage,
    ::igstk::USImageReader::ImageModifiedEvent,::igstk::USImageObject)

igstkObserverMacro( DICOMImageInvalidError, 
    ::igstk::DICOMImageReadingErrorEvent, ::igstk::EventHelperType::StringType)
}

int igstkUSImageReaderTest( int argc, char* argv[] )
{
  igstk::RealTimeClock::Initialize();

  if(argc < 2)
    {
    std::cerr << "Usage: " << argv[0] <<" USImage "<<" Non-USImage"<<std::endl;
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

  typedef igstk::USImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading US image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );

  reader->Print( std::cout );

  // Attach an observer
  typedef USImageReaderTest::USImageObserver USImageObserverType;
  USImageObserverType::Pointer USImageObserver = USImageObserverType::New();
  reader->AddObserver(::igstk::USImageReader::ImageModifiedEvent(),
                            USImageObserver);

  try
    {
    reader->RequestReadImage();
    }
  catch( ... )
    {
    std::cerr << "ERROR: An exception was thrown while reading the MR dataset"
              << std::endl;
    std::cerr << "This should not have happened. The State Machine should have"
              << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input"
              << std::endl;
    return EXIT_FAILURE;
    }

  reader->RequestGetImage();

  if(!USImageObserver->GotUSImage())
    {
    std::cout << "No USImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::USImageObject::Pointer USImage = USImageObserver->GetUSImage();

  USImage->Print(std::cout);

  /* Now try to load a non-MR image */
  std::cout<<"Reading non-US image : "<<argv[2]<<std::endl;

  directoryName = argv[2];

  reader->RequestSetDirectory( directoryName );

  reader->Print( std::cout );

  // Attach an error observer
  USImageReaderTest::DICOMImageInvalidErrorObserver::Pointer errorObs = 
    USImageReaderTest::DICOMImageInvalidErrorObserver::New();
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

  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
