/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    CoordinateSystems3.cxx
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

// Software Guide : BeginLatex
//
// \index{igstk::AxesObject}
// This example describes how to build a scene graph
// with a View, Tracker, and TrackerTool. This example
// will place all the objects in a developer specified
// world coordinate system.
//
// First, we include the appropriate header files.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

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

// Software Guide : EndCodeSnippet

int main( int , char *[] )
{

// Software Guide : BeginLatex
// Then we initialize the RealTimeClock. This is necessary to manage
// the timed events in IGSTK.
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  igstk::RealTimeClock::Initialize();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Here we create an AxesObject to function as the world 
// coordinate system. We do not create a representation since
// we will not be displaying the world coordinate system object.
// The worldCoordinateSystem just provides a placeholder node
// in the scene graph.
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer worldCoordinateSystem = AxesObjectType::New();
  
// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// Now, we create an AxesObject with an AxesObjectRepresentation
// to provide a visual reference for the Tracker's coordinate system.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  AxesObjectType::Pointer axesObject = AxesObjectType::New();
  axesObject->SetSize(3.0, 3.0, 3.0);

  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer axesRepresentation = RepresentationType::New();

  axesRepresentation->RequestSetAxesObject( axesObject );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Next, we create a View3D to show our 3D scene.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef igstk::View3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// Then we create a minimal FLTK GUI and display it.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef igstk::FLTKWidget      FLTKWidgetType;

  Fl_Window * form = new Fl_Window(600,600,"Circular Tracker");
  FLTKWidgetType * fltkWidget = new FLTKWidgetType(0,0,600,600,"FLTKWidget");

  fltkWidget->RequestSetView( view3D );

  // form->begin() is automatically called in the Fl_Window constructor.
  // form->end() : Add widgets after this point to the parent of the group.
  form->end();

  form->show();

  Fl::wait(0.5);

// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// Now we make a simulated tracker and tracker tool. This way we can
// run the example without having to rely on having a particular tracker.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet

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

// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// Now we will make a BoxObject and a BoxObjectRepresentation 
// that will represent the TrackerTool in the scene. In a real 
// application, one could use a CAD model of the actual
// tracker tool.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef igstk::BoxObject                ToolObjectType;
  typedef igstk::BoxObjectRepresentation  ToolRepresentationType;

  ToolObjectType::Pointer toolObject = ToolObjectType::New();
  toolObject->SetSize( 1.0, 1.0, 1.0 );

  ToolRepresentationType::Pointer 
              toolRepresentation = ToolRepresentationType::New();
  toolRepresentation->RequestSetBoxObject( toolObject );
  toolRepresentation->SetColor( 0.5, 1.0, 0.5 );
// Software Guide : EndCodeSnippet



// Software Guide : BeginLatex
//
// Here we build up the scene graph. We define an identity transform, just
// to make things simple. 
//
// Then, we attach the AxesObject to the tracker
// using the identity transformation. As a result the AxesObject will
// be in the same position as the tracker. 
//
// Next, we connect the object representing the tracker tool to the 
// actual tracker tool. We use an identity transformation so that 
// the toolObject is in the same position as the tracker tool.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  axesObject->RequestSetTransformAndParent( identity, tracker );
  toolObject->RequestSetTransformAndParent( identity, trackerTool );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// These next lines are key differences from CoordinateSystems1.cxx
// (View From Tracker Coordinates Example) and CoordinateSystems2.cxx
// (View Follows Tracker Tool Example).
// In this example, we place the view and the 
// tracker into world coordinates using an identity transformation. In a real-world
// application, the transformation would not be the trivial identity transform.
// 
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet

  tracker->RequestSetTransformAndParent( identity, worldCoordinateSystem );
  view3D->RequestSetTransformAndParent( identity, worldCoordinateSystem );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Before we render, we also setup the view3D. We specify a refresh
// rate, background color, and camera parameters. Then we add the 
// objects we wish to display.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet

  view3D->SetRefreshRate( 30 );
  view3D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view3D->SetCameraPosition( 10.0, 5.0, 3.0 );
  view3D->SetCameraFocalPoint( 0.0, 0.0, 0.0 );
  view3D->SetCameraViewUp( 0, 0, 1.0 );

  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( toolRepresentation );


// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Start the pulse generators for the view3D and tracker.
// 
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet

  view3D->RequestStart();
  tracker->RequestStartTracking();

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Now, run for a while.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  for( unsigned int i = 0; i < 10000; i++ )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();
    }

// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// Finally, we cleanup before we exit.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  view3D->RequestStop();
  tracker->RequestStopTracking();

  tracker->RequestClose();

  form->hide();

  delete fltkWidget;
  delete form;

  return EXIT_SUCCESS;
// Software Guide : EndCodeSnippet

}
