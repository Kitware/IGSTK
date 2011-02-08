/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidgetTest.cxx
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
#include "igstkView3D.h"
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

namespace FLTKWidgetTest{

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

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
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

      if( m_PulseCounter > 30 )
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
  ::igstk::View     * m_View;
  bool *              m_End;

};

}

int igstkFLTKWidgetTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool bEnd = false;

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::CRITICAL );

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

    ellipsoid->RequestSetTransformAndParent( transform, worldReference );

    translation[1] = -0.25;  // translate the cylinder along Y
    translation[2] = -2.00;  // translate the cylinder along Z
    rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

    transform.SetTranslationAndRotation(
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransformAndParent( transform, worldReference );

    View2DType::Pointer view2D = View2DType::New();
    view2D->SetLogger( logger );

    View3DType::Pointer view3D = View3DType::New();
    view3D->SetLogger( logger );

    igstk::Transform identityTransform;
    identityTransform.SetToIdentity(
      igstk::TimeStamp::GetLongestPossibleTime() );

    view2D->RequestSetTransformAndParent( identityTransform, worldReference );
    view3D->RequestSetTransformAndParent( identityTransform, worldReference );

    // Add the cylinder and ellipsoid to the views
    view2D->RequestAddObject( ellipsoidRepresentation );
    view3D->RequestAddObject( cylinderRepresentation );

    // Set the refresh rate of the views.
    view2D->SetRefreshRate( 10 );
    view3D->SetRefreshRate( 10 );

    typedef igstk::FLTKWidget      WidgetType;
    Fl_Window * form = new Fl_Window(601,301,"View Test");

    // instantiate FLTK widgets
    WidgetType * widget2D = new WidgetType( 10,10,280,280,"2D View");
    widget2D->RequestEnableInteractions(); //code coverage
    widget2D->RequestDisableInteractions();//code coverage
    widget2D->GetRenderWindowInteractor(); //code coverage
    widget2D->RequestSetView( NULL );      //code coverage
    widget2D->RequestSetView( view2D );
    widget2D->GetRenderWindowInteractor(); //code coverage
    widget2D->RequestDisableInteractions();//code coverage
    widget2D->RequestEnableInteractions(); //code coverage
    

    WidgetType * widget3D = new WidgetType( 310,10,280,280,"3D View");
    widget3D->RequestEnableInteractions(); //code coverage
    widget3D->RequestDisableInteractions();//code coverage
    widget3D->GetRenderWindowInteractor(); //code coverage
    widget3D->RequestSetView( NULL );      //code coverage
    widget3D->RequestSetView( view3D );
    widget3D->GetRenderWindowInteractor(); //code coverage
    widget3D->RequestDisableInteractions();//code coverage
    widget3D->RequestEnableInteractions(); //code coverage

    form->end();
    form->show();

    widget2D->SetLogger( logger );
    widget3D->SetLogger( logger );

    view3D->RequestResetCamera();
    view2D->RequestResetCamera();

    typedef FLTKWidgetTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();

    viewObserver->SetView( view3D );
    viewObserver->SetEndFlag( &bEnd );

    view2D->RequestStart();
    view3D->RequestStart();

    while( !bEnd )
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      }

    //Test a widget without a view connected to it
    WidgetType * widget3DWithoutViewConnected =
                      new WidgetType( 310,10,280,280,"3D View");
    widget3DWithoutViewConnected->SetLogger( logger );

    delete widget2D;
    delete widget3D;
    delete widget3DWithoutViewConnected;
    delete form;
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
