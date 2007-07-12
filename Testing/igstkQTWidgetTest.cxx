/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQTWidgetTest.cxx
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

#include <iostream>
#include "igstkViewNew2D.h"
#include "igstkViewNew3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkQTWidget.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace QTWidgetTest{
  
class ViewObserver : public ::itk::Command 
{
public:
  
  typedef  ViewObserver               Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  ViewObserver() 
    {
    m_PulseCounter = 0;
    m_View = 0;
    }
public:

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }

  void SetView( ::igstk::ViewNew * view )
    {
    m_View = view;
    if( m_View )
      {
      m_View->AddObserver( ::igstk::RefreshEvent(), this );
      }
    }

  void SetEndFlag( bool * end )
    {
    m_End = end;
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter > 20 )
        {
        if( m_View )
          {
          m_View->RequestStop();
          } 
        else
          {
          std::cerr << "View pointer is NULL " << std::endl;
          }
        *m_End = true;
        return;
        }
      }
    }
private:
  
  unsigned long       m_PulseCounter;
  ::igstk::ViewNew *  m_View;
  bool *              m_End;

};

}

int igstkQTWidgetTest( int argc, char ** argv)
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::ViewNew2D  ViewNew2DType;
  typedef igstk::ViewNew3D  ViewNew3DType;

  bool bEnd = false;

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
    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    // Create the cylinder 
    igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
    cylinder->SetRadius(0.1);
    cylinder->SetHeight(0.5);

    // Create the cylinder representation
    igstk::CylinderObjectRepresentation::Pointer cylinderRepresentation =
                              igstk::CylinderObjectRepresentation::New();

    cylinderRepresentation->RequestSetCylinderObject( cylinder );
    cylinderRepresentation->SetColor(1.0,0.0,0.0);
    cylinderRepresentation->SetOpacity(1.0);

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

    ellipsoid->RequestSetTransform( transform );


    translation[1] = -0.25;  // translate the cylinder along Y
    translation[2] = -2.00;  // translate the cylinder along Z
    rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransform( transform );

    ViewNew2DType::Pointer view2D = ViewNew2DType::New();
    view2D->SetLogger( logger );
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );

    // Add the cylinder to the view
    view2D->RequestAddObject( cylinderRepresentation );

    ViewNew3DType::Pointer view3D = ViewNew3DType::New();
    view3D->SetLogger( logger );

    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
    // Add the ellipsoid to the view
    view3D->RequestAddObject( ellipsoidRepresentation );

    // Add the cylinder to the view
    view3D->RequestAddObject( cylinderRepresentation );

    // Create an QT minimal GUI
    QApplication app(argc, argv);
    QMainWindow  * qtMainWindow = new QMainWindow();
    qtMainWindow->setFixedSize(601,301);

    typedef igstk::QTWidget      QTWidgetType;

    QTWidgetType * qtWidget2D = new QTWidgetType();
    qtWidget2D->SetView( view2D );
    qtWidget2D->SetLogger( logger );
    qtMainWindow->setCentralWidget( qtWidget2D );
    
    QTWidgetType * qtWidget3D = new QTWidgetType();
    qtWidget3D->SetView( view3D );
    qtWidget3D->SetLogger( logger );

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->Update();    
    view2D->RequestSetRefreshRate( 30 );
    view2D->RequestStart();

    view3D->Update();    
    view3D->RequestSetRefreshRate( 30 );
    view3D->RequestStart();

    typedef QTWidgetTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view3D );
    viewObserver->SetEndFlag( &bEnd );

    qtMainWindow->show();

    while(1)
      {
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }


    delete qtMainWindow;
    delete qtWidget2D;
    delete qtWidget3D;

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
