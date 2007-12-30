/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectRepresentationVisibilityTest.cxx
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
// Warning about: identifier was truncated to '255' characters in the 
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkConfigure.h"
#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkFLTKWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkAxesObjectRepresentation.h"

#include "igstkCircularSimulatedTracker.h"
#include "igstkSimulatedTrackerTool.h"

#define OBJECTTRACKED

namespace igstk
{

namespace VisibilityObjectTest
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
    m_NumberOfPulsesToStop = 50;
    m_Form = 0;
    m_View = 0;
    }
public:

  void SetForm( Fl_Window * form )
    {
    m_Form = form;
    m_PulseCounter = 0;
    }

  void SetEndFlag( bool * end )
    {
    m_End = end;
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
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
    m_PulseCounter = 0;
    }

  void SetNumberOfPulsesToStop( unsigned long number )
    {
    m_NumberOfPulsesToStop = number;
    m_PulseCounter = 0;
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
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
      else
        {
        *m_End = false;
        }
      }
    }

private:

  unsigned long              m_PulseCounter;
  unsigned long              m_NumberOfPulsesToStop;
  Fl_Window *                m_Form;
  ::igstk::View::Pointer     m_View;
  bool *                     m_End;
};

} // end of VisibilityObjectTest namespace

} // end namespace igstk


int igstkSpatialObjectRepresentationVisibilityTest( int argc, char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
      std::cerr << "Usage: " << argv[0] 
              << "ScreenShot filename before "
              << "ScreenShotfilename  after  " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  std::ofstream logFile("visibilityLog.txt");
  logOutput->SetStream( logFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                               igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow 

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();
  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer axesRepresentation = RepresentationType::New();
  axesRepresentation->RequestSetAxesObject( worldReference );
  worldReference->SetSize( 0.5, 0.5, 0.5 );

  typedef ::igstk::CircularSimulatedTracker    TrackerType;
  typedef ::igstk::SimulatedTrackerTool        TrackerToolType;

  TrackerType::Pointer       tracker     = TrackerType::New();
  TrackerToolType::Pointer   trackerTool = TrackerToolType::New();


  typedef igstk::EllipsoidObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation1 = ObjectRepresentationType::New();
  ellipsoidRepresentation1->SetLogger( logger );

  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation2 = ObjectRepresentationType::New();

  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation3 = ObjectRepresentationType::New();

  typedef igstk::EllipsoidObject  ObjectType;
  ObjectType::Pointer ellipsoidObject1 = ObjectType::New();
  ellipsoidObject1->SetLogger( logger );
 
  ObjectType::Pointer ellipsoidObject2 = ObjectType::New();
  ellipsoidObject2->SetLogger( logger );
 
  ObjectType::Pointer ellipsoidObject3 = ObjectType::New();
  ellipsoidObject3->SetLogger( logger );
 
  igstk::Transform identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject1->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );
  ellipsoidObject2->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );

#ifdef OBJECTFIXED
  ellipsoidObject3->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );
#endif


#ifdef OBJECTTRACKED
  ellipsoidObject3->RequestSetTransformAndParent( identityTransform, trackerTool.GetPointer() );
  igstk::Transform calibrationTransform;
  calibrationTransform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  trackerTool->SetCalibrationTransform( calibrationTransform );
