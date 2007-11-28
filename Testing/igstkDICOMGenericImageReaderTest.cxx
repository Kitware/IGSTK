/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMGenericImageReaderTest.cxx
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

#include "igstkDICOMGenericImageReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

/* FIXME: This Event must be added, once we have the generic image.
namespace DICOMGenericImageReaderTest
  {
  igstkObserverObjectMacro(CTImage,
  ::igstk::DICOMGenericImageReader::ImageModifiedEvent,
  ::igstk::CTImageSpatialObject);
  }
*/


int igstkDICOMGenericImageReaderTest( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();


  if( argc < 2 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef igstk::DICOMGenericImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout<<"Reading CT image : "<<argv[1]<<std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  reader->Print( std::cout );

  // FIXME: This observer must be added back, once we have the generic image
  // class
  //
  // Attach an observer
  // typedef DICOMGenericImageReaderTest::CTImageObserver CTImageObserverType;
  // CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  //
  // FIXME: This event must be added
  // reader->AddObserver(
  //           ::igstk::DICOMGenericImageReader::ImageModifiedEvent(), 
  //           ctImageObserver);

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

  /* FIXME: Restore the verification of the image once we add the generic image.
   *
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::CTImageSpatialObject::Pointer ctImage = ctImageObserver->GetCTImage();

  // Details of the loaded image
  ctImage->Print( std::cout );
  */
  
  return EXIT_SUCCESS;
}
