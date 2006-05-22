/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObjectTest.cxx
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
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkTubeObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkView2D.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{
namespace TubeObjectTest
{
  
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

} // end namespace TubeObjectTest
} // end namespace igstk


int igstkTubeObjectTest( int, char * [] )
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
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                              = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);

  typedef igstk::TubeObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer TubeRepresentation 
                                            = ObjectRepresentationType::New();
  TubeRepresentation->SetLogger( logger );

  typedef igstk::TubeObject     ObjectType;
  typedef ObjectType::PointType TubePointType;

  ObjectType::Pointer TubeObject = ObjectType::New();
  TubeObject->SetLogger( logger );

  TubeRepresentation->RequestSetTubeObject( TubeObject );

  // Test Set/GetRadius()
  TubePointType p1;
  p1.SetPosition(0,0,0);
  p1.SetRadius(2);
  TubeObject->AddPoint(p1);
  TubePointType p2;
  p2.SetPosition(10,10,10);
  p2.SetRadius(10);
  TubeObject->AddPoint(p2);

  std::cout << "Testing the GetPoints() method: ";
  ObjectType::PointListType points = TubeObject->GetPoints();
  if(points.size() != 2)
    {
    std::cout << "GetPoints error : " << points.size() << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetNumberOfPoints: ";
  unsigned int nPoints = TubeObject->GetNumberOfPoints();
  if(nPoints != 2)
    {
    std::cout << "GetNumberOfPoints error : " << nPoints << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetPoint: ";
  const TubePointType* pt = TubeObject->GetPoint(1);
  if(!pt)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }
  const TubePointType* ptout = TubeObject->GetPoint(10);
  if(ptout)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }

  if(pt->GetPosition()[0] != 10)
    {
    std::cout << "GetPoint error: Position is not valid" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  TubeRepresentation->SetColor(0.1,0.2,0.3);
  TubeRepresentation->SetOpacity(0.4);
  if(fabs(TubeRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(TubeRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(TubeRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(TubeRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  TubeRepresentation->Print(std::cout);
  TubeRepresentation->GetNameOfClass();
  TubeObject->GetNameOfClass();
  TubeObject->Print(std::cout);

  // Testing CreateActors()
  std::cout << "Testing actors : ";

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  view2D->SetLogger( logger );

  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( TubeRepresentation );

  // Testing Update
  TubeRepresentation->IsModified();

  // Testing again in order to exercise the other half of an if().
  TubeRepresentation->IsModified();
  TubeRepresentation->SetColor(0.3,0.7,0.2);
  if( !TubeRepresentation->IsModified() )
    {
    std::cerr << "IsModified() failed to be true after a SetColor()" 
              << std::endl;
    return EXIT_FAILURE;
    }

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

  TubeObject->RequestSetTransform( transform );

  typedef ::igstk::TubeObjectTest::TransformObserver  TransformObserverType;

  TransformObserverType::Pointer transformObserver 
                                               = TransformObserverType::New();

  TubeObject->AddObserver( ::igstk::TransformModifiedEvent(), 
                                          transformObserver );
  
  TubeObject->RequestSetTransform( transform );
  TubeObject->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The TubeObject did not returned a Transform event" 
              << std::endl;
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

  // Exercise Copy() method
  ObjectRepresentationType::Pointer TubeRepresentation2 
                                                = TubeRepresentation->Copy();
  view2D->RequestAddObject( TubeRepresentation2 );
  if(TubeRepresentation2->GetOpacity() != TubeRepresentation->GetOpacity())
    {
    std::cerr << "Copy() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetTubeObject() with a null pointer as argument
  std::cout << "Testing RequestSetTubeObject() with NULL argument: ";
  ObjectRepresentationType::Pointer TubeRepresentation3 
                                            = ObjectRepresentationType::New();
  TubeRepresentation3->RequestSetTubeObject( 0 );

  // Exercise RequestSetTubeObject() called twice. 
  // The second call should be ignored.
  std::cout << "Testing RequestSetTubeObject() called twice: ";
  ObjectRepresentationType::Pointer TubeRepresentation4 
                                            = ObjectRepresentationType::New();
  ObjectType::Pointer TubeObjectA = ObjectType::New();
  ObjectType::Pointer TubeObjectB = ObjectType::New();
  TubeRepresentation4->RequestSetTubeObject( TubeObjectA );
  TubeRepresentation4->RequestSetTubeObject( TubeObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  TubeRepresentation->SetColor(0.9,0.7,0.1);
  TubeRepresentation->SetOpacity(0.8);

  std::cout << TubeRepresentation << std::endl;
  std::cout << TubeObjectA << std::endl;

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing Clear(): ";
  TubeObject->Clear();
  if(TubeObject->GetNumberOfPoints()>0)
    {
    std::cerr << "Clear() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;


  std::cout << "Test [DONE]" << std::endl;

  delete view2D;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
