/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectRepresentationTest.cxx
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

#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkCTImageReader.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace CTImageSpatialObjectRepresentationTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}

int igstkCTImageSpatialObjectRepresentationTest( int, char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::CTImageSpatialObjectRepresentation    RepresentationType;
  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef igstk::Object::LoggerType       LoggerType;
  typedef itk::StdStreamLogOutput         LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  representation->SetLogger( logger );

  // Instantiate a reader
  //
  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
 
  std::string name = representation->GetNameOfClass();

  // Attach an observer
  typedef CTImageSpatialObjectRepresentationTest::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  reader->RequestReadImage();
  reader->RequestGetImage();

  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }


  representation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );

  std::cout << "Name of class = " << name << std::endl;

  representation->Print( std::cout );

  // Do manual selections of slice number for each orientation 
    {
    representation->RequestSetOrientation( RepresentationType::Axial );
    for(unsigned int i=0; i<5; i++)
      {
      representation->RequestSetSliceNumber( i );
      }
    }

    {
    representation->RequestSetOrientation( RepresentationType::Sagittal );
    for(unsigned int i=0; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      }
    }

    {
    representation->RequestSetOrientation( RepresentationType::Coronal );
    for(unsigned int i=0; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      }
    }
  return EXIT_SUCCESS;

}
