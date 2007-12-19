/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    View1.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug 
//information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
// 
// This example illustrates how to use the \doxygen{View3D} class to display
// spatial objects.
//
// EndLatex

#include <iostream>
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkFLTKWidget.h"

int main( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  // BeginLatex
  // 
  // First, a 3D view and other useful data types are defined: 
  //
  // EndLatex

  // BeginCodeSnippet
  typedef igstk::FLTKWidget           WindowWidgetType;
  typedef igstk::View3D               View3DType;
  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  // EndCodeSnippet

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // BeginLatex
  //
  // For debugging purposes, the VTK window output can be redirected to a
  // logger, using \doxygen{VTKLoggerOutput}, as follows:
  //
  // EndLatex

  // BeginCodeSnippet
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                  igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  
  // EndCodeSnippet

  try
    {
    // BeginLatex
    // 
    // In this example, we would like to display an ellipsoid object.  To carry
    // this out, an ellipsoid spatial object is first instantiated:
    // 
    // EndLatex

    // BeginCodeSnippet
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    // EndCodeSnippet
  
    // BeginLatex
    // 
    // Next, a representation object is created using
    // the \doxygen{EllipsoidObjectRepresentation} class. The  representation
    // class provides the mechanism to generate a graphical description of the
    // spatial object for visualization in a VTK scene, as follows: 
    //
    //EndLatex 

    // BeginCodeSnippet 
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation = 
                                  igstk::EllipsoidObjectRepresentation::New();
    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);
    // EndCodeSnippet


    // BeginLatex
    // 
    // Next, the FLTK main window and an OpenGL FLTK Widget object are
    // instantiated:
    //
    // EndLatex

    // BeginCodeSnippet
    Fl_Window * form = new Fl_Window(301,301,"View Test");
    WindowWidgetType * fltkWidget = new WindowWidgetType(10,10,280,280,"3D View");

    View3DType::Pointer view3D = View3DType::New();


    fltkWidget->RequestSetView( view3D );

    form->end();
    form->show();
    // EndCodeSnippet
    
    // BeginCodeSnippet

    // EndCodeSnippet

   
    // BeginLatex
    // 
    // A geometrical transformation can then be applied to define the spatial
    // relationship between the view and the ellipsoid object as follows:
    // 
    // EndLatex

    // BeginCodeSnippet
    const double validityTimeInMilliseconds = 
      igstk::TimeStamp::GetLongestPossibleTime();

    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0;
    translation[1] = 0;
    translation[2] = 0;
    igstk::Transform::VersorType rotation;
    rotation.Set( 0.0, 0.0, 0.0, 1.0 );
    igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoid->RequestSetTransformAndParent( transform, view3D.GetPointer() );
    // EndCodeSnippet


    // BeginLatex
    // 
    // The ellipsoid is added to the scene using the \code{RequestAddObject}
    // method and then the scope of the camera is reset in order to make sure
    // that the object is visible in the window.
    //
    // EndLatex

    // BeginCodeSnippet
    view3D->RequestAddObject( ellipsoidRepresentation );

    view3D->SetCameraPosition( 0, 0, -1 );
    view3D->SetCameraViewUp( 0, -1, 0 );
    view3D->SetFocalPoint( 0, 0, 0 );

    view3D->RequestResetCamera();
    // EndCodeSnippet


    // BeginLatex
    //
    // The View components are designed for refreshing their representation at
    // regular intervals. The application developer must set the desired
    // refresh rate in Hertz, which should trigger the 
    // internal generation of pulses that makes it possible for the View class
    // to refresh itself as follows:
    // 
    // EndLatex

    // BeginCodeSnippet
    view3D->SetRefreshRate( 30 );
    view3D->RequestStart();
    // EndCodeSnippet


    // BeginLatex
    // 
    // At this point it is now possible to start the event loop that will drive
    // the user interaction of the application. Inside the loop, it is of
    // fundamental importance to invoke the call to the
    // \doxygen{PulseGenerator} method \code{CheckTimeouts()}. This methods
    // ensures that the pulse generator timers in all the autonomous IGSTK
    // classes are checked to see if they should trigger timer events. The same
    // loop should have some form of ``wait'' or ``sleep'' instruction in order
    // to prevent the loop from taking over the CPU time.
    //
    // EndLatex

    // BeginCodeSnippet
    for(unsigned int i=0; i<1000; i++)
      {
      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
      Fl::check();       // trigger FLTK redraws
      }
    // EndCodeSnippet


    // BeginLatex
    // 
    // Once the event loop finishes, the method \code{RequestStop()} should be 
    // called to stop the refresh process of the View class as follows:
    //
    // EndLatex


    // BeginCodeSnippet
    view3D->RequestStop();
    // EndCodeSnippet

    delete fltkWidget;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
