/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemObjectWithViewTest.cxx
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

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "igstkTransformObserver.h"
#include "itkStdStreamLogOutput.h"

int igstkCoordinateSystemObjectWithViewTest( int argc, char * argv [] )
{
 
  std::string ScreenShotFilename;

  if( argc > 1 )
    {
    ScreenShotFilename = argv[1];
    }

  igstk::RealTimeClock::Initialize();

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
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger
  
  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer AxesRepresentation = RepresentationType::New();
  AxesRepresentation->SetLogger( logger );

  typedef igstk::AxesObject       AxesObjectType;

  AxesObjectType::Pointer worldReference = AxesObjectType::New();


  typedef igstk::View2D  View2DType;

  View2DType::Pointer view2D = View2DType::New();
  view2D->SetLogger( logger );
    
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      WidgetType;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(301,301,"CoordinateSystemObjectWithViewTest");
  WidgetType * widget2D = new WidgetType(0,0,300,300,"View 2D");

  widget2D->RequestSetView( view2D );

  form->end();
  // End of the GUI creation

  form->show();

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";

  AxesObjectType::Pointer coordinateSystem = AxesObjectType::New();

  coordinateSystem->SetSize( 20.0, 30.0, 40.0 );
  AxesRepresentation->RequestSetAxesObject( coordinateSystem );

  
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


  typedef igstk::TransformObserver TransformObserverType;

  TransformObserverType::Pointer transformObserver
    = TransformObserverType::New();

  transformObserver->ObserveTransformEventsFrom( coordinateSystem );

  transformObserver->Clear();

  coordinateSystem->RequestSetTransformAndParent( transform, worldReference );
  coordinateSystem->RequestGetTransformToParent();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The CoordinateSystemObject did not returned a Transform event" 
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
  view2D->SetCameraFocalPoint( 0.0, 0.0, 0.0 );
  view2D->SetCameraViewUp( 0, 0, 1.0 );

  view2D->RequestSetTransformAndParent( transform, worldReference );

  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( AxesRepresentation );
  view2D->RequestResetCamera();
  view2D->RequestStart();

  // Check FLTK events first - helps views to show 
  // up inside the FLTK widget. 
  Fl::check(); 

  // Do manual redraws
  for(unsigned int i=0; i<200; i++)
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
  AxesObjectType::Pointer coordinateSystemA = AxesObjectType::New();
  AxesObjectType::Pointer coordinateSystemB = AxesObjectType::New();
  AxesObjectType::Pointer coordinateSystemC = AxesObjectType::New();

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

  coordinateSystemA->RequestSetTransformAndParent( transformA, worldReference );
  coordinateSystemB->RequestSetTransformAndParent( transformB, worldReference );
  coordinateSystemC->RequestSetTransformAndParent( transformC, worldReference );

  coordinateSystemA->SetSize(10,10,10);
  coordinateSystemB->SetSize(10,10,10);
  coordinateSystemC->SetSize(10,10,10);

  RepresentationType::Pointer AxesRepresentationA = RepresentationType::New();
  RepresentationType::Pointer AxesRepresentationB = RepresentationType::New();
  RepresentationType::Pointer AxesRepresentationC = RepresentationType::New();

  AxesRepresentationA->RequestSetAxesObject( coordinateSystemA );
  AxesRepresentationB->RequestSetAxesObject( coordinateSystemB );
  AxesRepresentationC->RequestSetAxesObject( coordinateSystemC );

  view2D->RequestAddObject( AxesRepresentationA );
  view2D->RequestAddObject( AxesRepresentationB );
  view2D->RequestAddObject( AxesRepresentationC );

  view2D->RequestResetCamera();

  // Do manual redraws
  for(unsigned int i=0; i<200; i++)
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

  delete widget2D;
  delete form;

  std::cout << "Test [DONE]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
