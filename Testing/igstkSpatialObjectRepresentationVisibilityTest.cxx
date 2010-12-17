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
#include "igstkDefaultWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkAxesObjectRepresentation.h"

#include "igstkCircularSimulatedTracker.h"
#include "igstkSimulatedTrackerTool.h"


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
    m_View = 0;
    m_End = NULL;
    }
public:

  void Reset()
    {
    if( m_End )
      {
      *m_End = false;
      }
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
    m_PulseCounter = 0;
    }

  void SetNumberOfPulsesToStop( unsigned long number )
    {
    m_NumberOfPulsesToStop = number;
    m_PulseCounter = 0;
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter > m_NumberOfPulsesToStop )
        {
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
  ::igstk::View::Pointer     m_View;
  bool *                     m_End;
};

class RepresentationObserver : public ::itk::Command 
{
public:

  typedef  RepresentationObserver     Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:

  RepresentationObserver() 
    {
    this->m_TransformExpired = false;
    }
public:

  void Reset()
    {
    this->m_TransformExpired = false;
    }

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }

  void SetObjectRepresentation( ::igstk::Object * representation )
    {
    if( representation )
      {
      representation->AddObserver( ::igstk::TransformExpiredErrorEvent(), this );
      }
    this->Reset();
    }

  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    if( ::igstk::TransformExpiredErrorEvent().CheckEvent( &event ) )
      {
      this->m_TransformExpired = true;
      }
    }

  bool GetTransformExpired() const
    {
    return this->m_TransformExpired;
    }

