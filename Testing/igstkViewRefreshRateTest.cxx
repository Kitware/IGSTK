/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewRefreshRateTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// 
//  The purpose of this test is to verify that a view is actually refreshed
//  at a rate close to the one that was requested by the application developer.
//
//


#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the debug 
// information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkRealTimeClock.h"

#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkFLTKWidget.h"

namespace ViewRefreshRateTest
{
  
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
    m_NumberOfPulsesToStop = 10;
    m_Form = 0;
    m_View = 0;
    }

public:

  void SetForm( Fl_Window * form )
    {
    m_Form = form;
    }

  void SetEndFlag( bool * end )
    {
    m_End = end;
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

  void SetNumberOfPulsesToStop( unsigned long number )
    {
    m_NumberOfPulsesToStop = number;
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter > m_NumberOfPulsesToStop )
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
  unsigned long       m_NumberOfPulsesToStop;
  Fl_Window *         m_Form;
  ::igstk::View *     m_View;
  bool *              m_End;
};

}

int igstkViewRefreshRateTest( int argc, char *argv [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  logger->SetPriorityLevel( itk::Logger::CRITICAL );

  LogOutputType::Pointer logOutput = LogOutputType::New();

  std::ofstream outputLogFile;
  if( argc > 1 )
    {
    outputLogFile.open( argv[1] );
    logOutput->SetStream( outputLogFile );
    logger->AddLogOutput( logOutput );
    }

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                              igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool result = true;

  try
    {
    // Define a representation for the coordinate system
    igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();

    typedef igstk::AxesObjectRepresentation  RepresentationType;
    RepresentationType::Pointer axesRepresentation = RepresentationType::New();
    axesRepresentation->RequestSetAxesObject( worldReference );


    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    ellipsoid->SetLogger( logger );
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                                igstk::EllipsoidObjectRepresentation::New();
    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);
    ellipsoidRepresentation->SetLogger( logger );

    View2DType::Pointer view2D = View2DType::New();
    View3DType::Pointer view3D = View3DType::New();

    view2D->SetLogger( logger );
    view3D->SetLogger( logger );

    typedef igstk::FLTKWidget    FLTKWidgetType;

    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(601,301,"View Refresh Rate Test");

    FLTKWidgetType * widget2D = new FLTKWidgetType(  10,10,280,280,"2D View");
    FLTKWidgetType * widget3D = new FLTKWidgetType( 310,10,280,280,"2D View");

    form->end();
    // End of the GUI creation

    widget2D->RequestSetView( view2D );
    widget3D->RequestSetView( view3D );
    
    form->show();
   
    bool bEnd = false;

    typedef ViewRefreshRateTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view2D );
    viewObserver->SetForm( form );
    viewObserver->SetEndFlag( &bEnd );

    const double validityTimeInMilliseconds = 1e5; // 100 seconds
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0;
    translation[1] = 0;
    translation[2] = 0;
    igstk::Transform::VersorType rotation;
    rotation.Set( 0.707, 0.0, 0.707, 0.0 );
    igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoid->RequestSetTransformAndParent( transform, worldReference );

    igstk::Transform identity;
    identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    view2D->RequestSetTransformAndParent( identity, worldReference );
    view3D->RequestSetTransformAndParent( identity, worldReference );

    // Add the ellipsoid to the view
    view2D->RequestAddObject( ellipsoidRepresentation );
    view3D->RequestAddObject( ellipsoidRepresentation->Copy() );
    
    // Add the axes of the reference system to the view
    view2D->RequestAddObject( axesRepresentation );
    view3D->RequestAddObject( axesRepresentation->Copy() );
    
    view2D->RequestResetCamera();
    view3D->RequestResetCamera();
 
    // Exercise the code for resizing the window
    form->resize(100, 100, 600, 300);

    const float refreshRate = 20;
    const float expectedNumberOfSeconds = 30;
    const unsigned long numberOfPulsesToStop = 
      static_cast< unsigned long >( refreshRate * expectedNumberOfSeconds );

    // Set the refresh rate for the view that we are validating
    view2D->SetRefreshRate( refreshRate );

    // Make it compete with another view that refreshes at double the rate.
    view3D->SetRefreshRate( refreshRate * 2 );

    viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
    
    view2D->RequestStart();
    view3D->RequestStart();

    // Manually trigger the first redraw in order to discard any initialization
    // time of the graphics.
    Fl::check();

    
    const double beginTime = igstk::RealTimeClock::GetTimeStamp();
    
    while( !bEnd )
      {
      Fl::wait(0.001);
      igstk::PulseGenerator::CheckTimeouts();
      }

    view2D->RequestStop();
    view3D->RequestStop();

    const double endTime   = igstk::RealTimeClock::GetTimeStamp();

    const double secondsElapsed = ( endTime - beginTime ) / 1000;

    const float actualRate = numberOfPulsesToStop / secondsElapsed;

    std::cout << "selected refresh rate   = " << refreshRate << std::endl;
    std::cout << "actual   refresh rate   = " << actualRate  << std::endl;

    //
    //   Give it a tolerance of 20%
    //
    if( fabs( actualRate - refreshRate ) / refreshRate > 0.20 )
      {
      std::cerr << "Refresh Rate did not match the expected value" << std::endl;
      std::cerr << "secondsElapsed          = " << secondsElapsed << std::endl;
      std::cerr << "expectedNumberOfSeconds = " << expectedNumberOfSeconds;
      std::cerr << std::endl;
      result = false;
      }

    // at this point the observer should have hid the form

    delete widget2D;
    delete widget3D;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    result = false;
    }

  if( !result )
    {
    return EXIT_FAILURE;
    }

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
