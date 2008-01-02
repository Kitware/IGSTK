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
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkPolarisTrackerTool.h"


int main(int , char** )
{ 

  igstk::RealTimeClock::Initialize();

  OneViewAndTrackingNewUsingFLTKWidgetImplementation   application;

  // define a tracker tool type
  typedef igstk::PolarisTrackerTool      TrackerToolType;

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( application.GetLogger() );
  application.Display3D->RequestSetView( view3D );
  application.Display3D->RequestEnableInteractions();

  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(200,200,300); // about a human skull
  
  double validityTimeInMilliseconds = igstk::TimeStamp::GetLongestPossibleTime(); 
  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer 
        ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);
  view3D->RequestAddObject( ellipsoidRepresentation );

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(1.0);
  cylinder->SetHeight(50.0);

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);
  view3D->RequestAddObject( cylinderRepresentation );

  // Make axes for the tracker coordinates.
  igstk::AxesObject::Pointer trackerAxes = igstk::AxesObject::New();
  trackerAxes->SetSize(300, 300, 300);

  igstk::AxesObjectRepresentation::Pointer trackerAxesRepresentation =
    igstk::AxesObjectRepresentation::New();
  trackerAxesRepresentation->RequestSetAxesObject( trackerAxes );
  view3D->RequestAddObject( trackerAxesRepresentation );

  // Make axes to display the tracker tool coordinates.
  igstk::AxesObject::Pointer toolAxes = igstk::AxesObject::New();
  toolAxes->SetSize( 300, 200, 100 );

  igstk::AxesObjectRepresentation::Pointer toolAxesRepresentation = 
    igstk::AxesObjectRepresentation::New();
  toolAxesRepresentation->RequestSetAxesObject( toolAxes );
  view3D->RequestAddObject( toolAxesRepresentation );


  // Create tracker tool and attach it to the tracker
  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool =  TrackerToolType::New();
  trackerTool->SetLogger( application.GetLogger() );
  //Select wired tracker tool
  trackerTool->RequestSelectWiredTrackerTool();
  //Set the port number to zero
  trackerTool->RequestSetPortNumber( 0 );
  // add the tool to the tracker
  application.AddTool( trackerTool );

  /** Attach axes to the camera coordinates. */
  double aLongTime = igstk::TimeStamp::GetLongestPossibleTime();

  igstk::Transform identity;
  identity.SetToIdentity( aLongTime );
 
  trackerAxes->RequestSetTransformAndParent( 
                                     identity, 
                                     application.GetTracker().GetPointer() );
  
  /** Attache axes to the tool coordinates. */
  toolAxes->RequestSetTransformAndParent( identity, 
                                          trackerTool.GetPointer() );

  /** Attach the view to the tracker coordinates */
  view3D->RequestSetTransformAndParent( 
                                    identity, 
                                    application.GetTracker().GetPointer() );
 
  view3D->RequestResetCamera();

  application.Show();

  // insert a delay
  Fl::wait(0.1);

  // Set the refresh rate and start 
  // the pulse generators of the views.
  view3D->SetRefreshRate( 60 );
  view3D->RequestStart();


  while( !application.HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
    }

  return EXIT_SUCCESS;
}
