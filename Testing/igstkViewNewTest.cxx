/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewNewTest.cxx
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
#include "igstkViewNew2D.h"
#include "igstkViewNew3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkWorldCoordinateReferenceSystemObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace ViewNewTest {
  
class ViewNewObserver : public ::itk::Command 
{
public:
  
  typedef  ViewNewObserver               Self;
  typedef  ::itk::Command                Superclass;
  typedef  ::itk::SmartPointer<Self>     Pointer;
  itkNewMacro( Self );

protected:
  ViewNewObserver() 
    {
    m_PulseCounter = 0;
    m_ViewNew = 0;
    }
public:

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }

  void SetViewNew( ::igstk::ViewNew * view )
    {
    m_ViewNew = view;
    if( m_ViewNew )
      {
      m_ViewNew->AddObserver( ::igstk::RefreshEvent(), this );
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

  void Execute(itk::Object *caller, const itk::EventObject & event)
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
        if( m_ViewNew )
          {
          m_ViewNew->RequestStop();
          } 
        else
          {
          std::cerr << "ViewNew pointer is NULL " << std::endl;
          }
        *m_End = true;
        return;
        }
      }
    }
private:
  
  unsigned long         m_PulseCounter;
  ::igstk::ViewNew    * m_ViewNew;
  bool *                m_End;
  bool *                m_Resize;
};


// This is an ad-hoc Widget that emulate the actions
// of an actual GUI Widget.
class DummyWidget 
{
public:
  typedef ::igstk::ViewProxy< DummyWidget > ProxyType;
  typedef ::igstk::ViewNew                  ViewType;

  friend class ::igstk::ViewProxy< DummyWidget >;

  void SetView( ViewType * view )
    {
    m_View = view;
    this->m_ProxyView.Connect( m_View );
    m_ProxyView.SetRenderWindowSize( m_View, 300, 300 );
    }

private:
  ProxyType           m_ProxyView;
  ViewType::Pointer   m_View; 

  // Fake methods needed to satisfy the API exposed to the ViewProxy
  void SetRenderer( vtkRenderer * ) {};
  void SetRenderWindowInteractor( vtkRenderWindowInteractor * ) {};
  void SetReporter ( ::itk::Object::Pointer ) {};
};

}

int igstkViewNewTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::ViewNew2D  ViewNew2DType;
  typedef igstk::ViewNew3D  ViewNew3DType;

  bool bEnd    = false;
  bool bResize = false;

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
    // Define the World coordinate system
    typedef igstk::WorldCoordinateReferenceSystemObject  
      WorldReferenceSystemType;

    WorldReferenceSystemType::Pointer worldReference =
      WorldReferenceSystemType::New();

    // make the Z axis very small to avoid disturbing the 2D view
    worldReference->SetSize(10,10,0.001); 

    // Define a representation for the coordinate system
    typedef igstk::AxesObjectRepresentation  RepresentationType;
    RepresentationType::Pointer AxesRepresentation = RepresentationType::New();
    AxesRepresentation->RequestSetAxesObject( worldReference );

    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    ellipsoid->RequestAttachToSpatialObjectParent( worldReference );

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
    cylinder->RequestAttachToSpatialObjectParent( worldReference );

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

    ellipsoid->RequestSetTransformToSpatialObjectParent( transform );


    translation[0] =  0.0;
    translation[1] =  0.5;  // translate the cylinder along Y
    translation[2] =  0.0;  // translate the cylinder along Z
    rotation.Set( 0.0, 0.0, 0.0, 1.0 );

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransformToSpatialObjectParent( transform );

    cylinderRepresentation->SetLogger( logger );
  
    ViewNew2DType::Pointer view2D = ViewNew2DType::New();
    ViewNew3DType::Pointer view3D = ViewNew3DType::New();

    view3D->SetRefreshRate( 30 );
    view3D->SetRendererBackgroundColor( 0.8, 0.9, 0.8 );
    view3D->SetCameraPosition( 0.0, 0.0, 250.0 ); // Looking from Z positive
    view3D->SetFocalPoint( 0.0, 0.0, 0.0 );   // Looking at the origin
    view3D->SetCameraViewUp( 0.0, 1.0, 0.0 ); // Y axis up

    view2D->SetLogger( logger );

    // Exercise GetNameOfClass() method
    std::cout << view2D->ViewNew2DType::Superclass::GetNameOfClass() 
              << std::endl;

    ViewNewTest::DummyWidget dummyWidget;

    dummyWidget.SetView( view2D );

    // Exercise GetNameOfClass() method
    std::cout << view3D->ViewNew3DType::Superclass::GetNameOfClass() 
              << std::endl;

    view2D->SetRefreshRate( 30 );
    view2D->SetRendererBackgroundColor( 0.8, 0.8, 0.9 );
    view2D->RequestSetOrientation( ViewNew2DType::Axial );
   
    view2D->RequestStart();

    // Add the ellipsoid and cylinder representations to the view
    view2D->RequestAddObject( AxesRepresentation );
    view2D->RequestAddObject( ellipsoidRepresentation );
    view2D->RequestAddObject( cylinderRepresentation );


    // Remove the ellipsoid from the view
    view2D->RequestRemoveObject( ellipsoidRepresentation );
    // Add it back
    view2D->RequestAddObject( ellipsoidRepresentation );
    
    // Do automatic redraws using the internal PulseGenerator
    typedef ViewNewTest::ViewNewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetViewNew( view2D );
    viewObserver->SetEndFlag( &bEnd );
    viewObserver->SetResizeFlag( &bResize );

    // Exercise and test the Print() methods
    view2D->Print( std::cout, 0 );

    std::cout << *view2D << std::endl;

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
    view2D->RequestSaveScreenShot("igstkViewNewTestScreenshot1.png");


    // Do manual redraws of the 3D view
    ViewNewTest::DummyWidget dummyWidget2;
    dummyWidget2.SetView( view3D );

    ObserverType::Pointer viewObserver2 = ObserverType::New();
    
    bEnd = false;
    bResize = false;

    viewObserver2->SetViewNew( view3D );
    viewObserver2->SetEndFlag( &bEnd );
    viewObserver2->SetResizeFlag( &bResize );

    view3D->RequestStart();

    // Add the ellipsoid and cylinder representations to the view
    view3D->RequestAddObject( AxesRepresentation );
    view3D->RequestAddObject( ellipsoidRepresentation );
    view3D->RequestAddObject( cylinderRepresentation );
 
    while( !bEnd )
      {
      igstk::PulseGenerator::CheckTimeouts();
      }
    view3D->RequestStop();

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