#endif

  trackerTool->RequestSetName("tool1");
  trackerTool->RequestConfigure();
  tracker->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );

   
  ellipsoidRepresentation1->RequestSetEllipsoidObject(ellipsoidObject1);
  ellipsoidRepresentation1->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidRepresentation1->SetOpacity( 1.0 );
 
  ellipsoidRepresentation2->RequestSetEllipsoidObject(ellipsoidObject2);
  ellipsoidRepresentation2->SetColor( 1.0, 0.0, 0.0 );
  ellipsoidRepresentation2->SetOpacity( 1.0 );
  
  ellipsoidRepresentation3->RequestSetEllipsoidObject(ellipsoidObject3);
  ellipsoidRepresentation3->SetColor( 0.0, 1.0, 0.0 );
  ellipsoidRepresentation3->SetOpacity( 1.0 );
  
 
  typedef igstk::View3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( logger );
 
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;

  Fl_Window * form = new Fl_Window(512,512,"Visibility Test");
  FLTKWidgetType * fltkWidget = new FLTKWidgetType(6,6,500,500,"View 2D");
  
  form->end();
  // End of the GUI creation

  form->show();
  
  igstk::PulseGenerator::Sleep(500);

  fltkWidget->RequestSetView( view3D );

  // this will indirectly call CreateActors() 
  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( ellipsoidRepresentation1  );
  view3D->RequestAddObject( ellipsoidRepresentation2 );
  view3D->RequestAddObject( ellipsoidRepresentation3 );

  view3D->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );

  double validityTimeInMilliseconds = 1000; // 1 second
  igstk::Transform transform1;
  igstk::Transform::VectorType translation1;
  translation1[0] =  0.5;
  translation1[1] =- 0.5;
  translation1[2] =  0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns



  
  bool bEnd = false;
  const double refreshRate = 20.0;

  typedef ::igstk::VisibilityObjectTest::ViewObserver  ViewObserverType;
  ViewObserverType::Pointer viewObserver = ViewObserverType::New();

  view3D->SetRefreshRate( refreshRate );

  viewObserver->SetView( view3D );
  viewObserver->SetForm( form );
  viewObserver->SetEndFlag( &bEnd );

  const unsigned long numberOfSeconds = 3;
  const unsigned long numberOfPulsesToStop = 
    static_cast< unsigned long >( refreshRate * numberOfSeconds );

  transform1.SetTranslationAndRotation( 
      translation1, rotation, errorValue, validityTimeInMilliseconds );

  ellipsoidObject1->RequestSetTransformAndParent( transform1, worldReference.GetPointer() );

  // This is the transform for the permanent object.
  igstk::Transform transform2;
  igstk::Transform::VectorType translation2;
  translation2[0] = 0.0;
  translation2[1] = 1.0;
  translation2[2] = 0.0;

  transform2.SetTranslationAndRotation( 
      translation2, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject2->RequestSetTransformAndParent( transform2, worldReference.GetPointer() );

  
#ifdef OBJECTFIXED
  // This is the transform for the permanent object.
  igstk::Transform transform3;
  igstk::Transform::VectorType translation3;
  translation3[0] = -0.5;
  translation3[1] = -0.5;
  translation3[2] =  0.0;

  transform3.SetTranslationAndRotation( 
      translation3, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject3->RequestSetTransformAndParent( transform3, worldReference.GetPointer() );
#endif


  view3D->RequestResetCamera();
  view3D->RequestStart();


  // Stabilize the transient period where the camera
  // must be reset.
  for(unsigned int k=0; k<100; k++)
    {
    igstk::PulseGenerator::Sleep(20);
    igstk::PulseGenerator::CheckTimeouts();
    }

 
  std::string screenShotFileName1 = argv[1]; 
  std::string screenShotFileName2 = argv[2];

  bool screenShotTaken = false;

  // restart the count of the observer
  viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
  bEnd = false;
  view3D->RequestStart();

  tracker->RequestOpen();
  trackerTool->RequestAttachToTracker( tracker );

  tracker->RequestStartTracking();

  while(1)
    {
    Fl::wait(0.001);
    if ( ! screenShotTaken )  
      {
      view3D->RequestSaveScreenShot( screenShotFileName1 );
      screenShotTaken = true;
      }
    igstk::PulseGenerator::CheckTimeouts();
    if( bEnd )
      {
      break;
      }
    }
  
  tracker->RequestStopTracking();
  tracker->RequestClose();

  view3D->RequestSaveScreenShot( screenShotFileName2 );

  delete fltkWidget;
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
