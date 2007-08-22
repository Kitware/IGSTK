/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemObjectTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkCoordinateReferenceSystemObject.h"
#include "igstkWorldCoordinateReferenceSystemObject.h"
#include "igstkRealTimeClock.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkVTKLoggerOutput.h"

namespace igstk
{
namespace CoordinateReferenceSystemObjectTest
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
  bool                m_GotTransform;

};

} // end namespace CoordinateReferenceSystemObjectTest
} // end namespace igstk


int igstkCoordinateReferenceSystemObjectTest( int, char * [] )
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
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger
  typedef igstk::WorldCoordinateReferenceSystemObject  
    WorldReferenceSystemType;

  WorldReferenceSystemType::Pointer worldReference =
    WorldReferenceSystemType::New();

  worldReference->SetLogger( logger );
 
  typedef igstk::CoordinateReferenceSystemObject  ObjectType;
  ObjectType::Pointer coordinateSystem = ObjectType::New();
  coordinateSystem->SetLogger( logger );

  coordinateSystem->RequestAttachToSpatialObjectParent( worldReference );
    
  // Test Set/GetRadius()
  std::cout << "Testing Set/GetSize() : ";
  coordinateSystem->SetSize(10,20,30);

  if(coordinateSystem->GetSizeX() != 10.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX() 
              << " v.s " << 10.0 << std::endl; 
    return EXIT_FAILURE;
    }
   
  if(coordinateSystem->GetSizeY() != 20.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX()
              << " v.s " << 20.0 << std::endl; 
    return EXIT_FAILURE;
    }
 
  if(coordinateSystem->GetSizeZ() != 30.0)
    {
    std::cerr << "SizeX error : " << coordinateSystem->GetSizeX() 
              << " v.s " << 30.0 << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  coordinateSystem->SetSize( 20.0, 30.0, 40.0 );
  
  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  const double validityTimeInMilliseconds = 20000.0; // 20 seconds
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


  typedef ::igstk::CoordinateReferenceSystemObjectTest::TransformObserver
     TransformObserverType;

  TransformObserverType::Pointer transformObserver 
                                               = TransformObserverType::New();

  coordinateSystem->AddObserver( ::igstk::TransformModifiedEvent(), 
                                                          transformObserver );
  
  coordinateSystem->RequestSetTransformToSpatialObjectParent( transform );
  coordinateSystem->RequestGetTransform();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The CoordinateReferenceSystemObject did not returned a Transform event" 
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

  std::cout << "[PASSED]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Test Failing due to error messages ";
    std::cout << " received in vtkLoggerOutput" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
