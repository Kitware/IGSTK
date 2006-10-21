/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectRepresentationVisibilityTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

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
    m_NumberOfPulsesToStop = 100;
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

  unsigned long       m_PulseCounter;
  unsigned long       m_NumberOfPulsesToStop;
  Fl_Window          *m_Form;
  ::igstk::View      *m_View;
  bool *              m_End;
};


} // end of VisibilityObjectTest namespace

} // end namespace igstk


int igstkSpatialObjectRepresentationVisibilityTest( int argc, char * argv [] )
{

  igstk::RealTimeClock::Initialize();

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
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK OutputWindow 

  typedef igstk::EllipsoidObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation = ObjectRepresentationType::New();
  ellipsoidRepresentation->SetLogger( logger );

  ObjectRepresentationType::Pointer 
                    ellipsoidRepresentation2 = ObjectRepresentationType::New();

  typedef igstk::EllipsoidObject  ObjectType;
  ObjectType::Pointer ellipsoidObject = ObjectType::New();
  ellipsoidObject->SetLogger( logger );
 
  ObjectType::Pointer ellipsoidObject2 = ObjectType::New();
  ellipsoidObject2->SetLogger( logger );
 
  ellipsoidRepresentation2->RequestSetEllipsoidObject(ellipsoidObject2);
  ellipsoidRepresentation2->SetColor( 1.0, 0.0, 0.0 );
  ellipsoidRepresentation2->SetOpacity( 1.0 );
  
   
  ellipsoidRepresentation->RequestSetEllipsoidObject(ellipsoidObject);
  ellipsoidRepresentation->SetColor( 0.0, 0.0, 1.0 );
  ellipsoidRepresentation->SetOpacity( 1.0 );
  
   Fl_Window * form = new Fl_Window(512,512,"Visibility Test");

  typedef igstk::View3D  View3DType;
  View3DType * view3D = new View3DType(6,6,500,500,"View 3D");
  view3D->SetLogger( logger );
  
  form->end();
  // End of the GUI creation

  form->show();
  
  // this will indirectly call CreateActors() 
  view3D->RequestAddObject( ellipsoidRepresentation  );
  view3D->RequestAddObject( ellipsoidRepresentation2 );

  double validityTimeInMilliseconds = 1000;
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns


  igstk::Transform transform2;
  igstk::Transform::VectorType translation2;
  translation2[0] = 0;
  translation2[1] = 2;
  translation2[2] = 2;

  
  bool bEnd = false;
  const double refreshRate = 20.0;

  typedef ::igstk::VisibilityObjectTest::ViewObserver  ViewObserverType;
  ViewObserverType::Pointer viewObserver = ViewObserverType::New();

  view3D->RequestSetRefreshRate( refreshRate );
  view3D->RequestEnableInteractions();

  viewObserver->SetView( view3D );
  viewObserver->SetForm( form );
  viewObserver->SetEndFlag( &bEnd );

  const unsigned long numberOfSeconds = 3;
  const unsigned long numberOfPulsesToStop = 
    static_cast< unsigned long >( refreshRate * numberOfSeconds );

  view3D->RequestStart();

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  ellipsoidObject->RequestSetTransform( transform );

  transform2.SetTranslationAndRotation( 
      translation2, rotation, errorValue, 
      ::igstk::TimeStamp::GetLongestPossibleTime() );

  ellipsoidObject2->RequestSetTransform( transform2 );

  
  // Stabilize the transient period where the camera
  // must be reset.
  for(unsigned int k=0; k<10; k++)
    {
    Fl::wait(0.05);
    igstk::PulseGenerator::CheckTimeouts();
    view3D->RequestResetCamera();
    }

  
  // Now start the real test.
  //
  // 1) Set the transform to be valid for one second
  // 2) Reset the pulses counter in the view observer
  //    to count for 3 seconds.
  //
  // 3) Restart the pulse generator of the View.
  //
  for( unsigned int tt=0; tt<5; tt++ )
    {
    validityTimeInMilliseconds = 1000;
    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoidObject->RequestSetTransform( transform );

    // restart the count of the observer
    viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
    bEnd = false;
    view3D->RequestStart();

    while(1)
      {
      Fl::wait(0.001);
      igstk::PulseGenerator::CheckTimeouts();
      if( bEnd )
        {
        break;
        }
      }
    }


  delete view3D;
  
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
