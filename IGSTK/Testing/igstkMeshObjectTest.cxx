/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObjectTest.cxx
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
#include "igstkMeshObject.h"
#include "igstkMeshReader.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkView3D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
#endif

namespace igstk
{

namespace MeshObjectTest
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
  Fl_Window *         m_Form;
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
  
} // end of MeshObjectTest namespace

} // end namespace igstk


int igstkMeshObjectTest( int argc, char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
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

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject  
    WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference =
    WorldReferenceSystemType::New();

  worldReference->SetLogger( logger );
  */
#endif 

  typedef igstk::MeshObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer MeshRepresentation = 
                                          ObjectRepresentationType::New();
  MeshRepresentation->SetLogger( logger );

  typedef igstk::MeshObject     ObjectType;
  typedef ObjectType::PointType MeshPointType;

  ObjectType::Pointer meshObject = ObjectType::New();
  meshObject->SetLogger( logger );

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  // FIXCS meshObject->RequestAttachToSpatialObjectParent( worldReference );
#endif 

  meshObject->AddPoint(0,0,0,0);
  meshObject->AddPoint(1,9,0,0);
  meshObject->AddPoint(2,9,9,0);
  meshObject->AddPoint(3,0,0,9);
  meshObject->AddTetrahedronCell(0,0,1,2,3);
  meshObject->AddTriangleCell(1,0,1,2);
   
  if( argc > 1 )
    {
    typedef igstk::MeshReader    ReaderType;

    ReaderType::Pointer  reader = ReaderType::New();

    std::string filename = argv[1];
    reader->RequestSetFileName( filename );
    reader->RequestReadObject();
    MeshRepresentation->RequestSetMeshObject( reader->GetOutput() );
    }
  else
    {
    MeshRepresentation->RequestSetMeshObject( meshObject );
    }


  // Test Property
  std::cout << "Testing Property : ";
  MeshRepresentation->SetColor(0.1,0.2,0.3);
  MeshRepresentation->SetOpacity(0.4);
  if(fabs(MeshRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;


  // Testing PrintSelf()
  MeshRepresentation->Print(std::cout);
  MeshRepresentation->GetNameOfClass();
  meshObject->GetNameOfClass();
  meshObject->Print(std::cout);

  // Testing CreateActors()
  std::cout << "Testing actors : ";

  Fl_Window * form = new Fl_Window(512,512,"MeshObject Test");

  typedef igstk::View3D  View3DType;
  View3DType * view3D = new View3DType(6,6,500,500,"View 3D");
  view3D->SetLogger( logger );
  
  std::cout << "form->end()" << std::endl;
  form->end();
  // End of the GUI creation

  form->show();
  
  // this will indirectly call CreateActors() 
  view3D->RequestAddObject( MeshRepresentation );

  // Testing again in order to exercise the other half of an if().
  MeshRepresentation->SetColor(0.3,0.7,0.2);
  
  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 5000.0;
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

  typedef ::igstk::MeshObjectTest::TransformObserver  TransformObserverType;

  TransformObserverType::Pointer transformObserver = 
                                                TransformObserverType::New();

  meshObject->AddObserver( ::igstk::TransformModifiedEvent(), 
                           transformObserver );

#ifdef IGSTK_USE_COORDINATE_REFERENCE_SYSTEM
  // FIXCS meshObject->RequestSetTransformToSpatialObjectParent( transform );
#else
  meshObject->RequestSetTransform( transform );
#endif

#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  meshObject->RequestGetTransform();
#endif

  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The MeshObject did not returned ";
    std::cerr << "a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();

  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range";
      std::cerr << " [FAILED]" << std::endl;
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

  // Exercise Copy() method
  ObjectRepresentationType::Pointer MeshRepresentation2 = 
                                                  MeshRepresentation->Copy();
  view3D->RequestAddObject( MeshRepresentation2 );
  if(MeshRepresentation2->GetOpacity() != MeshRepresentation->GetOpacity())
    {
    std::cerr << "Copy() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetMeshObject() with a null pointer as argument
  std::cout << "Testing RequestSetMeshObject() with NULL argument: ";
  ObjectRepresentationType::Pointer MeshRepresentation3 = 
                                              ObjectRepresentationType::New();
  MeshRepresentation3->RequestSetMeshObject( 0 );

  // Exercise RequestSetMeshObject() called twice. 
  // The second call should be ignored.
  std::cout << "Testing RequestSetMeshObject() called twice: ";
  ObjectRepresentationType::Pointer MeshRepresentation4 = 
                                              ObjectRepresentationType::New();
  ObjectType::Pointer MeshObjectA = ObjectType::New();
  ObjectType::Pointer MeshObjectB = ObjectType::New();
  MeshRepresentation4->RequestSetMeshObject( MeshObjectA );
  MeshRepresentation4->RequestSetMeshObject( MeshObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  MeshRepresentation->SetColor(0.9,0.7,0.1);
  MeshRepresentation->SetOpacity(0.8);

  std::cout << MeshRepresentation << std::endl;
  std::cout << MeshObjectA << std::endl;

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;


  bool bEnd = false;

  typedef ::igstk::MeshObjectTest::ViewObserver ObserverType;
  ObserverType::Pointer viewObserver = ObserverType::New();
  
  const double refreshRate = 20.0;

  view3D->RequestSetRefreshRate( refreshRate );
  view3D->RequestResetCamera();
  view3D->RequestEnableInteractions();

  viewObserver->SetView( view3D );
  viewObserver->SetForm( form );
  viewObserver->SetEndFlag( &bEnd );

  const unsigned long numberOfSeconds = 4;
  const unsigned long numberOfPulsesToStop = 
    static_cast< unsigned long >( refreshRate * numberOfSeconds );

  viewObserver->SetNumberOfPulsesToStop( numberOfPulsesToStop );
 

  view3D->RequestStart();

  while(1)
    {
    Fl::wait(0.0001);
    igstk::PulseGenerator::CheckTimeouts();
    if( bEnd )
      {
      break;
      }
    }

  // Verify that the object appears if the Transform is updated.
  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  meshObject->RequestSetTransform( transform );
#endif

  while(1)
    {
    Fl::wait(0.0001);
    igstk::PulseGenerator::CheckTimeouts();
    if( bEnd )
      {
      break;
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
