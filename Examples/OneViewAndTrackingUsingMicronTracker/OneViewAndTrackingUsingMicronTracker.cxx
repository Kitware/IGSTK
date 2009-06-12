/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingMicronTracker.cxx
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

#include "OneViewAndTrackingUsingMicronTrackerImplementation.h"
#include "igstkView3D.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"


int main(int argc, char** argv )
{ 

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "MicronTracker_Camera_Calibration_file" << "\t"
                            << "MicronTracker_initialization_file"  << "\t"
                            << "Marker_template_directory " << std::endl; 
    return EXIT_FAILURE;
    }

 
  OneViewAndTrackingUsingMicronTrackerImplementation   application;

  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(200,200,300); // about a human skull
  
  double validityTimeInMilliseconds = 1e20; // in seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0.0;
  translation[1] = 0.0;
  translation[2] = 0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  std::cout << "Transform to static ellipsoid = " << transform << std::endl;

  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer 
        ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(1.0);
  cylinder->SetHeight(300.0);

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();

  // set a logger
  view3D->SetLogger( application.GetLogger() );

  // Make the view the parent of the tracker 
  application.AttachTrackerToView( view3D ); 

  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );
 
  application.Display3D->RequestSetView( view3D );
  application.Show();

  std::string  CameraCalibrationFileDirectory = argv[1];
  std::string InitializationFile = argv[2];
  std::string markerTemplateDirectory = argv[3];

  application.InitializeTracker( 
    InitializationFile, 
    CameraCalibrationFileDirectory, 
    markerTemplateDirectory );

  application.ConfigureTrackerToolsAndAttachToTheTracker();


  // Associate the cylinder spatial object to the first tracker tool 
  application.AttachObjectToTrackerTool ( 1, cylinder );

  // Associate the ellispsoid spatial object to the second tracker tool 
  application.AttachObjectToTrackerTool ( 2, ellipsoid );


  igstk::Transform             toolTransform; 
  igstk::Transform::VectorType position;

  // Set the refresh rate and start 
  // the pulse generators of the views.

  view3D->SetRefreshRate( 30 );
  view3D->RequestResetCamera();

  view3D->RequestStart();


  while( !application.HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();

    if( application.IsTrackingTurnedOn())
      {
      application.GetTrackerToolTransform( 1, toolTransform );

      position = toolTransform.GetTranslation();
      std::cout << "Trackertool1:" 
                << "  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;

      application.GetTrackerToolTransform( 2, toolTransform );

      position = toolTransform.GetTranslation();
      std::cout << "Trackertool2:" 
                << "  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
      }
    }

  return EXIT_SUCCESS;
}
