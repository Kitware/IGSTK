/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCircularSimulatedTrackerTest.cxx
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
#include "igstkBoxObject.h"
#include "igstkBoxObjectRepresentation.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkSimulatedTrackerTool.h"
#include "igstkCircularSimulatedTracker.h"

int igstkCircularSimulatedTrackerTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();

  //
  // Create a Axes object to provide a visual reference
  //
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer axesObject = AxesObjectType::New();
  axesObject->SetSize(3.0, 3.0, 3.0);

  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer axesRepresentation = RepresentationType::New();

  axesRepresentation->RequestSetAxesObject( axesObject );

  typedef igstk::View3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
    
  typedef igstk::FLTKWidget      FLTKWidgetType;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(600,600,"Circular Tracker");
  FLTKWidgetType * fltkWidget = new FLTKWidgetType(0,0,600,600,"FLTKWidget");

  fltkWidget->RequestSetView( view3D );

  form->end();
  // End of the GUI creation

  form->show();

  Fl::wait(0.5);

  typedef igstk::CircularSimulatedTracker     TrackerType;
  typedef igstk::SimulatedTrackerTool         TrackerToolType;

  TrackerType::Pointer      tracker     = TrackerType::New();

  const double speedInDegreesPerSecond = 36.0;
  const double radiusInMillimeters = 2.0;

  tracker->RequestOpen();
  tracker->SetRadius( radiusInMillimeters );
  tracker->GetRadius(); // coverage
  tracker->SetAngularSpeed( speedInDegreesPerSecond );
  tracker->GetAngularSpeed(); // coverage;
  tracker->RequestSetFrequency( 100.0 );

  tracker->Print( std::cout );

  TrackerToolType::Pointer  trackerTool = TrackerToolType::New();

  trackerTool->RequestSetName("Circle1");
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  typedef igstk::BoxObject                ToolObjectType;
  typedef igstk::BoxObjectRepresentation  ToolRepresentationType;

  ToolObjectType::Pointer toolObject = ToolObjectType::New();
  toolObject->SetSize( 1.0, 1.0, 1.0 );

  ToolRepresentationType::Pointer 
              toolRepresentation = ToolRepresentationType::New();
  toolRepresentation->RequestSetBoxObject( toolObject );
  toolRepresentation->SetColor( 1.0, 0.5, 0.5 );

  typedef igstk::CylinderObject                TargetObjectType;
  typedef igstk::CylinderObjectRepresentation  TargetRepresentationType;

  TargetObjectType::Pointer targetObject = TargetObjectType::New();
  targetObject->SetRadius( 0.1 );
  targetObject->SetHeight( 2.0 );

  TargetRepresentationType::Pointer 
            targetRepresentation = TargetRepresentationType::New();
  targetRepresentation->RequestSetCylinderObject( targetObject );
  targetRepresentation->SetColor( 0.5, 0.5, 1.0 );

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  igstk::Transform cylinderTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  translation[0] =    0;
  translation[1] =    0;
  translation[2] = -1.0;
  rotation.Set(0.0, 0.0, 0.0, 1.0);
  const double transformUncertainty = 1.0;
  cylinderTransform.SetTranslation(
                         translation, 
                         transformUncertainty,
                         igstk::TimeStamp::GetLongestPossibleTime() );

  // Connect the objects in the scene to a coordinate reference system.
  tracker->RequestSetTransformAndParent( identity, axesObject );
  toolObject->RequestSetTransformAndParent( identity, trackerTool );
  targetObject->RequestSetTransformAndParent( cylinderTransform, axesObject );
  view3D->RequestSetTransformAndParent( identity, axesObject );

  view3D->SetRefreshRate( 30 );
  view3D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view3D->SetCameraPosition( 10.0, 5.0, 3.0 );
  view3D->SetCameraFocalPoint( 0.0, 0.0, 0.0 );
  view3D->SetCameraViewUp( 0, 0, 1.0 );

  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( toolRepresentation );
  view3D->RequestAddObject( targetRepresentation );

  view3D->RequestStart();

  for( unsigned int i = 0; i < 100; i++ )
    {
    Fl::wait(0.01);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();
    }

  tracker->RequestStartTracking();

  view3D->RequestStop();
  tracker->RequestStopTracking();

  toolRepresentation->SetColor( 0.5, 1.0, 0.5 );

  view3D->RequestStart();
  tracker->RequestStartTracking();

  // Show now the cube being tracked
  for( unsigned int i = 0; i < 3000; i++ )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();
    }

  view3D->RequestStop();
  tracker->RequestStopTracking();

  tracker->RequestClose();

  form->hide();

  delete fltkWidget;
  delete form;

  return EXIT_SUCCESS;
}