private:
  bool           m_TransformExpired;
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
 
 
  ellipsoidRepresentation1->RequestSetEllipsoidObject(ellipsoidObject1);
  ellipsoidRepresentation1->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidRepresentation1->SetOpacity( 1.0 );
 
  ellipsoidRepresentation2->RequestSetEllipsoidObject(ellipsoidObject2);
  ellipsoidRepresentation2->SetColor( 1.0, 0.0, 0.0 );
  ellipsoidRepresentation2->SetOpacity( 1.0 );
  
  ellipsoidRepresentation3->RequestSetEllipsoidObject(ellipsoidObject3);
  ellipsoidRepresentation3->SetColor( 0.0, 1.0, 0.0 );
  ellipsoidRepresentation3->SetOpacity( 1.0 );
  
 
  typedef ::igstk::VisibilityObjectTest::RepresentationObserver  RepresentationObserverType;
  RepresentationObserverType::Pointer representationObserver1 = RepresentationObserverType::New();
  RepresentationObserverType::Pointer representationObserver2 = RepresentationObserverType::New();
  RepresentationObserverType::Pointer representationObserver3 = RepresentationObserverType::New();

  representationObserver1->SetObjectRepresentation( ellipsoidRepresentation1 );
  representationObserver2->SetObjectRepresentation( ellipsoidRepresentation2 );
  representationObserver3->SetObjectRepresentation( ellipsoidRepresentation3 );

  typedef igstk::View3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( logger );
 
  // Create an minimal GUI
  typedef igstk::DefaultWidget      WidgetType;

  WidgetType * widget = new WidgetType( 512, 512 );
  
  // At this point the View should be displayed
  widget->RequestSetView( view3D );

  // this will indirectly call CreateActors() 
  view3D->RequestAddObject( axesRepresentation );
  view3D->RequestAddObject( ellipsoidRepresentation1  );
  view3D->RequestAddObject( ellipsoidRepresentation2 );
  view3D->RequestAddObject( ellipsoidRepresentation3 );

  igstk::Transform identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  view3D->RequestSetTransformAndParent( identityTransform, worldReference );

  double validityTimeInMilliseconds = 1000; // 1 second
  igstk::Transform transform1;
  igstk::Transform::VectorType translation1;
  translation1[0] =  1.0;
  translation1[1] =- 1.0;
  translation1[2] =  0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  
  transform1.SetTranslationAndRotation( 
      translation1, rotation, errorValue, validityTimeInMilliseconds );

  ellipsoidObject1->RequestSetTransformAndParent( transform1, worldReference );

  // This is the transform for the permanent object.
  igstk::Transform transform2;
  igstk::Transform::VectorType translation2;
  translation2[0] = 0.0;
  translation2[1] = 2.0;
  translation2[2] = 0.0;

  transform2.SetTranslationAndRotation( 
      translation2, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject2->RequestSetTransformAndParent( transform2, worldReference );

  
  // This is the transform for the permanent object.
  igstk::Transform transform3;
  igstk::Transform::VectorType translation3;
  translation3[0] = -1.0;
  translation3[1] = -1.0;
  translation3[2] =  0.0;

  transform3.SetTranslationAndRotation( 
      translation3, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject3->RequestSetTransformAndParent( transform3, worldReference );


  view3D->RequestResetCamera();

  bool bEnd = false;
  const double refreshRate = 20.0;

  typedef ::igstk::VisibilityObjectTest::ViewObserver  ViewObserverType;
  ViewObserverType::Pointer viewObserver = ViewObserverType::New();

  view3D->SetRefreshRate( refreshRate );

  viewObserver->SetView( view3D );
  viewObserver->SetEndFlag( &bEnd );

  const unsigned long numberOfSeconds = 2;
  const unsigned long numberOfPulsesToStop = 
    static_cast< unsigned long >( refreshRate * numberOfSeconds );

  const unsigned int numberOfFlicks = 4;

  for( unsigned int i=0; i<numberOfFlicks; i++)
    {
    viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
    viewObserver->Reset();

    transform1.SetTranslationAndRotation( 
        translation1, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoidObject1->RequestSetTransformAndParent( transform1, worldReference );

    std::cout << "Flick " << i <<  "   end flag =  " << bEnd << std::endl;

    view3D->RequestStart();

    while( !bEnd )
      {
      igstk::PulseGenerator::Sleep(20);
      igstk::PulseGenerator::CheckTimeouts();
      }

    view3D->RequestStop();

    }

 
  std::string screenShotFileName1 = argv[1]; 
  std::string screenShotFileName2 = argv[2];

  //
  //  Now visualize the object 3 under tracking
  //
  ellipsoidObject3->RequestSetTransformAndParent( 
    identityTransform, trackerTool );

  igstk::Transform calibrationTransform;

  calibrationTransform.SetToIdentity( 
    ::igstk::TimeStamp::GetLongestPossibleTime() );

  trackerTool->SetCalibrationTransform( calibrationTransform );

  trackerTool->RequestSetName("tool1");
  trackerTool->RequestConfigure();
  tracker->RequestSetTransformAndParent( identityTransform, worldReference );

 
  // restart the count of the observer
  viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
  viewObserver->Reset();

  view3D->RequestStart();

  const double speedInDegreesPerSecond = 36.0;
  const double radiusInMillimeters = 2.0;

  tracker->RequestOpen();
  tracker->SetRadius( radiusInMillimeters );
  tracker->SetAngularSpeed( speedInDegreesPerSecond );
  
  trackerTool->RequestAttachToTracker( tracker );

  tracker->RequestStartTracking();

  bool screenShotTaken = false;

  while( !bEnd )
    {
    igstk::PulseGenerator::Sleep(20);
    if ( ! screenShotTaken )  
      {
      view3D->RequestSaveScreenShot( screenShotFileName1 );
      screenShotTaken = true;
      }
    igstk::PulseGenerator::CheckTimeouts();
    }
  
  tracker->RequestStopTracking();
  tracker->RequestClose();
  view3D->RequestStop();

  // 
  // Now attach the View to the Tracker tool, in order to illustrate
  // how a display "from the point of view of the tracker tool" can
  // easily be obtained.
  //
  view3D->RequestSetTransformAndParent( identityTransform, trackerTool );

  viewObserver->SetNumberOfPulsesToStop( 50 );
  viewObserver->Reset();

  view3D->RequestStart();

  tracker->RequestOpen();
  tracker->RequestStartTracking();

  screenShotTaken = false;

  while( !bEnd )
    {
    igstk::PulseGenerator::Sleep(20);
    igstk::PulseGenerator::CheckTimeouts();
    }
  
  tracker->RequestStopTracking();

  tracker->RequestClose();
  view3D->RequestStop();
  igstk::PulseGenerator::Sleep(1);
  view3D->RequestSaveScreenShot( screenShotFileName2 );
  viewObserver->SetView(NULL);

  delete widget;

  if( !representationObserver1->GetTransformExpired() ||
      !representationObserver2->GetTransformExpired() ||
      !representationObserver3->GetTransformExpired() )
    {
    std::cerr << "Failure to capture the TransformExpiredErrorEvent properly" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "Success catching TransformExpiredErrorEvent()" << std::endl;
    }

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
