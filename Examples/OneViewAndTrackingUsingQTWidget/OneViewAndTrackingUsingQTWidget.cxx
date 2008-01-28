/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidget.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

//QT header file
#include <QApplication>
#include <QtTest/QTest>

#include "OneViewAndTrackingUsingQTWidgetGUI.h"
#include "igstkView3D.h"
#include "igstkVTKLoggerOutput.h"

int main(int argc, char** argv)
{ 

  igstk::RealTimeClock::Initialize();
  QApplication app(argc, argv);

  OneViewAndTrackingUsingQTWidgetGUI   mainWindow;

  typedef igstk::Object::LoggerType              LoggerType;
  typedef itk::StdStreamLogOutput                LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger


  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(200,200,300); // about a human skull
  
  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer 
        ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);
  ellipsoidRepresentation->SetLogger( logger );

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(1.0);
  cylinder->SetHeight(300.0);  // about the size of a needle

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);
  cylinderRepresentation->SetLogger( logger );

  double aLongTime = 1e300; // in seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0.0;
  translation[1] = 0.0;
  translation[2] = 0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );
  igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, aLongTime );


  translation[1] = -0.25;  // translate the cylinder along Y
  translation[2] = -2.00;  // translate the cylinder along Z
  rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

  transform.SetTranslationAndRotation( 
  translation, rotation, errorValue, aLongTime );

  // cylinder coordinates are relative to the ellipse
  cylinder->RequestSetTransformAndParent( transform, ellipsoid );

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();

  view3D->SetLogger( logger );
  mainWindow.SetView( view3D );

  //attach the view to the tracker coordinates
  igstk::Transform identity;
  identity.SetToIdentity( aLongTime );
 
  view3D->RequestSetTransformAndParent( 
            identity, mainWindow.GetTracker());

  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );

  //Associate the Spatial Object to the tracker
  mainWindow.AttachObjectToTrack( ellipsoid );
  view3D->SetRefreshRate( 10 );

  view3D->RequestResetCamera();
  mainWindow.show();
  view3D->RequestStart();
  
  while(! mainWindow.HasQuitted())
    {
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  return EXIT_SUCCESS;
}
