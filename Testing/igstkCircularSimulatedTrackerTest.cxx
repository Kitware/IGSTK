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
#include "igstkView2D.h"
#include "igstkFLTKWidget.h"
#include "igstkRealTimeClock.h"
#include "igstkCircularSimulatedTracker.h"

int igstkCircularSimulatedTrackerTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer axesObject = AxesObjectType::New();

  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer axesRepresentation = RepresentationType::New();

  // Test Property
  std::cout << "Testing Property : ";
  axesRepresentation->SetColor(0.1,0.2,0.3);
  axesRepresentation->SetOpacity(0.4);

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

  delete fltkWidget2D;
  delete form;

  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
