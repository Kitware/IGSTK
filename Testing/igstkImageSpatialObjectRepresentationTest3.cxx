/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest3.cxx
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
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace ImageSpatialObjectRepresentationTest3
{
  igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}


int igstkImageSpatialObjectRepresentationTest3( int argc, char* argv[] )
{

  igstk::RealTimeClock::Initialize();


  if( argc < 3 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage  "<< "Output image file for a screenshot" << std::endl;
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
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK OutputWindow -> logger

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  reader->RequestSetDirectory( directoryName );

  
  // Attach an observer
  typedef ImageSpatialObjectRepresentationTest3::CTImageObserver CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  /* set up the image spatial object representation */ 
  typedef igstk::CTImageSpatialObjectRepresentation ImageRepresentationType;
  ImageRepresentationType::Pointer imageRepresentation = ImageRepresentationType::New();
  imageRepresentation->SetLogger( logger );

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

  /* Set up the ellipsoidal spatial object that will be added to each 
   * each corners of the image */
  
  // Top Left corner
  typedef igstk::EllipsoidObject EllipsoidObjectType;

  EllipsoidObjectType::Pointer  ellipsoidObject = EllipsoidObjectType::New();
  ellipsoidObject->SetLogger ( logger );
  
  typedef igstk::EllipsoidObjectRepresentation EllipsoidRepresentationType;
  EllipsoidRepresentationType::Pointer 
       ellipsoidObjectRepresentation = EllipsoidRepresentationType::New();
  ellipsoidObjectRepresentation->SetLogger ( logger );
         
  EllipsoidObjectType::ArrayType radius;
  radius[0] = 20;
  radius[1] = 20;
  radius[2] = 20;
  ellipsoidObject->SetRadius(radius);

  igstk::Transform                          transform ;
  igstk::Transform::VectorType              translation;

  translation[0] = -153.66;
  translation[1] = -307.66;
  translation[2] = -196.0;
 
  igstk::Transform::ErrorType            errorValue = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime = 1e20;
  
  transform.SetTranslation( translation, errorValue, validtyTime ); 
      
  ellipsoidObject->RequestSetTransform( transform );
  
  ellipsoidObjectRepresentation->RequestSetEllipsoidObject( ellipsoidObject );
  ellipsoidObjectRepresentation->SetColor( 0.0, 1.0, 0.0 );
  ellipsoidObjectRepresentation->SetOpacity( 1.0 );

  //Top Right corner
  EllipsoidObjectType::Pointer  ellipsoidObject2 = EllipsoidObjectType::New();
  ellipsoidObject2->SetLogger ( logger );
  
  EllipsoidRepresentationType::Pointer 
       ellipsoidObjectRepresentation2 = EllipsoidRepresentationType::New();
  ellipsoidObjectRepresentation2->SetLogger ( logger );

  EllipsoidObjectType::ArrayType radius2;
  radius2[0] = 20;
  radius2[1] = 20;
  radius2[2] = 20;
  ellipsoidObject2->SetRadius(radius2);

  igstk::Transform                          transform2 ;
  igstk::Transform::VectorType              translation2;

  translation2[0] =  89.54; 
  translation2[1] =  -64.46;
  translation2[2] = -196.0;
 
  igstk::Transform::ErrorType            errorValue2 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime2 = 1e20;
  
  transform2.SetTranslation( translation2, errorValue2, validtyTime2 ); 
      
  ellipsoidObject2->RequestSetTransform( transform2 );
  
  ellipsoidObjectRepresentation2->RequestSetEllipsoidObject( ellipsoidObject2 );
  ellipsoidObjectRepresentation2->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidObjectRepresentation2->SetOpacity( 1.0 );

 //Bottom Left corner
  EllipsoidObjectType::Pointer  ellipsoidObject3 = EllipsoidObjectType::New();
  ellipsoidObject3->SetLogger ( logger );
  
  EllipsoidRepresentationType::Pointer 
       ellipsoidObjectRepresentation3 = EllipsoidRepresentationType::New();
  ellipsoidObjectRepresentation3->SetLogger ( logger );

  EllipsoidObjectType::ArrayType radius3;
  radius3[0] = 20;
  radius3[1] = 20;
  radius3[2] = 20;
  ellipsoidObject3->SetRadius(radius3);

  igstk::Transform                          transform3 ;
  igstk::Transform::VectorType              translation3;

  translation3[0] =  -396.87;
  translation3[1] =  -64.46;
  translation3[2] =  -196.0;
 
  igstk::Transform::ErrorType            errorValue3 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime3 = 1e20;
  
  transform3.SetTranslation( translation3, errorValue3, validtyTime3 ); 
      
  ellipsoidObject3->RequestSetTransform( transform3 );
  
  ellipsoidObjectRepresentation3->RequestSetEllipsoidObject( ellipsoidObject3 );
  ellipsoidObjectRepresentation3->SetColor( 1.0, 0.0, 0.0 );
  ellipsoidObjectRepresentation3->SetOpacity( 1.0 );

 //Bottom right corner
  EllipsoidObjectType::Pointer  ellipsoidObject4 = EllipsoidObjectType::New();
  ellipsoidObject4->SetLogger ( logger );
  
  EllipsoidRepresentationType::Pointer 
       ellipsoidObjectRepresentation4 = EllipsoidRepresentationType::New();
  ellipsoidObjectRepresentation4->SetLogger ( logger );

  EllipsoidObjectType::ArrayType radius4;
  radius4[0] = 20;
  radius4[1] = 20;
  radius4[2] = 20;
  ellipsoidObject4->SetRadius( radius4 );

  igstk::Transform                          transform4 ;
  igstk::Transform::VectorType              translation4;

         
  translation4[0] = -153.66 ;
  translation4[1] = 178.752;
  translation4[2] = -196.0;
 
  igstk::Transform::ErrorType            errorValue4 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime4 = 1e20;
  
  transform4.SetTranslation( translation4, errorValue4, validtyTime4 ); 
      
  ellipsoidObject4->RequestSetTransform( transform4 );
  
  ellipsoidObjectRepresentation4->RequestSetEllipsoidObject( ellipsoidObject4 );
  ellipsoidObjectRepresentation4->SetColor( 1.0, 1.0, 0.0 );
  ellipsoidObjectRepresentation4->SetOpacity( 1.0 );

  
  EllipsoidObjectType::Pointer  ellipsoidObject5 = EllipsoidObjectType::New();
  ellipsoidObject5->SetLogger ( logger );

  EllipsoidRepresentationType::Pointer 
       ellipsoidObjectRepresentation5 = EllipsoidRepresentationType::New();
  ellipsoidObjectRepresentation5->SetLogger ( logger );

  EllipsoidObjectType::ArrayType radius5;
  radius5[0] = 20;
  radius5[1] = 20;
  radius5[2] = 20;
  ellipsoidObject5->SetRadius( radius5 );

  igstk::Transform                          transform5 ;
  igstk::Transform::VectorType              translation5;

         
  translation5[0] = 0.0 ;
  translation5[1] = 0.0 ;
  translation5[2] = -196.0;
 
  igstk::Transform::ErrorType            errorValue5 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime5 = 1e20;
  
  transform5.SetTranslation( translation5, errorValue5, validtyTime5 ); 
      
  ellipsoidObject5->RequestSetTransform( transform5 );
  
  ellipsoidObjectRepresentation5->RequestSetEllipsoidObject( ellipsoidObject5 );
  ellipsoidObjectRepresentation5->SetColor( 1.0, 1.0, 1.0 );
  ellipsoidObjectRepresentation5->SetOpacity( 1.0 );

 
  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();

  view2D->SetLogger( logger ); 
  view2D->RequestEnableInteractions();

  // Add the image object representation to the view
  view2D->RequestAddObject( imageRepresentation );

  // Add the ellipsoid object representation to the view 
  view2D->RequestAddObject( ellipsoidObjectRepresentation );
  view2D->RequestAddObject( ellipsoidObjectRepresentation2 );
  view2D->RequestAddObject( ellipsoidObjectRepresentation3 );
  view2D->RequestAddObject( ellipsoidObjectRepresentation4 );
  view2D->RequestAddObject( ellipsoidObjectRepresentation5 );

  reader->RequestGetImage();
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  imageRepresentation->RequestSetImageSpatialObject( ctImageObserver->GetCTImage() );
  imageRepresentation->RequestSetOrientation( ImageRepresentationType::Axial );
  
  view2D->RequestSetOrientation( igstk::View2D::Axial );

  imageRepresentation->RequestSetSliceNumber( 0 );
  
  view2D->RequestResetCamera();
  view2D->RequestSetRefreshRate( 30 );
  view2D->RequestStart();
  Fl::wait(1.0);  
  igstk::PulseGenerator::CheckTimeouts();

  for(unsigned int i=0; i<10; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();
    }

  //Request refreshing stop to take a screenshot
  view2D->RequestStop();
      
  /* Save screenshots in a file */
  std::string filename;
  filename = argv[2]; 
  std::cout << "Saving a screen shot in file:" << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( filename );

 

  delete view2D;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}

