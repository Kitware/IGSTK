/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest2.cxx
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
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
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
  vtkLoggerOutput->SetLogger(logger);

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
  ImageRepresentationType::Pointer imageRepresentation 
                                            = ImageRepresentationType::New();
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

  /* Set up the cylinder spatial object that will be added to each 
   * each corners of the image */
  
  // Top Left corner
  typedef igstk::CylinderObject CylinderObjectType;

  CylinderObjectType::Pointer  cylinderObject = CylinderObjectType::New();
  cylinderObject->SetLogger ( logger );
  
  typedef igstk::CylinderObjectRepresentation CylinderRepresentationType;
  CylinderRepresentationType::Pointer 
       cylinderObjectRepresentation = CylinderRepresentationType::New();
  cylinderObjectRepresentation->SetLogger ( logger );

  double radius;
  radius= 20.0;
  cylinderObject->SetRadius( radius );

  double height;
  height = 2;
  cylinderObject->SetHeight( height );
 
  igstk::Transform                          transform;
  igstk::Transform::VectorType              translation;

  translation[0] = -153.66;
  translation[1] = -307.66;
  translation[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime = 1e20;
  
  transform.SetTranslation( translation, errorValue, validtyTime ); 
      
  cylinderObject->RequestSetTransform( transform );
  
  cylinderObjectRepresentation->RequestSetCylinderObject( cylinderObject );
  cylinderObjectRepresentation->SetColor( 1.0, 1.0, 1.0 );
  cylinderObjectRepresentation->SetOpacity( 1.0 );

  //Top Right corner
  CylinderObjectType::Pointer  cylinderObject2 = CylinderObjectType::New();
  cylinderObject2->SetLogger ( logger );
  
  CylinderRepresentationType::Pointer 
       cylinderObjectRepresentation2 = CylinderRepresentationType::New();
  cylinderObjectRepresentation2->SetLogger ( logger );

  double radius2;
  radius2= 20.0;
  cylinderObject2->SetRadius( radius2 );

  double height2;
  height2 = 2;
  cylinderObject2->SetHeight( height2 );

  igstk::Transform                          transform2;
  igstk::Transform::VectorType              translation2;

  translation2[0] = -153.66 + 512*0.6718;
  translation2[1] = -307.66;
  translation2[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue2 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime2 = 1e20;
  
  transform2.SetTranslation( translation2, errorValue2, validtyTime2 ); 
      
  cylinderObject2->RequestSetTransform( transform2 );
  
  cylinderObjectRepresentation2->RequestSetCylinderObject( cylinderObject2 );
  cylinderObjectRepresentation2->SetColor( 1.0, 1.0, 1.0 );
  cylinderObjectRepresentation2->SetOpacity( 1.0 );

 //Bottom Left corner
  CylinderObjectType::Pointer  cylinderObject3 = CylinderObjectType::New();
  cylinderObject3->SetLogger ( logger );
  
  CylinderRepresentationType::Pointer 
       cylinderObjectRepresentation3 = CylinderRepresentationType::New();
  cylinderObjectRepresentation3->SetLogger ( logger );

  double radius3;
  radius3= 20.0;
  cylinderObject3->SetRadius( radius3 );

  double height3;
  height3 = 2;
  cylinderObject3->SetHeight( height3 );

  igstk::Transform                          transform3;
  igstk::Transform::VectorType              translation3;

  translation3[0] = -153.66;
  translation3[1] = -307.66 + 512*0.6718;
  translation3[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue3 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime3 = 1e20;
  
  transform3.SetTranslation( translation3, errorValue3, validtyTime3 ); 
      
  cylinderObject3->RequestSetTransform( transform3 );
  
  cylinderObjectRepresentation3->RequestSetCylinderObject( cylinderObject3 );
  cylinderObjectRepresentation3->SetColor( 1.0, 1.0, 1.0 );
  cylinderObjectRepresentation3->SetOpacity( 1.0 );

  //Bottom right corner
  CylinderObjectType::Pointer  cylinderObject4 = CylinderObjectType::New();
  cylinderObject4->SetLogger ( logger );
  
  CylinderRepresentationType::Pointer 
       cylinderObjectRepresentation4 = CylinderRepresentationType::New();
  cylinderObjectRepresentation4->SetLogger ( logger );

  double radius4;
  radius4= 20.0;
  cylinderObject4->SetRadius( radius4 );

  double height4;
  height4 = 2;
  cylinderObject4->SetHeight( height4 );

  igstk::Transform                          transform4;
  igstk::Transform::VectorType              translation4;

  translation4[0] = -153.66 + 512*0.6718;
  translation4[1] = -307.66 + 512*0.6718;
  translation4[2] = -176.0;
 
  igstk::Transform::ErrorType            errorValue4 = 1e-20;
  igstk::Transform::TimePeriodType       validtyTime4 = 1e20;
  
  transform4.SetTranslation( translation4, errorValue4, validtyTime4 ); 
      
  cylinderObject4->RequestSetTransform( transform4 );
  
  cylinderObjectRepresentation4->RequestSetCylinderObject( cylinderObject4 );
  cylinderObjectRepresentation4->SetColor( 1.0, 1.0, 1.0 );
  cylinderObjectRepresentation4->SetOpacity( 1.0 );

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
  view2D->RequestAddObject( cylinderObjectRepresentation );
  view2D->RequestAddObject( cylinderObjectRepresentation2 );
  view2D->RequestAddObject( cylinderObjectRepresentation3 );
  view2D->RequestAddObject( cylinderObjectRepresentation4 );

  reader->RequestGetImage();
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  imageRepresentation->RequestSetImageSpatialObject( 
                                               ctImageObserver->GetCTImage() );
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
