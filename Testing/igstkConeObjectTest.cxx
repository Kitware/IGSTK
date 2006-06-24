/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkConeObjectTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkConeObject.h"
#include "igstkConeObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{
namespace ConeObjectTest
{
  class TransformObserver : public ::itk::Command 
  {
  public:
    typedef  TransformObserver   Self;
    typedef  ::itk::Command    Superclass;
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

} // end namespace ConeObjectTest
} // end namespace igstk


int igstkConeObjectTest( int, char * [] )
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
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK OutputWindow -> logger

  typedef igstk::ConeObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer ConeRepresentation = ObjectRepresentationType::New();
  ConeRepresentation->SetLogger( logger );

  typedef igstk::ConeObject  ObjectType;
  ObjectType::Pointer ConeObject = ObjectType::New();
  ConeObject->SetLogger( logger );
    
  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  ConeObject->SetRadius(3.2);

  double radiusRead = ConeObject->GetRadius();
  if(radiusRead != 3.2)
    {
    std::cerr << "Radius error : " << radiusRead << " v.s " << 3.2 << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Set/GetHeight()
  std::cout << "Testing Set/GetHeight() : ";
  ConeObject->SetHeight(4.1);

  double heightRead = ConeObject->GetHeight();
  if(heightRead != 4.1)
    {
    std::cerr << "Height error : " << heightRead << " v.s " << 4.1 << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  ConeRepresentation->SetColor(0.1,0.2,0.3);
  ConeRepresentation->SetOpacity(0.4);
  if(fabs(ConeRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cerr << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ConeRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ConeRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ConeRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  ConeRepresentation->RequestSetConeObject(ConeObject);
  ConeRepresentation->Print(std::cout);
  ConeObject->Print(std::cout);
  ConeObject->GetNameOfClass();
  ConeRepresentation->GetNameOfClass();

  // testing actors
  std::cout << "Testing actors : ";

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  view2D->SetLogger( logger );

  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( ConeRepresentation );
    
  std::cout << "[PASSED]" << std::endl;

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  ConeObject->SetRadius( 10.0 );
  
  ConeRepresentation->SetColor(0.3,0.7,0.2);
  
  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 20000.0;  // 20 seconds
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

  ConeObject->RequestSetTransform( transform );

  typedef ::igstk::ConeObjectTest::TransformObserver  TransformObserverType;

  TransformObserverType::Pointer transformObserver = TransformObserverType::New();

  ConeObject->AddObserver( ::igstk::TransformModifiedEvent(), transformObserver );
  
  ConeObject->RequestSetTransform( transform );
  ConeObject->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The ConeObject did not returned a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();

  
  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" << std::endl;
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
  ObjectRepresentationType::Pointer copy = ConeRepresentation->Copy();
  if(copy->GetOpacity() != ConeRepresentation->GetOpacity())
    {
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetConeObject() with a null pointer as argument
  std::cout << "Testing RequestSetConeObject() with NULL argument: ";
  ObjectRepresentationType::Pointer ConeRepresentation3 = ObjectRepresentationType::New();
  ConeRepresentation3->RequestSetConeObject( 0 );

  // Exercise RequestSetConeObject() called twice. The second call should be ignored.
  std::cout << "Testing RequestSetConeObject() called twice: ";
  ObjectRepresentationType::Pointer ConeRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer ConeObjectA = ObjectType::New();
  ObjectType::Pointer ConeObjectB = ObjectType::New();
  ConeRepresentation4->RequestSetConeObject( ConeObjectA );
  ConeRepresentation4->RequestSetConeObject( ConeObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  ConeRepresentation->SetColor(0.9,0.7,0.1);
  ConeRepresentation->SetOpacity(0.8);

  std::cout << ConeRepresentation << std::endl;
  std::cout << ConeObjectA << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  delete view2D;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
