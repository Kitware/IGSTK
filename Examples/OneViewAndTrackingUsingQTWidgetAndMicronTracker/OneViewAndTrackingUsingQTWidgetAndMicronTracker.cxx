/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidgetAndMicronTracker.cxx
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

#include "OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI.h"
#include "igstkView3D.h"
#include "igstkVTKLoggerOutput.h"

int main(int argc, char** argv)
{ 

  igstk::RealTimeClock::Initialize();
  QApplication app(argc, argv);

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "MicronTracker_Camera_Calibration_file" << "\t"
                            << "MicronTracker_initialization_file"  << "\t"
                            << "Marker_template_directory " << std::endl; 
    return EXIT_FAILURE;
    }

 

  OneViewAndTrackingUsingQTWidgetAndMicronTrackerGUI   mainWindow;

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

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();

  view3D->SetLogger( logger );

  mainWindow.AttachTrackerToView( view3D );

  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );
  view3D->SetRefreshRate( 30 );
  view3D->SetCameraPosition(-225.0,100.00,-1600.0);
  
  mainWindow.SetView( view3D );

  std::string  CameraCalibrationFileDirectory = argv[1];
  std::string InitializationFile = argv[2];
  std::string markerTemplateDirectory = argv[3];

  mainWindow.InitializeTracker( InitializationFile,
                                CameraCalibrationFileDirectory,
                                markerTemplateDirectory );

  mainWindow.ConfigureTrackerToolsAndAttachToTheTracker();

  // Associate the cylinder spatial object to the first tracker tool 
  mainWindow.AttachObjectToTrackerTool ( 1, cylinder );

  // Associate the ellispsoid spatial object to the second tracker tool 
  mainWindow.AttachObjectToTrackerTool ( 2, ellipsoid );

  view3D->RequestResetCamera();
  mainWindow.show();
  view3D->RequestStart();

  igstk::Transform             toolTransform; 
  igstk::Transform::VectorType position;

  while(! mainWindow.HasQuitted())
    {
    QTest::qWait(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  return EXIT_SUCCESS;
}
