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
#include "igstkView2D.h"
#include "igstkEvents.h"
#include "igstkAxesObject.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkFLTKWidget.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

class FLTKWidgetTest2 : public igstk::FLTKWidget 
{
  typedef igstk::FLTKWidget Superclass;

public:
  void resize( int xPos, int yPos, int width, int height )
    {
    Superclass::resize( xPos, yPos, width, height );
    }

  FLTKWidgetTest2( int xPos, int yPos, int width, int height, const char * name)
       : Superclass( xPos, yPos, width, height,name )
    {
    } 
};

int igstkFLTKWidgetTest2( int argc, char * argv[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D  View2DType;

  if( argc < 3 )
    {
    std::cerr  << "Usage: " << argv[0] 
               << "ScreenShotFileNam\t ScreenShotFileName2"
               << std::endl;
    return EXIT_FAILURE;
    }


  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
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
    // Create the referene system
    igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();
   
    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    ellipsoid->SetLogger( logger );

    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->SetLogger( logger );

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    const double validityTimeInMilliseconds = 2e10;
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

    ellipsoid->RequestSetTransformAndParent( transform, worldReference );

    View2DType::Pointer view2D = View2DType::New();

    igstk::Transform identityTransform;
    identityTransform.SetToIdentity( 
            igstk::TimeStamp::GetLongestPossibleTime());

    view2D->RequestSetTransformAndParent( identityTransform, worldReference );
   

    // Create an FLTK minimal GUI
    typedef FLTKWidgetTest2    WidgetType;

    Fl_Window * form = new Fl_Window(512,512,"View Test");
    
    // instantiate FLTK widget 
    WidgetType * widget2D = new WidgetType( 10,10,280,280,"2D View");
    widget2D->RequestSetView( view2D );
    
    form->end();
    form->show();
    // End of the GUI creation

    view2D->RequestAddObject( ellipsoidRepresentation );
//    view2D->RequestResetCamera();

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->SetRefreshRate( 10 );
    view2D->RequestStart();

    std::string ScreenShotFileName = argv[1];

    for(unsigned int i=0; i<100; i++)
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      }

    view2D->RequestSaveScreenShot( ScreenShotFileName );

    //Resize the window and take a screenshot
    std::string ScreenShotFileName2 = argv[2];

    // resize the widget
    widget2D->resize(10,10,490,490);
    for(unsigned int i=0; i<100; i++)
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      }
    view2D->RequestSaveScreenShot( ScreenShotFileName2 );

    delete widget2D;
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
