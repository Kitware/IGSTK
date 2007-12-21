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
#include "igstkView2D.h"
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

  typedef igstk::View2D  View2DType;

  View2DType::Pointer view2D = View2DType::New();
    
  typedef igstk::FLTKWidget      FLTKWidgetType;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(301,301,"CoordinateReferenceSystemObjectWithViewTest");
  FLTKWidgetType * fltkWidget2D = new FLTKWidgetType(0,0,300,300,"View 2D");

  fltkWidget2D->RequestSetView( view2D );

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
  view2D->RequestSetTransformAndParent( transform, axesObject.GetPointer() );

  tracker->RequestOpen();
  tracker->RequestStartTracking();

  view2D->SetRefreshRate( 30 );
  view2D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
  view2D->SetCameraPosition( 100.0, 100.0, 100.0 );
  view2D->SetFocalPoint( 0.0, 0.0, 0.0 );
  view2D->SetCameraViewUp( 0, 0, 1.0 );

  view2D->RequestAddObject( axesRepresentation );

  view2D->RequestResetCamera();
  view2D->RequestStart();

  for(unsigned int i=0; i<500; i++)
    {
    Fl::wait(0.01);
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();       // trigger FLTK redraws
    }

  view2D->RequestStop();

  tracker->RequestStopTracking();
  tracker->RequestClose();

  delete fltkWidget2D;
  delete form;

  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
