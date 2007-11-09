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
#include "igstkViewNew3D.h"
#include "igstkFLTKWidget.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

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

  void SetView( ::igstk::ViewNew * view )
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
  ::igstk::ViewNew::Pointer  m_View;
  bool *                     m_End;
};

class MyTracker : public ::igstk::Tracker
{
public:

    /** Macro with standard traits declarations. */
    igstkStandardClassTraitsMacro( MyTracker, ::igstk::Tracker )

    typedef Superclass::TransformType           TransformType;

    void GetTransform(TransformType & transform) 
          { this->GetToolTransform(0, 0, transform); }

protected:
    MyTracker():m_StateMachine(this) 
      {
      m_Position[0] = -1.0;
      m_Position[1] = -0.5;
      m_Position[2] =  0.0;

      m_ValidityTime = 1000.0; // 1 second
      }
    virtual ~MyTracker() {};

    typedef ::igstk::Tracker::ResultType        ResultType;
    typedef TransformType::VectorType           PositionType;
    typedef TransformType::ErrorType            ErrorType;

    virtual ResultType InternalOpen( void ) { return SUCCESS; }
    virtual ResultType InternalActivateTools( void ) 
      {
      igstkLogMacro( DEBUG, "MyTracker::InternalActivateTools called ...\n");
      TrackerPortType::Pointer port = TrackerPortType::New();
      port->AddTool( TrackerToolType::New() );
      this->AddPort( port );
      return SUCCESS;
      }
    virtual ResultType InternalStartTracking( void ) { return SUCCESS; }
    virtual ResultType InternalUpdateStatus( void ) 
      { 
      TransformType transform;
      transform.SetToIdentity( m_ValidityTime );
      
      m_Position[0] += 0.01;  // drift along a vector (1.0, 2.0, 3.0)
      m_Position[1] += 0.00;  // just to simulate a linear movement
      m_Position[2] += 0.00;  // being tracked in space.

      ErrorType errorValue = 0.5; 

      transform.SetTranslation( m_Position, errorValue, m_ValidityTime );
      this->SetToolTransform( 0, transform );

      std::cout << "MyTracker::InternalUpdateStatus() " 
                << m_Position << std::endl;
  
      return SUCCESS; 
      }
    virtual ResultType InternalReset( void ) { return SUCCESS; }
    virtual ResultType InternalStopTracking( void ) { return SUCCESS; }
    virtual ResultType InternalDeactivateTools( void ) 
      { 
      m_Port = TrackerPortType::New();
      m_Tool = TrackerToolType::New();
      m_Port->AddTool( m_Tool );
      this->AddPort( m_Port );
      return SUCCESS; 
      }
    virtual ResultType InternalClose( void ) { return SUCCESS; }

private:

    MyTracker(const Self&);  //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    typedef TrackerTool                 TrackerToolType;
    typedef TrackerPort                 TrackerPortType;
    typedef Transform::TimePeriodType   TimePeriodType;
    TimePeriodType                      m_ValidityTime;
    TrackerPortType::Pointer            m_Port;
    TrackerToolType::Pointer            m_Tool;
    PositionType                        m_Position;
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
  
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;

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

  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject  
    WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference =
    WorldReferenceSystemType::New();
  */

  typedef igstk::AxesObjectRepresentation  RepresentationType;
  RepresentationType::Pointer AxesRepresentation = RepresentationType::New();
  // FIXCS AxesRepresentation->RequestSetAxesObject( worldReference );
  // FIXCS worldReference->SetSize( 0.5, 0.5, 0.5 );

  typedef ::igstk::VisibilityObjectTest::MyTracker    TrackerType;
  typedef ::igstk::TrackerTool                        TrackerToolType;

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
 
  /* FIXCS
  ellipsoidObject1->RequestAttachToSpatialObjectParent( worldReference );
  ellipsoidObject2->RequestAttachToSpatialObjectParent( worldReference );
#ifdef OBJECTFIXED
  ellipsoidObject3->RequestAttachToSpatialObjectParent( worldReference );
#endif
  */

#ifdef OBJECTTRACKED
  trackerTool->RequestAttachSpatialObject( ellipsoidObject3 );
  igstk::Transform calibrationTransform;
  calibrationTransform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  // FIXCS ellipsoidObject3->RequestSetCalibrationTransformToTrackerTool( calibrationTransform );
#endif
  tracker->RequestAddTool( trackerTool );
  // FIXCS tracker->RequestAttachToSpatialObjectParent( worldReference );

  igstk::Transform trackerTransform;
  trackerTransform.SetToIdentity( ::igstk::TimeStamp::GetLongestPossibleTime() );
  // FIXCS tracker->RequestSetTransformToSpatialObjectParent( trackerTransform );

   
  ellipsoidRepresentation1->RequestSetEllipsoidObject(ellipsoidObject1);
  ellipsoidRepresentation1->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidRepresentation1->SetOpacity( 1.0 );
 
  ellipsoidRepresentation2->RequestSetEllipsoidObject(ellipsoidObject2);
  ellipsoidRepresentation2->SetColor( 1.0, 0.0, 0.0 );
  ellipsoidRepresentation2->SetOpacity( 1.0 );
  
  ellipsoidRepresentation3->RequestSetEllipsoidObject(ellipsoidObject3);
  ellipsoidRepresentation3->SetColor( 0.0, 1.0, 0.0 );
  ellipsoidRepresentation3->SetOpacity( 1.0 );
  
 
  typedef igstk::ViewNew3D  View3DType;

  View3DType::Pointer view3D = View3DType::New();
  view3D->SetLogger( logger );
 
  // Create an FLTK minimal GUI
  typedef igstk::FLTKWidget      FLTKWidgetType;

  Fl_Window * form = new Fl_Window(512,512,"Visibility Test");
  FLTKWidgetType * fltkWidget = new FLTKWidgetType(6,6,500,500,"View 2D");

  fltkWidget->RequestSetView( view3D );
  
  form->end();
  // End of the GUI creation

  form->show();
  
  // this will indirectly call CreateActors() 
  // FIXCS view3D->RequestAddObject( AxesRepresentation );
  view3D->RequestAddObject( ellipsoidRepresentation1  );
  view3D->RequestAddObject( ellipsoidRepresentation2 );
  view3D->RequestAddObject( ellipsoidRepresentation3 );

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

  view3D->RequestStart();

  transform1.SetTranslationAndRotation( 
      translation1, rotation, errorValue, validityTimeInMilliseconds );

  // FIXCS ellipsoidObject1->RequestSetTransformToSpatialObjectParent( transform1 );

  // This is the transform for the permanent object.
  igstk::Transform transform2;
  igstk::Transform::VectorType translation2;
  translation2[0] = 0.0;
  translation2[1] = 1.0;
  translation2[2] = 0.0;

  transform2.SetTranslationAndRotation( 
      translation2, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  // FIXCS ellipsoidObject2->RequestSetTransformToSpatialObjectParent( transform2 );

  
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

  ellipsoidObject3->RequestSetTransformToSpatialObjectParent( transform3 );
#endif


  // Stabilize the transient period where the camera
  // must be reset.
  for(unsigned int k=0; k<10; k++)
    {
    Fl::wait(0.05);
    igstk::PulseGenerator::CheckTimeouts();
    view3D->RequestResetCamera();
    }

 
  std::string screenShotFileName1 = argv[1]; 
  std::string screenShotFileName2 = argv[2];

  bool screenShotTaken = false;

  // restart the count of the observer
  viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
  bEnd = false;
  view3D->RequestStart();

  tracker->RequestOpen();
  tracker->RequestInitialize();
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
