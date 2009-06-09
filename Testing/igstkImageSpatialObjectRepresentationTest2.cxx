/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest2.cxx
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
#include "igstkAxesObject.h"
#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"


namespace ImageSpatialObjectRepresentationTest2
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)
}


int igstkImageSpatialObjectRepresentationTest2( int argc, char* argv[] )
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
  vtkLoggerOutput->SetLogger(logger);

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  reader->RequestSetDirectory( directoryName );

  
  // Attach an observer
  typedef ImageSpatialObjectRepresentationTest2::CTImageObserver 
                                                          CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New();
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  /* set up the image spatial object representation */ 
  typedef igstk::CTImageSpatialObjectRepresentation ImageRepresentationType;
  ImageRepresentationType::Pointer imageRepresentation =
    ImageRepresentationType::New();
  imageRepresentation->SetLogger( logger );

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

  /* Set up the box spatial object that will be added to each 
   * each corners of the image */
  
  // Top Left corner
  typedef igstk::BoxObject BoxObjectType;

  BoxObjectType::Pointer  boxObject = BoxObjectType::New();
  boxObject->SetLogger ( logger );
  
  typedef igstk::BoxObjectRepresentation BoxRepresentationType;
  BoxRepresentationType::Pointer 
       boxObjectRepresentation = BoxRepresentationType::New();
  boxObjectRepresentation->SetLogger ( logger );

  double size_X;
  double size_Y;
  double size_Z;
  
  size_X = 20.0;
  size_Y = 20.0;
  size_Z = 2.0;

  boxObject->SetSize( size_X, size_Y, size_Z );

  igstk::Transform                          transform;
  igstk::Transform::VectorType              translation;

  translation[0] = -153.66;
  translation[1] = -307.66;
  translation[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime = 1e20;
  
  transform.SetTranslation( translation, errorValue, validtyTime ); 

  boxObject->RequestSetTransformAndParent( transform, worldReference );

  boxObjectRepresentation->RequestSetBoxObject( boxObject );
  boxObjectRepresentation->SetColor( 1.0, 1.0, 1.0 );
  boxObjectRepresentation->SetOpacity( 1.0 );

  //Top Right corner
  BoxObjectType::Pointer  boxObject2 = BoxObjectType::New();
  boxObject2->SetLogger ( logger );
  
  BoxRepresentationType::Pointer 
       boxObjectRepresentation2 = BoxRepresentationType::New();
  boxObjectRepresentation2->SetLogger ( logger );

  double size_X2;
  double size_Y2;
  double size_Z2;

  size_X2 = 30.0;
  size_Y2 = 30.0;
  size_Z2 = 2.0;

  boxObject2->SetSize( size_X2, size_Y2, size_Z2 );

  igstk::Transform                          transform2;
  igstk::Transform::VectorType              translation2;

  translation2[0] = -153.66 + 512*0.6718;
  translation2[1] = -307.66;
  translation2[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue2 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime2 = 1e20;
  
  transform2.SetTranslation( translation2, errorValue2, validtyTime2 ); 

  boxObject2->RequestSetTransformAndParent( transform2, worldReference );

  boxObjectRepresentation2->RequestSetBoxObject( boxObject2 );
  boxObjectRepresentation2->SetColor( 1.0, 1.0, 1.0 );
  boxObjectRepresentation2->SetOpacity( 1.0 );

 //Bottom Left corner
  BoxObjectType::Pointer  boxObject3 = BoxObjectType::New();
  boxObject3->SetLogger ( logger );
  
  BoxRepresentationType::Pointer 
       boxObjectRepresentation3 = BoxRepresentationType::New();
  boxObjectRepresentation3->SetLogger ( logger );

  double size_X3;
  double size_Y3;
  double size_Z3;

  size_X3 = 40.0;
  size_Y3 = 40.0;
  size_Z3 = 2.0;

  boxObject3->SetSize( size_X3, size_Y3, size_Z3 );

  igstk::Transform                          transform3;
  igstk::Transform::VectorType              translation3;

  translation3[0] = -153.66;
  translation3[1] = -307.66 + 512*0.6718;
  translation3[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue3 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime3 = 1e20;
  
  transform3.SetTranslation( translation3, errorValue3, validtyTime3 ); 
      
  boxObject3->RequestSetTransformAndParent( transform3, worldReference );

  boxObjectRepresentation3->RequestSetBoxObject( boxObject3 );
  boxObjectRepresentation3->SetColor( 1.0, 1.0, 1.0 );
  boxObjectRepresentation3->SetOpacity( 1.0 );

  // Bottom right corner
  BoxObjectType::Pointer  boxObject4 = BoxObjectType::New();
  boxObject4->SetLogger ( logger );
  
  BoxRepresentationType::Pointer 
       boxObjectRepresentation4 = BoxRepresentationType::New();
  boxObjectRepresentation4->SetLogger ( logger );

  double size_X4;
  double size_Y4;
  double size_Z4;

  size_X4 = 50.0;
  size_Y4 = 50.0;
  size_Z4 = 2.0;

  boxObject4->SetSize( size_X4, size_Y4, size_Z4 );

  igstk::Transform                          transform4;
  igstk::Transform::VectorType              translation4;

  translation4[0] = -153.66 + 512*0.6718;
  translation4[1] = -307.66 + 512*0.6718;
  translation4[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue4 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime4 = 1e20;
  
  transform4.SetTranslation( translation4, errorValue4, validtyTime4 ); 
      
  boxObject4->RequestSetTransformAndParent( transform4, worldReference );

  boxObjectRepresentation4->RequestSetBoxObject( boxObject4 );
  boxObjectRepresentation4->SetColor( 1.0, 1.0, 1.0 );
  boxObjectRepresentation4->SetOpacity( 1.0 );

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

  igstk::Transform identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  view2D->RequestSetTransformAndParent( identityTransform, worldReference );

  // Add the image object representation to the view
  view2D->RequestAddObject( imageRepresentation );

  // Add the box object representation to the view 
  view2D->RequestAddObject( boxObjectRepresentation );
  view2D->RequestAddObject( boxObjectRepresentation2 );
  view2D->RequestAddObject( boxObjectRepresentation3 );
  view2D->RequestAddObject( boxObjectRepresentation4 );

  reader->RequestGetImage();
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::CTImageSpatialObject::Pointer ImagePointer;
  ImagePointer imageSpatialObject = ctImageObserver->GetCTImage(); 

  imageSpatialObject->SetLogger( logger );

  transform.SetToIdentity( validtyTime );

  imageSpatialObject->RequestSetTransformAndParent( transform, worldReference );

  imageRepresentation->RequestSetImageSpatialObject( imageSpatialObject );

  imageRepresentation->RequestSetOrientation( ImageRepresentationType::Axial );
  
  view2D->RequestSetOrientation( igstk::View2D::Axial );

  imageRepresentation->RequestSetSliceNumber( 0 );
  
  view2D->RequestResetCamera();
  view2D->SetRefreshRate( 30 );
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
  igstk::PulseGenerator::Sleep(500);
  view2D->RequestStop();
  igstk::PulseGenerator::Sleep(500);
  view2D->RequestSaveScreenShot( filename );

  delete widget2D;
  delete form;
 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
