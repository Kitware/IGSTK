/*=========================================================================
  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQTWidgetTest2.cxx
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

// QT header files
#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QtTest/QTest>

#include <iostream>
#include "igstkViewNew2D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkQTWidget.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkQTWidgetTest2( int argc, char * argv[]) 
{
  igstk::RealTimeClock::Initialize();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
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
    // Create an QT minimal GUI
    QApplication app(argc, argv);
    QMainWindow  * qtMainWindow = new QMainWindow();
    qtMainWindow->setFixedSize(601,301);

    typedef igstk::QTWidget      QTWidgetType;
    typedef igstk::ViewNew2D  ViewNew2DType;

    //Instantiate widget and view component
    QTWidgetType * qtWidget2D = new QTWidgetType();
    ViewNew2DType::Pointer view2D = ViewNew2DType::New();
    view2D->SetLogger( logger );
    view2D->RequestResetCamera();

    qtWidget2D->SetLogger( logger );
    qtWidget2D->RequestSetView( view2D );

    //Instantiate a second widget and a view component
    QTWidgetType * qtWidget2D2 = new QTWidgetType();
    ViewNew2DType::Pointer view2D2 = ViewNew2DType::New();
    view2D2->SetLogger( logger );
    view2D2->RequestResetCamera();

    qtWidget2D2->SetLogger( logger );
    qtWidget2D2->RequestSetView( view2D2 );
    qtWidget2D2->setGeometry(QRect(260,10,255,301));

    QGridLayout *widgetLayout = new QGridLayout();
    QWidget     *widgetGroup  = new QWidget();

    widgetLayout->addWidget(qtWidget2D, 0, 0);
    widgetLayout->addWidget(qtWidget2D2, 0, 1);
    widgetGroup->setLayout(widgetLayout);
    qtMainWindow->setCentralWidget( widgetGroup );

    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(1.0,2.0,3.0);
   
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

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
  
#ifndef USE_SPATIAL_OBJECT_DEPRECATED
    ellipsoid->RequestSetTransform( transform );
#endif

    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );

    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation2 =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation2->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation2->SetColor(0.0,0.0,1.0);
    ellipsoidRepresentation->SetOpacity(1.0);
    // Add the ellipsoid to the view
    view2D2->RequestAddObject( ellipsoidRepresentation2 );

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->SetRefreshRate( 30 );
    view2D->RequestStart();

    view2D2->SetRefreshRate( 30 );
    view2D2->RequestStart();

    view2D->RequestResetCamera();
    view2D2->RequestResetCamera();

    qtMainWindow->show();

    for( unsigned int i=0; i <= 50 ; i++ )
      {
      QTest::qWait(10);      
      igstk::PulseGenerator::CheckTimeouts();
      }
    
    delete qtWidget2D;
    delete qtWidget2D2;
    delete qtMainWindow;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
