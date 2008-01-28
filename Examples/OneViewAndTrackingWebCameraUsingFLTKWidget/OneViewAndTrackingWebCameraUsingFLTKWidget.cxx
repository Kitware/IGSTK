/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingWebCameraUsingFLTKWidget.cxx
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

#include "OneViewAndTrackingWebCameraUsingFLTKWidgetImplementation.h"
#include "igstkView3D.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkWebCameraTrackerTool.h"


int main(int , char** )
{ 

  igstk::RealTimeClock::Initialize();

  OneViewAndTrackingWebCameraUsingFLTKWidgetImplementation   application;

  // define a tracker tool type
  typedef igstk::WebCameraTrackerTool      TrackerToolType;

  // instantiate a 3D view 
  typedef igstk::View3D        View3DType;
  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( application.GetLogger() );
  application.Display3D->RequestSetView( view3D );
  application.Display3D->RequestEnableInteractions();

  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(200,200,300); // about a human skull
  
  double validityTimeInMilliseconds = 
               igstk::TimeStamp::GetLongestPossibleTime(); 

  // Create a box representing the active 4 marker planar rigid body
  // probe
  igstk::BoxObject::Pointer box = igstk::BoxObject::New();
  box->SetSize(10, 100, 100); // long down x

  igstk::BoxObjectRepresentation::Pointer
          boxRepresentation = igstk::BoxObjectRepresentation::New();
  boxRepresentation->RequestSetBoxObject( box );
  boxRepresentation->SetColor( 1.0, 1.0, 0.0 );
  boxRepresentation->SetOpacity( 1.0 );
  view3D->RequestAddObject( boxRepresentation );

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(10.0);
  cylinder->SetHeight(300.0);

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
  //Set the tool name
  trackerTool->RequestSetName( "WhiteLed" );
  // add the tool to the tracker
  application.AddTool( trackerTool );

  /** Attach axes to the camera coordinates. */
  double aLongTime = igstk::TimeStamp::GetLongestPossibleTime();

  igstk::Transform identity;
  identity.SetToIdentity( aLongTime );
 
  trackerAxes->RequestSetTransformAndParent( 
                                     identity, 
                                     application.GetTracker() );
  
  /** Attache axes to the tool coordinates. */
  toolAxes->RequestSetTransformAndParent( identity, 
                                          trackerTool );
  /** Attach the box representing the tracker tool to the tracker tool 
   *  coordinates.
   */
  box->RequestSetTransformAndParent( identity, trackerTool );

  /** Attach the cylinder coordinates the box. 
   *  The rotation is used to rotate the cylinder so that
   *  it is pointing down the long (x) axis of the box.
   */
  const double err = 1e-5;
  igstk::Transform cylinderToBoxTransform;
  cylinderToBoxTransform.SetToIdentity( aLongTime );
  igstk::Transform::VersorType cylinderVersor;
  cylinderVersor.Set(0, 1, 0, 0); 
  cylinderToBoxTransform.SetRotation( cylinderVersor, err, aLongTime );
  cylinder->RequestSetTransformAndParent( cylinderToBoxTransform, box );

  /** Attach the view to the tracker coordinates */
  view3D->RequestSetTransformAndParent( 
                                    identity, 
                                    application.GetTracker() );
  view3D->SetCameraFocalPoint( 0.0, 0.0 , 0.0 );
  view3D->SetCameraPosition( 0.0, 0.0 , 600.0 );
  view3D->SetCameraViewUp(  0.0, 1.0 , 0.0 );
  view3D->SetCameraClippingRange( 50.0 , 9000.0 );
  view3D->SetCameraParallelProjection( false );

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
