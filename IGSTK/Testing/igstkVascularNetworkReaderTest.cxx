/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReaderTest.cxx
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

#include "igstkVascularNetworkReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkView3D.h"
#include "igstkVascularNetworkObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"

namespace VascularNetworkReaderTest
{
igstkObserverObjectMacro(VascularNetwork,
    ::igstk::VascularNetworkObjectModifiedEvent,
    ::igstk::VascularNetworkObject)

igstkObserverObjectMacro(Vessel,
  ::igstk::VesselObjectModifiedEvent,
  ::igstk::VesselObject)
}

int igstkVascularNetworkReaderTest( int argc, char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
    {
    std::cerr << "Error: Missing command line arguments" << std::endl;
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << " inputGoodFileName ";
    std::cerr << " inputBadFileName " 
              << " screenshot" << std::endl;
    return 1;
    }

  typedef igstk::VascularNetworkReader    ReaderType;

  ReaderType::Pointer  reader = ReaderType::New();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

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

  typedef ReaderType::VascularNetworkType       VascularNetworkType;
  typedef VascularNetworkType::VesselObjectType VesselObjectType;

  VascularNetworkType::Pointer network = 
                                vascularNetworkObserver->GetVascularNetwork();

  typedef VascularNetworkReaderTest::VesselObserver VesselObserver;

  VesselObserver::Pointer vesselObserver = VesselObserver::New();
 
  network->AddObserver(
            igstk::VesselObjectModifiedEvent(),
            vesselObserver);

  std::cout << "Testing GetVessel(): ";
  network->RequestGetVessel(0);
  if(!vesselObserver->GotVessel())
    {
    std::cout << "No Vessel!" << std::endl;
    return EXIT_FAILURE;
    }
  
  vesselObserver->Reset();

  network->RequestGetVessel(10000);
  if(vesselObserver->GotVessel())
    {
    std::cout << "Get Vessel but should not" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]";

  network->Print( std::cout );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                              = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"Vascular Network View Test");
    
  typedef igstk::View3D  View3DType;

  View3DType * view3D = new View3DType( 10,10,512,512,"3D View");

  form->end();
  form->show();

  view3D->SetLogger( logger ); 
  view3D->RequestEnableInteractions();

  // Create the vascular network representation
  typedef igstk::VascularNetworkObjectRepresentation 
                                            VascularNetworkRepresentationType;

  VascularNetworkRepresentationType::Pointer
     vascularNetworkRepresentation = VascularNetworkRepresentationType::New();

  vascularNetworkRepresentation->SetColor(1.0,0.0,0.0);
  vascularNetworkRepresentation->SetOpacity(0.5);

  vascularNetworkRepresentation->RequestSetVascularNetworkObject(network);

  // Add the ellipsoid object representation to the view 
  view3D->RequestAddObject( vascularNetworkRepresentation );

  view3D->RequestResetCamera();
  view3D->RequestSetRefreshRate( 30 );
  view3D->RequestStart();
  Fl::wait(1.0);  
  igstk::PulseGenerator::CheckTimeouts();

  for(unsigned int i=0; i<10; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();
    }

  //Request refreshing stop to take a screenshot
  view3D->RequestStop();
      
  /* Save screenshots in a file */
  std::string filename = argv[3]; 
  std::cout << "Saving a screen shot in file:" 
            << filename.c_str() << std::endl;
  view3D->RequestSaveScreenShot( filename );

  delete view3D;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  std::cout << "Test DONE" << std::endl;
  return EXIT_SUCCESS;
}
