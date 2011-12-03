/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
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
  
  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                              = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow -> logger

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();
  worldReference->SetLogger( logger );

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

  reader->RequestGetImage(); // Request to send the image as an event.

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

  CTImagePointer ctImagePointer = ctImageObserver->GetCTImage();  

  typedef igstk::CTImageSpatialObjectRepresentation RepresentationType;

  RepresentationType::Pointer representation = RepresentationType::New();
          
  representation->SetLogger( logger );

  representation->RequestSetImageSpatialObject( ctImagePointer );
                        

  representation->RequestSetOrientation( RepresentationType::Axial );
  representation->RequestSetSliceNumber( 0 );

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

  view2D->RequestAddObject( representation );
    
  // Reseting the camera after reading the image is more effective
  view2D->RequestResetCamera();

  // Configuring the view refresh rate
  view2D->SetRefreshRate( 40 );
  view2D->RequestStart();

  // Check for FLTK events before we start checking 
  // IGSTK events. This should allow the widget to 
  // be displayed first.
  Fl::check(); 

  // Do manual redraws
  for( unsigned int i=0; i < 10; i++)
    {
    Fl::wait( 0.05 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }


  //
  // Now read the image, so we can test the normal case
  //
  try
    {
    reader->RequestReadImage(); // Request to read the image from the files
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
  
  reader->RequestGetImage();    // Request to send the image as an event.

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

  CTImagePointer ctImage = ctImageObserver->GetCTImage();

  ctImage->SetLogger( logger );

  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  ctImage->RequestSetTransformAndParent( transform, worldReference );
  view2D->RequestSetTransformAndParent( transform, worldReference );

  representation->RequestSetImageSpatialObject( ctImage );
                           

  view2D->RequestAddObject( representation );

  // Do manual redraws
  view2D->RequestResetCamera();
  for( unsigned int i=0; i < 20; i++)
    {
    Fl::wait( 0.05 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  
  /* Save screenshots in a file */
  std::string filename;
  filename = argv[2]; 
  Fl::wait( 0.05 );
  igstk::PulseGenerator::CheckTimeouts();
  Fl::check();       // trigger FLTK redraws
  view2D->RequestStop();
  Fl::wait( 0.05 );
  igstk::PulseGenerator::CheckTimeouts();
  Fl::check();       // trigger FLTK redraws
  view2D->RequestSaveScreenShot( filename ); 
  view2D->RequestStart();


  // Do manual redraws for each orientation while changing slice numbers
    {
    representation->RequestSetOrientation( RepresentationType::Axial );
    view2D->RequestSetOrientation( igstk::View2D::Axial );
    view2D->RequestResetCamera();
    for(unsigned int i=0; i<10; i++)
      {
      representation->RequestSetSliceNumber( i );
      Fl::wait( 0.05 );
      igstk::PulseGenerator::CheckTimeouts();
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
      Fl::wait( 0.05 );
      igstk::PulseGenerator::CheckTimeouts();
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
      Fl::wait( 0.05 );
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      std::cout << "i= " << i << std::endl;
      }
    }

  view2D->RequestRemoveObject( representation );
  representation = NULL;
  view2D->RequestRemoveObject( representation );

  /** Testing the reuse of the reader by reading a image with different size
   *  Testing the ITK image to VTK image filter pipeline */
  if (1)
    {
    igstk::CTImageReader::Pointer reader2 = igstk::CTImageReader::New();
    igstk::CTImageSpatialObjectRepresentation::Pointer representation2 
                          = igstk::CTImageSpatialObjectRepresentation::New();
    igstk::CTImageReader::DirectoryNameType directoryName2;

    // Add an observer that will receive the image as an event.
    reader2->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                        ctImageObserver);

    std::cout << "Reading the first DICOM series : " << argv[1] <<std::endl;
    directoryName2 = argv[1];

    reader2->RequestSetDirectory( directoryName2 );  // Set the DICOM directory
    reader2->RequestReadImage();   // Request to read the image from the files
    reader2->RequestGetImage();    // Request to send the image as an event.

    if( ctImageObserver->GotCTImage() )
      {
        
      CTImagePointer ctImage2 = ctImageObserver->GetCTImage();

      if( ctImage2->IsEmpty() )
        {
        std::cerr << "The image was expected to be Non-Empty,but it was empty."
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

    ctImage = ctImageObserver->GetCTImage();

    ctImage->SetLogger( logger );

    igstk::Transform transform2;
    transform2.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
    ctImage->RequestSetTransformAndParent( transform2, worldReference );

    representation2->RequestSetImageSpatialObject( ctImage );

    representation2->RequestSetOrientation( RepresentationType::Axial );

    view2D->RequestAddObject( representation2 );
    view2D->RequestSetOrientation( igstk::View2D::Axial );

    for(unsigned int i=0; i<10; i++)
      {
      representation2->RequestSetSliceNumber( i );
      Fl::wait( 0.05 );
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();
      std::cout << "i= " << i << std::endl;
      }

    std::cout << "Reading the second DICOM series : " << argv[3] <<std::endl;
    directoryName2 = argv[3];

    reader2 = ReaderType::New();                     // To avoid vtkImageImport update extent mismatch error
    reader2->RequestSetDirectory( directoryName2 );  // Set the DICOM directory
    reader2->RequestReadImage();   // Request to read the image from the files
    reader2->RequestGetImage();    // Request to send the image as an event.

    if( ctImageObserver->GotCTImage() )
      {
        
      CTImagePointer ctImage2 = ctImageObserver->GetCTImage();

      if( ctImage2->IsEmpty() )
        {
        std::cerr << "The image was expected to be Non-Empty,but it was empty."
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


    representation2->RequestSetImageSpatialObject( 
                                       ctImageObserver->GetCTImage() );

    representation2->RequestSetOrientation( RepresentationType::Axial );

    for(unsigned int i=0; i<10; i++)
      {
      representation2->RequestSetSliceNumber( i );
      Fl::wait( 0.05 );
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();
      std::cout << "i= " << i << std::endl;
      }
    }

  delete widget2D;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
