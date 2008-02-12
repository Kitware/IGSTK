/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectRepresentationWindowLevelTest.cxx
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

#include "igstkConfigure.h"
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"

namespace CTImageSpatialObjectReadingAndRepresentationTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}

int igstkCTImageSpatialObjectRepresentationWindowLevelTest( 
                                                        int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
      std::cerr << "Usage: " << argv[0] << "  CTImage  " 
              << "Output image file for a screenshot" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::CRITICAL );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                              = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow -> logger

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();  


  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  // Read in a DICOM series
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );  // Set the DICOM directory
  
 
  typedef igstk::CTImageSpatialObject  CTImageType;
  typedef CTImageType::Pointer         CTImagePointer;

  // First, on purpose attempt to use an Empty image, 
  // in order to test error conditions.
  //
  typedef CTImageSpatialObjectReadingAndRepresentationTest::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      WidgetType;

  View2DType::Pointer view2D = View2DType::New();

  // instantiate FLTK widget 
  WidgetType * widget2D = new WidgetType( 10,10,512,512,"2D View");
  widget2D->RequestSetView( view2D );
  widget2D->SetLogger( logger );

  form->end();
  form->show();

  view2D->SetLogger( logger ); 


  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();

  representation->SetLogger( logger );

  view2D->RequestAddObject( representation );
    

  // Configuring the view refresh rate
  view2D->SetRefreshRate( 40 );
  view2D->RequestStart();

  reader->RequestReadImage(); // Request to read the image from the files
  reader->RequestGetImage();  // Request to send the image as an event.

  if( ctImageObserver->GotCTImage() )
    {
      
    CTImagePointer imageSpatialObject = ctImageObserver->GetCTImage();

    if( imageSpatialObject->IsEmpty() )
      {
      std::cerr << "The image was expected to be Non-Empty, but it was empty." 
                << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cerr << "Test for reception of the image PASSED !" << std::endl;
      }
   
    }
  else
    {
    std::cerr << "The image was expected to be received" << std::endl;
    std::cerr << " but the payload event did not arrive." << std::endl;
    return EXIT_FAILURE;
    }


  CTImagePointer ctImage = ctImageObserver->GetCTImage();

  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  ctImage->RequestSetTransformAndParent( transform, worldReference );
  view2D->RequestSetTransformAndParent( transform, worldReference );


  representation->RequestSetImageSpatialObject( ctImage );


  representation->RequestSetSliceNumber( 0 );

  double window = 2000;
  double level  =  100;
 
  representation->SetWindowLevel( window, level );

  view2D->RequestResetCamera();

  // Check for FLTK events before we start checking 
  // IGSTK events. This should allow the widget to 
  // be displayed first.
  Fl::check(); 

  // Do manual redraws
  for( unsigned int i=0; i < 20; i++)
    {
    Fl::wait( 0.05 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  
  /* Save screenshots in a file */
  std::string filename;
  filename = argv[2]; 
  view2D->RequestSaveScreenShot( filename ); 

  
  // Do manual redraws  while changing the window level
  for(unsigned int i=0; i<100; i++)
    {
    level = i * 10;
    Fl::wait( 0.05 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    std::cout << "i= " << i << std::endl;
    representation->SetWindowLevel( window, level );
    }
  


  delete widget2D;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
