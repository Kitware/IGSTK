/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewTest.cxx
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
#include "igstkViewProxy.h"
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkDefaultWidget.h"

namespace ViewTest {

class ViewObserver : public ::itk::Command
{
public:

  typedef  ViewObserver                  Self;
  typedef  ::itk::Command                Superclass;
  typedef  ::itk::SmartPointer<Self>     Pointer;
  itkNewMacro( Self );

protected:
  ViewObserver()
    {
    m_PulseCounter = 0;
    m_View = 0;
    }
public:

  void ResetCounter()
    {
    m_PulseCounter = 0;
    *m_End = false;
    }

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

  void SetResizeFlag( bool * resize )
    {
    m_Resize = resize;
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter == 10 )
        {
        *m_Resize = true;
        }

      if( m_PulseCounter > 50 )
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

  unsigned long         m_PulseCounter;
  ::igstk::View       * m_View;
  bool *                m_End;
  bool *                m_Resize;
};

}

#define TESTView2D
#define TESTView3D

int igstkViewTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D               View2DType;
  typedef igstk::View3D               View3DType;
  typedef ViewTest::ViewObserver      ObserverType;

  bool bEnd    = false;
  bool bResize = false;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );

  // The following line can be set to DEBUG if necessary
  logger->SetPriorityLevel( itk::Logger::CRITICAL );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput =
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from
                                       // VTK OutputWindow -> logger


  try
    {
    // Define a representation for the coordinate system
    igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();

    typedef igstk::AxesObjectRepresentation  RepresentationType;
    RepresentationType::Pointer AxesRepresentation = RepresentationType::New();
    AxesRepresentation->RequestSetAxesObject( worldReference );

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

    const double validityTimeInMilliseconds =
      igstk::TimeStamp::GetLongestPossibleTime();

    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0.5;
    translation[1] = 0;
    translation[2] = 0;
    igstk::Transform::VersorType rotation;
    rotation.Set( 0.0, 0.0, 0.0, 1.0 );
    igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

    transform.SetTranslationAndRotation(
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoid->RequestSetTransformAndParent( transform, worldReference );

    // 30 degrees in radians
    const double cylinderAngle = 30.0 * vcl_atan(1.0) / 45.0;

    const double rx = vcl_sin( cylinderAngle / 2.0 );
    const double rw = vcl_cos( cylinderAngle / 2.0 );
    translation[0] =  0.0;
    translation[1] =  0.5;  // translate the cylinder along Y
    translation[2] =  0.0;  // translate the cylinder along Z
    rotation.Set( rx, 0.0, 0.0, rw );

    transform.SetTranslationAndRotation(
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransformAndParent( transform, worldReference );

    cylinderRepresentation->SetLogger( logger );

#ifdef TESTView3D
      { // create a scope for the view3D

      // Test the 3D view
      View3DType::Pointer view3D = View3DType::New();
      view3D->RequestSaveScreenShot("igstkViewTestScreenshotIdle.invalid"); // code coverage

      // Use a surrogate Widget to initialize the view
      igstk::DefaultWidget dummyWidget2(300,300);
      dummyWidget2.RequestSetView( view3D );

      ObserverType::Pointer viewObserver2 = ObserverType::New();

      bEnd = false;
      bResize = false;

      view3D->RequestSaveScreenShot("igstkViewTestScreenshotIdle.png"); // code coverage

      viewObserver2->SetView( view3D );
      viewObserver2->SetEndFlag( &bEnd );
      viewObserver2->SetResizeFlag( &bResize );
      viewObserver2->ResetCounter();

      worldReference->SetSize(1.0,1.0,1.0);

      view3D->SetRefreshRate( 30 );
      view3D->SetRendererBackgroundColor( 0.8, 0.9, 0.8 );
      view3D->SetCameraPosition( 5.0, 2.0, 1.0 ); // Looking from a diagonal
      view3D->SetCameraFocalPoint( 0.0, 0.0, 0.0 );   // Looking at the origin
      view3D->SetCameraViewUp( 0.0, 0.0, 1.0 ); // Z axis up
      view3D->SetCameraZoomFactor( 1 );

      // Exercise GetNameOfClass() method
      std::cout << view3D->View3DType::Superclass::GetNameOfClass()
                << std::endl;

      // Exercise Print() and PrintSelf() method.
      view3D->Print( std::cout );

      transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
      view3D->RequestSetTransformAndParent( transform, worldReference );

      // Add the ellipsoid and cylinder representations to the view
      view3D->RequestAddObject( NULL );          // code coverage
      view3D->RequestAddAnnotation2D( NULL );    // code coverage
      view3D->RequestRemoveObject( NULL );       // code coverage
      view3D->RequestAddObject( AxesRepresentation );
      view3D->RequestAddObject( ellipsoidRepresentation );
      view3D->RequestAddObject( cylinderRepresentation );
      view3D->RequestStart();

      while( !bEnd )
        {
        igstk::PulseGenerator::CheckTimeouts();
        }
      view3D->RequestStop();

      // Provide code coverage for the Proxy
      dummyWidget2.TestProxy();

      // Exercise the screenshot option with a valid filename
      view3D->RequestSaveScreenShot("igstkViewTestScreenshot2.png");
      view3D->RequestSaveScreenShot("ScreenShot.invalid");

      } // end of view3D scope
#endif

#ifdef TESTView2D
    // create a scope to destroy the view2D at the end
      {
      View2DType::Pointer view2D = View2DType::New();
      view2D->SetLogger( logger );

      // Exercise GetNameOfClass() method
      std::cout << view2D->View2DType::Superclass::GetNameOfClass()
                << std::endl;

      igstk::DefaultWidget dummyWidget(300,300);
      dummyWidget.RequestSetView( view2D );

      // Create an observer in order to count number of view redraws
      ObserverType::Pointer viewObserver = ObserverType::New();

      bEnd = false;
      bResize = false;

      viewObserver->SetView( view2D );
      viewObserver->SetEndFlag( &bEnd );
      viewObserver->SetResizeFlag( &bResize );
      viewObserver->ResetCounter();


      view2D->SetRefreshRate( 30 );
      view2D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
      view2D->RequestSetOrientation( View2DType::Axial );

      transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
      view2D->RequestSetTransformAndParent( transform, worldReference );

      // Add the ellipsoid and cylinder representations to the view
      view2D->RequestAddObject( AxesRepresentation );
      view2D->RequestAddObject( ellipsoidRepresentation );
      view2D->RequestAddObject( cylinderRepresentation );

      // Remove the ellipsoid from the view
      view2D->RequestRemoveObject( ellipsoidRepresentation );
      // Add it back
      view2D->RequestAddObject( ellipsoidRepresentation );

      // Exercise and test the Print() methods
      view2D->Print( std::cout, 0 );

      std::cout << *view2D << std::endl;

      // Auto adjust the camera zoom factor
      view2D->RequestResetCamera();

      view2D->RequestStart();

      while( !bEnd )
        {
        igstk::PulseGenerator::CheckTimeouts();
        }
      view2D->RequestStop();

      // Now test resizing the window:
      viewObserver->ResetCounter();
      view2D->RequestStart();

      while(1)
        {
        igstk::PulseGenerator::CheckTimeouts();
        if( bEnd )
          {
          break;
          }

        // modify the render window
        if ( bResize )
          {
          bResize = false;
          }
        }

      // Exercise the screenshot option with a valid filename
      view2D->RequestStop();
      view2D->RequestSaveScreenShot("igstkViewTestScreenshot1.png");
      } // end of view2D scope
#endif

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
