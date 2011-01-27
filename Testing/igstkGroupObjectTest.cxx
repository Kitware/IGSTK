/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGroupObjectTest.cxx
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

#include "igstkConfigure.h"
#include "igstkAxesObject.h"
#include "igstkGroupObject.h"
#include "igstkTubeObject.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkTransformObserver.h"

namespace igstk
{
namespace GroupObjectTest
{

class ChildObserver : public ::itk::Command 
{
public:
  typedef  ChildObserver              Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  ChildObserver() 
    {
    m_Child = NULL;
    m_GotChild = false;
    m_GotChildNotAvailableMessage = true;
    }
  ~ChildObserver() {}
public:

  typedef ::igstk::SpatialObjectModifiedEvent      PositiveEventType;
  typedef ::igstk::SpatialObjectNotAvailableEvent  NegativeEventType;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {

    m_Child = NULL;
    m_GotChild = false;
    m_GotChildNotAvailableMessage = true;

    if( PositiveEventType().CheckEvent( &event ) )
      {
      const PositiveEventType * spatialObjectEvent = 
                 dynamic_cast< const PositiveEventType *>( &event );
      if( spatialObjectEvent )
        {
        m_Child = spatialObjectEvent->Get();
        m_GotChild = true;
        m_GotChildNotAvailableMessage = false;
        }
      }

    if( NegativeEventType().CheckEvent( &event ) )
      {
      const NegativeEventType * negativeEvent = 
                 dynamic_cast< const NegativeEventType *>( &event );
      if( negativeEvent )
        {
        m_Child = NULL;
        m_GotChild = false;
        m_GotChildNotAvailableMessage = true;
        }
      }
    
    }

  bool GotChild() const
    {
    return m_GotChild;
    }

  bool GotChildNotAvailableMessage() const
    {
    return m_GotChildNotAvailableMessage;
    }

  const ::igstk::SpatialObject * GetChild() const
    {
    return m_Child;
    }
        
private:

  ::igstk::SpatialObject *  m_Child;
  bool                      m_GotChild;
  bool                      m_GotChildNotAvailableMessage;
};
} // end namespace GroupObjectTest
} // end namespace igstk


int igstkGroupObjectTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
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
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow -> logger

  // Create the referene system
  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();

  typedef igstk::GroupObject  ObjectType;
  ObjectType::Pointer group = ObjectType::New();
  group->SetLogger( logger );
    
  igstk::Transform identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  group->RequestSetTransformAndParent( identityTransform, worldReference );

  // Testing PrintSelf()
  group->Print(std::cout);
  group->GetNameOfClass();


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

  typedef igstk::TransformObserver TransformObserverType;

  TransformObserverType::Pointer transformObserver 
    = TransformObserverType::New();

  transformObserver->ObserveTransformEventsFrom( group );

  transformObserver->Clear();

  group->RequestSetTransformAndParent( transform, worldReference );

  group->RequestGetTransformToParent();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The GroupObject did not returned a Transform event" 
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

  //
  //  Test methods related to child addition.
  //
  if( group->GetNumberOfChildren()  != 0 )
    {
    std::cerr << "Error: initial number of children is not zero" << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::TubeObject  ChildObjectType;

  const unsigned int maximumNumberOfChildrenToTry = 100;

  ChildObjectType::Pointer children[ maximumNumberOfChildrenToTry ];

  igstk::Transform childTransform;
  childTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  for( unsigned int birthId = 0; birthId < maximumNumberOfChildrenToTry; birthId++ )
    {
    ChildObjectType::Pointer newBorn = ChildObjectType::New();
    children[ birthId ] = newBorn;
  
    //
    // note that RequestAddChild will call internally
    // the RequestAttachToSpatialObjectParent() method
    //
    group->RequestAddChild( childTransform, newBorn );
  
    if( group->GetNumberOfChildren()  != birthId+1 )
      {
      std::cerr << "Error: number of children is not " << birthId+1 << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << "GetNumberOfChildren() Test PASSED " << std::endl;

  typedef igstk::GroupObjectTest::ChildObserver  ChildObserverType;

  ChildObserverType::Pointer childObserver = ChildObserverType::New();

  group->AddObserver( igstk::SpatialObjectModifiedEvent(), childObserver );
  group->AddObserver( igstk::SpatialObjectNotAvailableEvent(), childObserver );

  for( unsigned int childId = 0; 
       childId < maximumNumberOfChildrenToTry; childId++ )
    {
    group->RequestGetChild( childId );

    if( !childObserver->GotChild() )
      {
      std::cerr << "Error: problem getting Child " << childId << std::endl;
      return EXIT_FAILURE;
      }

    if( childObserver->GotChildNotAvailableMessage() )
      {
      std::cerr << "Error: problem getting Child, false negative ";
      std::cerr << childId << std::endl;
      return EXIT_FAILURE;
      }

    igstk::SpatialObject::ConstPointer receivedChild = childObserver->GetChild();

    if( receivedChild.GetPointer() != children[ childId ].GetPointer() )
      {
      std::cerr << "Error: wrong child has been returned " << std::endl;
      return EXIT_FAILURE;
      }
    }

  //
  // Now ask for a non existing child, verify that we don't get it,
  // and that we get the event indicating that it is not available.
  //
  group->RequestGetChild( maximumNumberOfChildrenToTry+10 );

  if( childObserver->GotChild() )
    {
    std::cerr << "Error: problem getting Child out of bounds" << std::endl;
    return EXIT_FAILURE;
    }

  if( !childObserver->GotChildNotAvailableMessage() )
    {
    std::cerr << "Error: failed to get ChildNotAvailableMessage";
    return EXIT_FAILURE;
    }

  std::cout << "Test [DONE]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
