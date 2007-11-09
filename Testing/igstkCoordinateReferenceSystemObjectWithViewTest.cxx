/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemObjectWithViewTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
// FIXCS #include "igstkCoordinateReferenceSystemObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkViewNew2D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkTransformObserverTestHelper.h"

int igstkCoordinateReferenceSystemObjectWithViewTest( int argc, char * argv [] )
{
 
  std::string ScreenShotFilename;

  if( argc > 1 )
    {
    ScreenShotFilename = argv[1];
    }

  igstk::RealTimeClock::Initialize();

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
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger
  
  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer AxesRepresentation = RepresentationType::New();
  AxesRepresentation->SetLogger( logger );

  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject WorldReferenceSystemType;
  WorldReferenceSystemType::Pointer worldReference = WorldReferenceSystemType::New();
  */

  /* FIXCS
  typedef igstk::CoordinateReferenceSystemObject  ObjectType;
  ObjectType::Pointer coordinateSystem = ObjectType::New();
  coordinateSystem->SetLogger( logger );
    
  coordinateSystem->RequestAttachToSpatialObjectParent( worldReference );

  // Test Set/GetRadius()
  std::cout << "Testing Set/GetSize() : ";
  coordinateSystem->SetSize(10,20,30);

  if(coordinateSystem->GetSizeX() != 10.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX() 
              << " v.s " << 10.0 << std::endl; 
    return EXIT_FAILURE;
    }
   
  if(coordinateSystem->GetSizeY() != 20.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX()
              << " v.s " << 20.0 << std::endl; 
    return EXIT_FAILURE;
    }
 
  if(coordinateSystem->GetSizeZ() != 30.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX() 
              << " v.s " << 30.0 << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;
  */

  // Test Property
  std::cout << "Testing Property : ";
  AxesRepresentation->SetColor(0.1,0.2,0.3);
  AxesRepresentation->SetOpacity(0.4);
  if(fabs(AxesRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cerr << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(AxesRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  // FIXCS AxesRepresentation->RequestSetAxesObject( coordinateSystem );
  AxesRepresentation->Print(std::cout);
  // FIXCS coordinateSystem->Print(std::cout);
  // FIXCS coordinateSystem->GetNameOfClass();
  AxesRepresentation->GetNameOfClass();

  // testing actors
  std::cout << "Testing actors : ";

  typedef igstk::ViewNew2D  View2DType;

  View2DType::Pointer view2D = View2DType::New();
  view2D->SetLogger( logger );
    
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(301,301,"CoordinateReferenceSystemObjectWithViewTest");
  FLTKWidgetType * fltkWidget2D = new FLTKWidgetType(0,0,300,300,"View 2D");

  fltkWidget2D->RequestSetView( view2D );

  form->end();
  // End of the GUI creation

  std::cout << "[PASSED]" << std::endl;

  form->show();

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  // FIXCS coordinateSystem->SetSize( 20.0, 30.0, 40.0 );
  
  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 6e5; // 600 seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 0;
  translation[2] = 0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );


  typedef igstk::TransformObserverTestHelper TransformObserverType;

  TransformObserverType::Pointer transformObserver 
                                               = TransformObserverType::New();

  /* FIXCS 
  coordinateSystem->AddObserver( ::igstk::TransformModifiedEvent(), 
                                                          transformObserver );

  coordinateSystem->RequestSetTransformToSpatialObjectParent( transform );
  coordinateSystem->RequestGetTransformToWorld();
  */
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The CoordinateReferenceSystemObject did not returned a Transform event" 
              << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();

  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" 
                << std::endl;
      std::cerr << "input  translation = " << translation << std::endl;
      std::cerr << "output translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
      fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
      fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
      fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "Rotation component is out of range [FAILED]" << std::endl;
    std::cerr << "input  rotation = " << rotation << std::endl;
    std::cerr << "output rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;

  view2D->SetRefreshRate( 30 );
  view2D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view2D->SetCameraPosition( 100.0, 100.0, 100.0 );
  view2D->SetFocalPoint( 0.0, 0.0, 0.0 );
  view2D->SetCameraViewUp( 0, 0, 1.0 );

  // this will indirectly call CreateActors() 
  // FIXCS view2D->RequestAddObject( AxesRepresentation );
  view2D->RequestResetCamera();
  view2D->RequestStart();

  // Do manual redraws
  for(unsigned int i=0; i<50; i++)
    {
    Fl::wait(0.01);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  //
  // Add three other coordinate reference systems
  // and position and rotate them avay from the main 
  // coordinate system.
  //
  /*
  FIXCS 
  ObjectType::Pointer coordinateSystemA = ObjectType::New();
  coordinateSystemA->RequestAttachToSpatialObjectParent( coordinateSystem );

  ObjectType::Pointer coordinateSystemB = ObjectType::New();
  coordinateSystemB->RequestAttachToSpatialObjectParent( coordinateSystem );

  ObjectType::Pointer coordinateSystemC = ObjectType::New();
  coordinateSystemC->RequestAttachToSpatialObjectParent( coordinateSystem );
  */

  igstk::Transform transformA;
  igstk::Transform transformB;
  igstk::Transform transformC;

  translation[0] = 10.0;
  translation[1] =  0.0;
  translation[2] =  0.0;
  rotation.Set( 0.2588, 0.0, 0.0, 0.9659 );

  transformA.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  translation[0] =  0.0;
  translation[1] = 20.0;
  translation[2] =  0.0;
  rotation.Set( 0.0, 0.2588, 0.0, 0.9659 );

  transformB.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  translation[0] =  0.0;
  translation[1] =  0.0;
  translation[2] = 30.0;
  rotation.Set( 0.0, 0.0, 0.2588, 0.9659 );

  transformC.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  /*
  FIXCS 
  coordinateSystemA->RequestSetTransformToSpatialObjectParent( transformA );
  coordinateSystemB->RequestSetTransformToSpatialObjectParent( transformB );
  coordinateSystemC->RequestSetTransformToSpatialObjectParent( transformC );

  coordinateSystemA->SetSize(10,10,10);
  coordinateSystemB->SetSize(10,10,10);
  coordinateSystemC->SetSize(10,10,10);
  */

  RepresentationType::Pointer AxesRepresentationA = RepresentationType::New();
  RepresentationType::Pointer AxesRepresentationB = RepresentationType::New();
  RepresentationType::Pointer AxesRepresentationC = RepresentationType::New();

  /* FIXCS
  AxesRepresentationA->RequestSetAxesObject( coordinateSystemA );
  AxesRepresentationB->RequestSetAxesObject( coordinateSystemB );
  AxesRepresentationC->RequestSetAxesObject( coordinateSystemC );
  */

  view2D->RequestAddObject( AxesRepresentationA );
  view2D->RequestAddObject( AxesRepresentationB );
  view2D->RequestAddObject( AxesRepresentationC );

  // Do manual redraws
  for(unsigned int i=0; i<50; i++)
    {
    Fl::wait(0.01);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  // Save the new scene
  if( argc > 1 )
    {
    view2D->RequestSaveScreenShot( ScreenShotFilename );
    }

  delete fltkWidget2D;
  delete form;

  std::cout << "Test [DONE]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
