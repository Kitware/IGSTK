/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FourViewsAndTracking.cxx
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

#include "FourViewsAndTrackingImplementation.h"

using namespace igstk;

int main(int , char** )
{ 

  igstk::RealTimeClock::Initialize();

  FourViewsAndTrackingImplementation   application;


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
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  cylinder->RequestSetTransformAndParent( identity, ellipsoid );

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);


  // Create another cylinder 
  igstk::CylinderObject::Pointer cylinder2 = igstk::CylinderObject::New();
  cylinder2->SetRadius(30.0);
  cylinder2->SetHeight(300.0);  // about the size of a needle

  igstk::Transform cylinder2Transform;
  igstk::Transform::VersorType rotation2;
  rotation2.Set( 0.0, 1.0, 0.0, 1.0 );

  cylinder2Transform.SetRotation( 
    rotation2, errorValue, igstk::TimeStamp::GetLongestPossibleTime() );

  // Create another cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation2 = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation2->RequestSetCylinderObject( cylinder2 );
  cylinderRepresentation2->SetColor(1.0,1.0,0.0);
  cylinderRepresentation2->SetOpacity(0.6);

  cylinder2->RequestSetTransformAndParent( cylinder2Transform, ellipsoid );

  // Add the ellipsoid representations to the views
  application.AddEllipsoid( ellipsoidRepresentation );
  application.AddCylinder(  cylinderRepresentation  );
  application.AddCylinder(  cylinderRepresentation2  );

  // Associate the Spatial Object to the tracker
  application.AttachObjectToTrack( cylinder );

  application.ConnectViewsToSpatialObjectParent( ellipsoid );

  application.ResetCameras();

  // Show the main GUI window
  application.Show();

  while( !application.HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
    }


  return EXIT_SUCCESS;
}
