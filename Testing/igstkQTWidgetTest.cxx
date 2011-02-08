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
#include <QtTest/QTest>

#include <iostream>
#include "igstkView2D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkQTWidget.h"

#include "igstkLogger.h"
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

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }

  void SetView( ::igstk::View * view )
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

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
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
  ::igstk::View *     m_View;
  bool *              m_End;
};

}

int igstkQTWidgetTest( int argc, char * argv[]) 
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D  View2DType;

  bool bEnd = false;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
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

#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    ellipsoid->RequestSetTransform( transform );
#endif

    translation[1] = -0.25;  // translate the cylinder along Y
    translation[2] = -2.00;  // translate the cylinder along Z
    rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

#ifdef USE_SPATIAL_OBJECT_DEPRECATED
    cylinder->RequestSetTransform( transform );
#endif

    View2DType::Pointer view2D = View2DType::New();
    view2D->SetLogger( logger );
    
    view2D->RequestResetCamera();
    
    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );

    // Add the cylinder to the view
    view2D->RequestAddObject( cylinderRepresentation );

    // Create an QT minimal GUI
    QApplication app(argc, argv);
    QMainWindow  * qtMainWindow = new QMainWindow();
    qtMainWindow->setFixedSize(601,301);

    typedef igstk::QTWidget      QTWidgetType;

    QTWidgetType * qtWidget2D = new QTWidgetType();
    qtWidget2D->SetLogger( logger );
    qtWidget2D->RequestDisableInteractions();// code coverage
    qtWidget2D->RequestEnableInteractions(); // code coverage
    qtWidget2D->GetRenderWindowInteractor(); // code coverage
    qtWidget2D->RequestSetView( NULL );      // code coverage
    qtWidget2D->RequestSetView( view2D );
    qtWidget2D->GetRenderWindowInteractor(); // code coverage
    
    //coverage stuff
    qtWidget2D->RequestDisableInteractions();
    qtWidget2D->RequestEnableInteractions();
    std::cout << qtWidget2D << std::endl;
    QTestEventList events;
    QPoint qp(1,1); 
    events.addMousePress(Qt::LeftButton);
    events.addMouseRelease(Qt::LeftButton);
    events.addMousePress(Qt::MidButton);
    events.addMouseRelease(Qt::MidButton);
    events.addMousePress(Qt::RightButton);
    events.addMouseRelease(Qt::RightButton);
    events.addMouseMove(qp);
    events.simulate(qtWidget2D); 

    qtMainWindow->setCentralWidget( qtWidget2D );
    
    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->SetRefreshRate( 30 );
    view2D->RequestStart();

    qtMainWindow->show();

    typedef QTWidgetTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view2D );
    viewObserver->SetEndFlag( &bEnd );
    
    while(1)
      {
      QTest::qWait(10);
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }
    
    delete qtWidget2D;
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
