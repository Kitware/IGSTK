/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidgetTest2.cxx
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
//  Warning about: identifier was truncated to '255' characters in the 
//  debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkViewNew2D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkWorldCoordinateReferenceSystemObject.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkFLTKWidget.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int main( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::ViewNew2D  ViewNew2DType;

  bool bEnd = false;

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();

  std::ofstream loggerFileOutput;
  
  loggerFileOutput.open("VisibilityLog.txt");

  logOutput->SetStream( loggerFileOutput );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger

  try
    {
    // create the World coordinate reference system
    igstk::WorldCoordinateReferenceSystemObject::Pointer worldReference =
      igstk::WorldCoordinateReferenceSystemObject::New();

    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    ellipsoid->SetLogger( logger );

    ellipsoid->RequestAttachToSpatialObjectParent( worldReference );

    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->SetLogger( logger );

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    const double validityTimeInMilliseconds = 2000;
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

//    ellipsoid->RequestSetTransform( transform ); // DEPRECATED
    ellipsoid->RequestSetTransformToSpatialObjectParent( transform );

    ViewNew2DType::Pointer view2D = ViewNew2DType::New();
    
    view2D->RequestResetCamera();
    view2D->RequestAddObject( ellipsoidRepresentation );


    // Create an FLTK minimal GUI
    typedef igstk::FLTKWidget      FLTKWidgetType;

    // End of the GUI creation

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->RequestSetRefreshRate( 10 );

    Fl_Window * form = new Fl_Window(301,301,"View Test");
    
    // instantiate FLTK widget 
    FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType( 10,10,280,280,"2D View");
    fltkWidget2D->RequestSetView( view2D );
    
    view2D->RequestInitializeRenderWindowInteractor();    

    form->end();
    form->show();

    view2D->RequestStart();

    for(unsigned int i=0; i<1000; i++)
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      }

    delete fltkWidget2D;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  loggerFileOutput.close();

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
