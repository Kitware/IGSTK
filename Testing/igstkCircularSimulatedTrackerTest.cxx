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
#include "igstkView3D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkCircularSimulatedTracker.h"

int igstkCircularSimulatedTrackerTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();

  //
  // Create a Axes object to provide a visual reference
  //
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer axesObject = AxesObjectType::New();

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

  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CircularSimulatedTracker     TrackerType;
  typedef igstk::TrackerTool                  TrackerToolType;

  TrackerType::Pointer      tracker     = TrackerType::New();
  TrackerToolType::Pointer  trackerTool = TrackerToolType::New();

  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  typedef igstk::BoxObject                ToolObjectType;
  typedef igstk::BoxObjectRepresentation  ToolRepresentationType;

  ToolObjectType::Pointer toolObject = ToolObjectType::New();
  ToolRepresentationType::Pointer toolRepresentation = ToolRepresentationType::New();

  toolRepresentation->RequestSetBoxObject( toolObject );

  // Connect the objects in the scene to a coordinate reference system.
  tracker->RequestSetTransformAndParent( transform, axesObject.GetPointer() );
  toolObject->RequestSetTransformAndParent( transform, trackerTool.GetPointer() );
  view3D->RequestSetTransformAndParent( transform, axesObject.GetPointer() );

  tracker->RequestOpen();
  tracker->RequestStartTracking();

  view3D->SetRefreshRate( 30 );
  view3D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view3D->SetCameraPosition( 100.0, 100.0, 100.0 );
  view3D->SetFocalPoint( 0.0, 0.0, 0.0 );
  view3D->SetCameraViewUp( 0, 0, 1.0 );

  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( toolRepresentation );

  view3D->RequestResetCamera();
  view3D->RequestStart();

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
