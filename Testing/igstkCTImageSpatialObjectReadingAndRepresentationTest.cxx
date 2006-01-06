/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
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
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkCTImageSpatialObjectReadingAndRepresentationTest( int argc, char* argv[] )
{

  if( argc < 4 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage  "<< "Output image file for a screenshot" <<" CTImageExcerpt "<< std::endl;
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

  // First, on purpose attempt to use an Empty image, 
  // in order to test error conditions.
  //
  CTImagePointer ctImage = reader->GetOutput();

  if( !ctImage->IsEmpty() )
    {
    std::cerr << "The image was expected to be empty, but it is not..." << std::endl;
    return EXIT_FAILURE;
    }

  
  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();
          
  representation->SetLogger( logger );

  representation->RequestSetImageSpatialObject( reader->GetOutput() );

  representation->RequestSetOrientation( RepresentationType::Axial );
  representation->RequestSetSliceNumber( 0 );

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();

  view2D->SetLogger( logger ); 
  view2D->RequestResetCamera();
  view2D->RequestEnableInteractions();

  view2D->RequestAddObject( representation );
    
  // Do manual redraws
  for( unsigned int i=0; i < 10; i++)
    {
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    }


  //
  // Now read the image, so we can test the normal case
  //
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
  
  ctImage = reader->GetOutput();

  if( ctImage->IsEmpty() )
    {
    std::cerr << "The image was expected to be Non-Empty, but it was empty." << std::endl;
    return EXIT_FAILURE;
    }
 
  representation->RequestSetImageSpatialObject( reader->GetOutput() );

  view2D->RequestAddObject( representation );

  // Do manual redraws
  for( unsigned int i=0; i < 20; i++)
    {
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    }

  /* Save screenshots in a file */
  std::string filename;
  filename = argv[2]; 
  view2D->RequestSaveScreenShot( filename ); 

  // Do manual redraws for each orientation while changing slice numbers
  {
  representation->RequestSetOrientation( RepresentationType::Axial );
  view2D->RequestSetOrientation( igstk::View2D::Axial );
  view2D->RequestResetCamera();
  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    std::cout << "i= " << i << std::endl;
    }
  }

  {
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  view2D->RequestSetOrientation( igstk::View2D::Sagittal );
  view2D->RequestResetCamera();
  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    std::cout << "i= " << i << std::endl;
    }
  }

  {
  representation->RequestSetOrientation( RepresentationType::Coronal );
  view2D->RequestSetOrientation( igstk::View2D::Coronal );
  view2D->RequestResetCamera();
  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    std::cout << "i= " << i << std::endl;
    }
  }

  view2D->RequestRemoveObject( representation );
  representation = NULL;
  view2D->RequestRemoveObject( representation );

  /** Testing the reuse of the reader by reading a image with different size
   *  Testing the ITK image to VTK image filter pipeline 
   */
  {
  igstk::CTImageReader::Pointer reader = igstk::CTImageReader::New();
  igstk::CTImageSpatialObjectRepresentation::Pointer representation = igstk::CTImageSpatialObjectRepresentation::New();
  igstk::CTImageReader::DirectoryNameType directoryName;

  view2D->RequestAddObject( representation );
  view2D->RequestSetOrientation( igstk::View2D::Axial );

  std::cout << "Reading the first DICOM series : " << argv[1] <<std::endl;
  directoryName = argv[1];
  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();    
  representation->RequestSetImageSpatialObject( reader->GetOutput() );
  representation->RequestSetOrientation( RepresentationType::Axial );

  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();
    Fl::check();
    std::cout << "i= " << i << std::endl;
    }

  std::cout << "Reset the reader...." << std::endl;
  reader->RequestResetReader();

  std::cout << "Reading the second DICOM series : " << argv[3] <<std::endl;
  directoryName = argv[3];
  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();    
  representation->RequestSetImageSpatialObject( reader->GetOutput() );
  representation->RequestSetOrientation( RepresentationType::Axial );

  for(unsigned int i=0; i<10; i++)
    {
    representation->RequestSetSliceNumber( i );
    view2D->Update();
    Fl::check();
    std::cout << "i= " << i << std::endl;
    }

  }

  delete view2D;
  delete form;
 
  return EXIT_SUCCESS;
}

