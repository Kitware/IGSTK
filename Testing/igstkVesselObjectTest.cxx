/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVesselObjectTest.cxx
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

#include "igstkVesselObject.h"
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace igstk
{
namespace VesselObjectTest
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

} // end namespace VesselObjectTest
} // end namespace igstk


int igstkVesselObjectTest( int, char * [] )
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

  typedef igstk::VesselObject     ObjectType;
  typedef ObjectType::PointType TubePointType;

  ObjectType::Pointer VesselObject = ObjectType::New();
  VesselObject->SetLogger( logger );

  // Test Set/GetRadius()
  TubePointType p1;
  p1.SetPosition(0,0,0);
  p1.SetRadius(2);
  VesselObject->AddPoint(p1);
  TubePointType p2;
  p2.SetPosition(10,10,10);
  p2.SetRadius(10);
  VesselObject->AddPoint(p2);

  std::cout << "Testing the GetPoints() method: ";
  ObjectType::PointListType points = VesselObject->GetPoints();
  if(points.size() != 2)
    {
    std::cout << "GetPoints error : " << points.size() << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetNumberOfPoints: ";
  unsigned int nPoints = VesselObject->GetNumberOfPoints();
  if(nPoints != 2)
    {
    std::cout << "GetNumberOfPoints error : " << nPoints << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetPoint: ";
  const TubePointType* pt = VesselObject->GetPoint(1);
  if(!pt)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }
  const TubePointType* ptout = VesselObject->GetPoint(10);
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

  // Testing PrintSelf()
  VesselObject->GetNameOfClass();
  VesselObject->Print(std::cout);

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

  VesselObject->RequestSetTransform( transform );

  typedef ::igstk::VesselObjectTest::TransformObserver  TransformObserverType;

  TransformObserverType::Pointer transformObserver 
                                               = TransformObserverType::New();

  VesselObject->AddObserver( ::igstk::TransformModifiedEvent(), 
                                          transformObserver );
  
  VesselObject->RequestSetTransform( transform );
  VesselObject->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The VesselObject did not returned a Transform event" 
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

  std::cout << "Testing Clear(): ";
  VesselObject->Clear();
  if(VesselObject->GetNumberOfPoints()>0)
    {
    std::cerr << "Clear() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
