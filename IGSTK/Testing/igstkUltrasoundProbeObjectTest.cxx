/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundProbeObjectTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkConfigure.h"
#include "igstkUltrasoundProbeObject.h"
#include "igstkUltrasoundProbeObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
#endif

namespace igstk
{

namespace UltrasoundProbeObjectTest
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
    }

  void SetNumberOfPulsesToStop( unsigned long number )
    {
    m_NumberOfPulsesToStop = number;
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
      }
    }
private:
  unsigned long       m_PulseCounter;
  unsigned long       m_NumberOfPulsesToStop;
  Fl_Window  *        m_Form;
  ::igstk::View *     m_View;
  bool *              m_End;
};

class TransformObserver : public ::itk::Command 
{
public:
  typedef  TransformObserver          Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  TransformObserver() 
    {
    m_GotTransform = false;
    }
  ~TransformObserver() {}
public:
    
  typedef ::igstk::TransformModifiedEvent  EventType;
        
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    m_GotTransform = false;
    if( EventType().CheckEvent( &event ) )
      {
      const EventType * transformEvent = 
                    dynamic_cast< const EventType *>( &event );
      if( transformEvent )
        {
        m_Transform = transformEvent->Get();
        m_GotTransform = true;
        }
      }
    }

  bool GotTransform() const
    {
    return m_GotTransform;
    }

  const ::igstk::Transform & GetTransform() const
    {
    return m_Transform;
    }
        
private:

  ::igstk::Transform  m_Transform;
  bool m_GotTransform;

};


} // end of UltrasoundProbeObjectTest namespace

} // end of igstk namespace

int igstkUltrasoundProbeObjectTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer 
                              vtkLoggerOutput = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject  
    WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference =
    WorldReferenceSystemType::New();

  worldReference->SetLogger( logger );
  */
#endif 

  typedef igstk::UltrasoundProbeObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer UltrasoundProbeRepresentation 
                                            = ObjectRepresentationType::New();
  UltrasoundProbeRepresentation->SetLogger( logger );

  typedef igstk::UltrasoundProbeObject  ObjectType;
  ObjectType::Pointer UltrasoundProbeObject = ObjectType::New();
  UltrasoundProbeObject->SetLogger( logger );

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  // FIXCS UltrasoundProbeObject->RequestAttachToSpatialObjectParent( worldReference );
#endif 

  // Test Property
  std::cout << "Testing Property : ";
  UltrasoundProbeRepresentation->SetColor(0.1,0.2,0.3);
  UltrasoundProbeRepresentation->SetOpacity(0.4);
  if(fabs(UltrasoundProbeRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cerr << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(UltrasoundProbeRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(UltrasoundProbeRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(UltrasoundProbeRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  UltrasoundProbeRepresentation->RequestSetUltrasoundProbeObject(
                                                       UltrasoundProbeObject);
  UltrasoundProbeRepresentation->Print(std::cout);
  UltrasoundProbeObject->Print(std::cout);
  UltrasoundProbeObject->GetNameOfClass();
  UltrasoundProbeRepresentation->GetNameOfClass();

  // testing actors
  std::cout << "Testing actors : ";

  Fl_Window * form = new Fl_Window(512,512,"View2D Test");

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(6,6,500,500,"View 2D");
  view2D->SetLogger( logger );

  form->end();
  // End of the GUI creation

  form->show();
  

  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( UltrasoundProbeRepresentation );
  std::cout << "[PASSED]" << std::endl;

  // Testing again in order to exercise the other half of an if().
  UltrasoundProbeRepresentation->SetColor(0.3,0.7,0.2);

  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 2000.0;
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  typedef ::igstk::UltrasoundProbeObjectTest::TransformObserver  
                                                        TransformObserverType;

  TransformObserverType::Pointer 
                             transformObserver = TransformObserverType::New();

  UltrasoundProbeObject->AddObserver( ::igstk::TransformModifiedEvent(),
                                                          transformObserver );

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  // FIXCS UltrasoundProbeObject->RequestSetTransformToSpatialObjectParent( transform );
#else
  UltrasoundProbeObject->RequestSetTransform( transform );
#endif
  
  UltrasoundProbeObject->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The UltrasoundProbeObject did not returned \
                 a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();

  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" 
                << std::endl;
      std::cerr << "input  translation = " << translation << std::endl;
      std::cerr << "output translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
      fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
      fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
      fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "Rotation component is out of range [FAILED]" << std::endl;
    std::cerr << "input  rotation = " << rotation << std::endl;
    std::cerr << "output rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;

  // Testing the Copy() function
  std::cout << "Testing Copy(): ";
  ObjectRepresentationType::Pointer 
                                  copy = UltrasoundProbeRepresentation->Copy();
  if(copy->GetOpacity() != UltrasoundProbeRepresentation->GetOpacity())
    {
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetUltrasoundProbeObject() with a null pointer as argument
  std::cout << "Testing RequestSetUltrasoundProbeObject() with NULL argument: ";
  ObjectRepresentationType::Pointer 
             UltrasoundProbeRepresentation3 = ObjectRepresentationType::New();
  UltrasoundProbeRepresentation3->RequestSetUltrasoundProbeObject( 0 );

  // Exercise RequestSetUltrasoundProbeObject() called twice. 
  // The second call should be ignored.
  std::cout << "Testing RequestSetUltrasoundProbeObject() called twice: ";
  ObjectRepresentationType::Pointer 
             UltrasoundProbeRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer UltrasoundProbeObjectA = ObjectType::New();
  ObjectType::Pointer UltrasoundProbeObjectB = ObjectType::New();
  UltrasoundProbeRepresentation4->RequestSetUltrasoundProbeObject( 
                                                     UltrasoundProbeObjectA );
  UltrasoundProbeRepresentation4->RequestSetUltrasoundProbeObject( 
                                                     UltrasoundProbeObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  UltrasoundProbeRepresentation->SetColor(0.9,0.7,0.1);
  UltrasoundProbeRepresentation->SetOpacity(0.8);

  std::cout << UltrasoundProbeRepresentation << std::endl;
  std::cout << UltrasoundProbeObjectA << std::endl;

  bool bEnd = false;

  typedef ::igstk::UltrasoundProbeObjectTest::ViewObserver ObserverType;
  ObserverType::Pointer viewObserver = ObserverType::New();
  
  view2D->RequestSetRefreshRate( 20 );
  view2D->RequestResetCamera();
  view2D->RequestEnableInteractions();

  viewObserver->SetView( view2D );
  viewObserver->SetForm( form );
  viewObserver->SetEndFlag( &bEnd );

  view2D->RequestStart();

  while(1)
    {
    Fl::wait(0.0001);
    igstk::PulseGenerator::CheckTimeouts();
    if( bEnd )
      {
      break;
      }
    }
  
  std::cout << "Test [DONE]" << std::endl;

  delete view2D;
  delete form;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
