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
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace CTImageSpatialObjectReadingAndRepresentationTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}

int igstkCTImageSpatialObjectReadingAndRepresentationTest( 
                                                        int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
    {
      std::cerr << "Usage: " << argv[0] << "  CTImage  " 
              << "Output image file for a screenshot" 
              << " CTImageExcerpt " << std::endl;
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
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                              = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow -> logger

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  // Read in a DICOM series
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
 
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

  reader->RequestGetImage();

  if( ctImageObserver->GotCTImage() )
    {
    std::cerr << "Error: The image was sent  from the Reader " << std::endl;
    std::cerr << "even though it was not actually read." << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "Test for premature reading of the image: PASSED !" 
              << std::endl;
    }

  
  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();
          
  representation->SetLogger( logger );

  representation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );

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
    std::cerr << "ERROR: An exception was thrown while reading the CT dataset"
              << std::endl;
    std::cerr << "This should not have happened. The State Machine should have"
              << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input " 
              << std::endl;
    return EXIT_FAILURE;
    }
  
  reader->RequestGetImage();

  if( ctImageObserver->GotCTImage() )
    {
      
    CTImagePointer ctImage = ctImageObserver->GetCTImage();

    if( ctImage->IsEmpty() )
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


  representation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );

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
   *  Testing the ITK image to VTK image filter pipeline */
    {
    igstk::CTImageReader::Pointer reader = igstk::CTImageReader::New();
    igstk::CTImageSpatialObjectRepresentation::Pointer representation 
                          = igstk::CTImageSpatialObjectRepresentation::New();
    igstk::CTImageReader::DirectoryNameType directoryName;

    view2D->RequestAddObject( representation );
    view2D->RequestSetOrientation( igstk::View2D::Axial );

    std::cout << "Reading the first DICOM series : " << argv[1] <<std::endl;
    directoryName = argv[1];
    reader->RequestSetDirectory( directoryName );
    reader->RequestReadImage();
    representation->RequestSetImageSpatialObject( 
                                              ctImageObserver->GetCTImage() );
    representation->RequestSetOrientation( RepresentationType::Axial );

    for(unsigned int i=0; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      view2D->Update();
      Fl::check();
      std::cout << "i= " << i << std::endl;
      }

    std::cout << "Reading the second DICOM series : " << argv[3] <<std::endl;
    directoryName = argv[3];
    reader->RequestSetDirectory( directoryName );
    reader->RequestReadImage();
    representation->RequestSetImageSpatialObject( 
                                              ctImageObserver->GetCTImage() );
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
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
