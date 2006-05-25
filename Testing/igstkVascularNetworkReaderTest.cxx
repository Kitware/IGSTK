/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReaderTest.cxx
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

#include "igstkVascularNetworkReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace VascularNetworkReaderTest
{
igstkObserverObjectMacro(VascularNetwork,
    ::igstk::VascularNetworkReader::VascularNetworkModifiedEvent,
    ::igstk::VascularNetworkObject)
}

int igstkVascularNetworkReaderTest( int argc, char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
    std::cerr << "Error: Missing command line arguments" << std::endl;
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << " inputGoodFileName ";
    std::cerr << " inputBadFileName " << std::endl;
    return 1;
    }

  typedef igstk::VascularNetworkReader    ReaderType;

  ReaderType::Pointer  reader = ReaderType::New();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  reader->SetLogger( logger );

  std::string name = reader->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;

  reader->Print( std::cout );

  // Test error condition: on purpose request to read an object
  // without having provided the filename 
  reader->RequestReadObject();

  // Test empty name
  std::string emptyname;
  reader->RequestSetFileName( emptyname );
  
  // Test file doesn't exist
  std::string filenameThatDoesntExist = "/This/FileName/Does/Not/Exist";
  reader->RequestSetFileName( filenameThatDoesntExist );

  // Test file that is a directory
  std::string filenameIsADirectory = ".";
  reader->RequestSetFileName( filenameIsADirectory );
  
  // Now reading a corrupted file
  std::string filenameWithCorruptedContent = argv[2];

  reader->RequestSetFileName( filenameWithCorruptedContent );
  reader->RequestReadObject();

  // Test file that exists
  std::string filenameThatExists = argv[1];
  reader->RequestSetFileName( filenameThatExists );

  // Request to read the object from the file
  reader->RequestReadObject();
  
  typedef VascularNetworkReaderTest::VascularNetworkObserver
                                                     VascularNetworkObserver;
  VascularNetworkObserver::Pointer vascularNetworkObserver 
                                            = VascularNetworkObserver::New();
  reader->AddObserver(ReaderType::VascularNetworkModifiedEvent(),
                      vascularNetworkObserver);

  reader->RequestGetVascularNetwork();

  if(!vascularNetworkObserver->GotVascularNetwork())
    {
    std::cout << "No VascularNetwork!" << std::endl;
    return EXIT_FAILURE;
    }

  typedef ReaderType::VascularNetworkType VascularNetworkType;
  VascularNetworkType::ConstPointer network = 
                                       vascularNetworkObserver->GetVascularNetwork();

  network->Print( std::cout );

  std::cout << "Test DONE" << std::endl;
  return EXIT_SUCCESS;
}
