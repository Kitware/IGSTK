/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageReaderTest.cxx
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

#include "igstkMRImageReader.h"
#include "itkImageFileWriter.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace MRImageReaderTest
{
igstkObserverObjectMacro(MRImage,
    ::igstk::MRImageReader::ImageModifiedEvent,::igstk::MRImageSpatialObject)
}

int igstkMRImageReaderTest( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();


  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  MRImage "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef igstk::MRImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout<<"Reading MR image : "<<argv[1]<<std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  reader->Print( std::cout );

  // Attach an observer
  typedef MRImageReaderTest::MRImageObserver MRImageObserverType;
  MRImageObserverType::Pointer mrImageObserver = MRImageObserverType::New();
  reader->AddObserver(::igstk::MRImageReader::ImageModifiedEvent(),
                            mrImageObserver);

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

  if(!mrImageObserver->GotMRImage())
    {
    std::cout << "No MRImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::MRImageSpatialObject::Pointer mrImage = mrImageObserver->GetMRImage();

  return EXIT_SUCCESS;
}
