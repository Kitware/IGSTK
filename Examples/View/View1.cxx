/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    View1.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug 
//information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

// BeginLatex
// This example illustrates how to use the view class.
// EndLatex

// BeginCodeSnippet
//
int main( int, char * [] )
{
  // EndCodeSnippet
  igstk::RealTimeClock::Initialize();

  // BeginLatex
  // Define a 3D view
  // EndLatex
  // BeginCodeSnippet
  typedef igstk::View3D  View3DType;
  // EndCodeSnippet
  // 
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  //BeginCodeSnippet
  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                  igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  
  //EndCodeSnippet
  try
    {

    //BeginCodeSnippet
    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    //EndCodeSnippet
   
    //BeginCodeSnippet 
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation = 
                                  igstk::EllipsoidObjectRepresentation::New();
    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);
    //EndCodeSnippet
    // BeginCodeSnippet
    const double validityTimeInMilliseconds = 1e300; // 100 seconds
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
    //EndCodeSnippet
    //BeginCodeSnippet
    ellipsoid->RequestSetTransform( transform );
    //EndCodeSnippet
    //BeginCodeSnippet
    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(601,301,"View Test");
    
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");
    form->end();
    // End of the GUI creation

    form->show();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
    // Add the ellipsoid to the view
    view3D->RequestAddObject( ellipsoidRepresentation );
    
    // Do manual redraws
    for(unsigned int i=0; i<10; i++)
      {
      view3D->Update();  // schedule redraw of the view
      Fl::check();       // trigger FLTK redraws
      }

    delete view3D;
    delete form;
    //EndCodeSnippet
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


