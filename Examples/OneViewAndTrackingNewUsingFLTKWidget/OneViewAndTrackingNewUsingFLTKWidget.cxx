/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingNewUsingFLTKWidget.cxx
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

#include "OneViewAndTrackingNewUsingFLTKWidgetImplementation.h"
#include "igstkView3D.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"


int main(int , char** )
{ 

  igstk::RealTimeClock::Initialize();

  OneViewAndTrackingNewUsingFLTKWidgetImplementation   application;

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


  igstk::Transform transformToView;
  igstk::Transform::VectorType translationToView;
  translationToView[0] = 0.0;
  translationToView[1] = 0.0;
  translationToView[2] = 0.0;
  igstk::Transform::VersorType rotationToView;
  rotationToView.Set( 0.0, 1.0, 0.0, 1.0 );
  transformToView.SetTranslationAndRotation(
      translationToView, rotationToView, errorValue, validityTimeInMilliseconds );

  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer 
        ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(1.0);
  //cylinder->SetHeight(300.0);  // about the size of a needle
  cylinder->SetHeight(50.0);  // about the size of a needle

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();

  /** We pass the bare pointer so that the compiler can figure out the type
      for the templated method. */
  ellipsoid->RequestSetTransformAndParent( transformToView, 
                                           view3D.GetPointer() );

  /* cylinder->RequestSetTransformAndParent( transform, 
                                          ellipsoid.GetPointer() ); */

  // Make the view the parent of the tracker 
  application.AttachTrackerToView( view3D ); 


  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );
 

  application.Display3D->RequestSetView( view3D );

  application.Show();

  // Set the refresh rate and start 
  // the pulse generators of the views.

  view3D->SetRefreshRate( 30 );
  view3D->RequestStart();
  //view3D->SetCameraPosition(0.0, 0.0, -600.0);
  view3D->SetCameraPosition(-225.0,100.00,-1600.0);


  // Associate the Spatial Object to the tracker tool
  application.AttachObjectToTrackerTool ( cylinder );

  igstk::Transform             toolTransform; 
  igstk::Transform::VectorType position;

  while( !application.HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();

    application.GetTrackerToolTransform( toolTransform );

    position = toolTransform.GetTranslation();
    std::cout << "Trackertool:" 
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    std::cout << "CylinderSpatialObject:"
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    }

  return EXIT_SUCCESS;
}
