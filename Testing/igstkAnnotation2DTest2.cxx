/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2DTest2.cxx
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
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkViewNew2D.h"
#include "igstkAnnotation2D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkFLTKWidget.h"

namespace Annotation2DTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}


int igstkAnnotation2DTest2( int argc, char* argv[] )
{
  igstk::RealTimeClock::Initialize();

  if( argc < 2 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage  " << std::endl;
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

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger


  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  typedef igstk::CTImageSpatialObject  CTImageType;
  typedef CTImageType::ConstPointer    CTImagePointer;

  // Attach an observer
  typedef Annotation2DTest::CTImageObserver CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  // Now read the image, so we can test the normal case
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
  
  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();
          
  representation->SetLogger( logger );
  representation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );
  representation->RequestSetOrientation( RepresentationType::Axial );

  // Add 2D Annotations
  typedef igstk::Annotation2D Annotation2DType;
  Annotation2DType::Pointer annotation = Annotation2DType::New();
          
  annotation->SetLogger( logger );
  
  //Add the annotations to each corner ([0,3]
  annotation->RequestAddAnnotationText ( 0, "Corner 0");
  annotation->RequestAddAnnotationText ( 1, "Corner 1");
  annotation->RequestAddAnnotationText ( 2, "Corner 2");
  annotation->RequestAddAnnotationText ( 3, "Corner 3");

  //use different colors for each corner annotation 
  annotation->RequestSetFontColor( 0, 1.0, 0.0, 0.0 ); 
  annotation->RequestSetFontColor( 1, 0.0, 1.0, 0.0 ); 
  annotation->RequestSetFontColor( 2, 0.0, 0.0, 1.0 ); 
  annotation->RequestSetFontColor( 3, 1.0, 1.0, 0.0 ); 

  // Add an invalid index for testing purpose 
  annotation->RequestAddAnnotationText ( 10, "Invalid index");

  // Invoke AddAnnotations function for testing purpose
  annotation->RequestAddAnnotations();

  // Invoke the print function
  annotation->Print( std::cout );

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532," Annotation View Test");
    
  typedef igstk::ViewNew2D  View2DType;

  View2DType::Pointer view2D = View2DType::New();

  view2D->SetLogger( logger ); 

  // Add spatialobject
  view2D->RequestAddObject( representation );
  
  // Add annotation
  view2D->RequestAddAnnotation2D( annotation );

  // Center the camera in order to make visible
  // all the objects in the scene.
  view2D->RequestResetCamera();

  // Start the pulse generator of the View 
  view2D->RequestSetRefreshRate( 20 );
  view2D->RequestStart();

  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;
  
  // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget2D = 
                    new FLTKWidgetType( 10,10,512,512,"2D View");
  fltkWidget2D->RequestSetView( view2D );
  fltkWidget2D->SetLogger( logger );

  view2D->RequestInitializeRenderWindowInteractor();    
  view2D->RequestStart();

  form->resizable( form );
  form->end();
  form->show();

  // Do manual redraws
  for( unsigned int i=0; i < 100; i++)
    {
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();   // trigger FLTK redraws
    }

  //Modify the text of corner 2 and 3
  annotation->RequestAddAnnotationText( 2, "Corner 22" );
  annotation->RequestAddAnnotationText( 3, "Corner 33" );

  //Modify the font size
  annotation->RequestSetFontSize( 1, 10 );
  annotation->RequestSetFontSize( 0, 10 );

  // Do manual redraws
  for( unsigned int i=0; i < 100; i++)
    {
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();   // trigger FLTK redraws
    }

  delete fltkWidget2D;
  delete form;
  
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
