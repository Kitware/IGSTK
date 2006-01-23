/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2DTest.cxx
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

#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkAnnotation2D.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkAnnotation2DTest( int argc, char* argv[] )
{

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

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  typedef igstk::CTImageSpatialObject  CTImageType;
  typedef CTImageType::ConstPointer    CTImagePointer;

  // Now read the image, so we can test the normal case
  try
    {
    reader->RequestReadImage();
    }
  catch( ... )
    {
    std::cerr << "ERROR: An exception was thrown while reading the CT dataset" << std::endl;
    std::cerr << "This should not have happened. The State Machine should have" << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();
          
  representation->SetLogger( logger );
  representation->RequestSetImageSpatialObject( reader->GetOutput() );
  representation->RequestSetOrientation( RepresentationType::Axial );

  // Add 2D Annotations
  typedef igstk::Annotation2D Annotation2DType;
  Annotation2DType::Pointer annotation = Annotation2DType::New();
          
  annotation->SetLogger( logger );

  annotation->AddAnnotationText ( 0, "Corner 0");
  annotation->AddAnnotationText ( 1, "Corner 1");
  annotation->AddAnnotationText ( 2, "Corner 2");
  annotation->AddAnnotationText ( 3, "Corner 3");
  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();

  view2D->SetLogger( logger ); 
  view2D->RequestResetCamera();
  view2D->RequestEnableInteractions();

  // Add spatialobject
  view2D->RequestAddObject( representation );
  
  // Add annotation
  view2D->RequestAddAnnotation2D( annotation );

  // Do manual redraws
  for( unsigned int i=0; i < 10; i++)
    {
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    }

  delete view2D;
  delete form;
 
  return EXIT_SUCCESS;
}

