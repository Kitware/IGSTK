/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTest.cxx
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

#include "igstkCoordinateSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkRealTimeClock.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkTransformObserver.h"


int igstkCoordinateSystemTest( int, char * [] )
{
  int testResult = EXIT_SUCCESS;

  std::cout << "Running igstkCoordinateSystemTest " << std::endl;

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType                  LoggerType;
  typedef itk::StdStreamLogOutput                    LogOutputType;
  typedef igstk::TransformObserver                   ObserverType;

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::INFO );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput
                                             = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from
                                       // VTK OutputWindow -> logger

  typedef igstk::CoordinateSystem  ObjectType;
  ObjectType::Pointer coordinateSystemA = ObjectType::New();
  coordinateSystemA->SetLogger( logger );
  coordinateSystemA->SetName( "A" );

  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( coordinateSystemA );

  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  const double validityTimeInMilliseconds = 20000.0; // 20 seconds
  igstk::Transform transform1;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform1.SetTranslationAndRotation(
    translation, rotation, errorValue, validityTimeInMilliseconds );


  ObjectType::Pointer coordinateSystemRoot = ObjectType::New();
  coordinateSystemRoot->SetName( "Root" );

  coordinateSystemA->RequestSetTransformAndParent( transform1,
                                                   coordinateSystemRoot );

  coordinateSystemA->RequestComputeTransformTo(coordinateSystemRoot);
  /** Need to get transform from an observer */
  igstk::Transform  transform1b;
  if( coordSystemAObserver->GotTransform() )
    {
    ObserverType::PayloadType payload =
      coordSystemAObserver->GetTransformBetweenCoordinateSystems();

    transform1b = payload.GetTransform();
    std::cout << "Got payload with transfrom from ";
    std::cout << payload.GetSource()->GetName();
    std::cout << " to " << payload.GetDestination()->GetName() << std::endl;

    igstk::Transform::VectorType translation1b=transform1b.GetTranslation();

    for( unsigned int i=0; i<3; i++ )
      {
      if( fabs( translation1b[i]  - translation[i] ) > tolerance )
        {
        std::cerr << "Translation component is out of range [FAILED]"
                  << std::endl;
        std::cerr << "input  translation = " << translation << std::endl;
        std::cerr << "output translation = " << translation1b << std::endl;
        testResult = EXIT_FAILURE;
        }
      }

    igstk::Transform::VersorType rotation1b = transform1b.GetRotation();
    if( fabs( rotation1b.GetX() - rotation.GetX() ) > tolerance ||
        fabs( rotation1b.GetY() - rotation.GetY() ) > tolerance ||
        fabs( rotation1b.GetZ() - rotation.GetZ() ) > tolerance ||
        fabs( rotation1b.GetW() - rotation.GetW() ) > tolerance     )
      {
      std::cerr << "Rotation component is out of range [FAILED]"
                << std::endl;
      std::cerr << "input  rotation = " << rotation   << std::endl;
      std::cerr << "output rotation = " << rotation1b << std::endl;
      testResult = EXIT_FAILURE;
      }

    if (testResult == EXIT_SUCCESS)
      {
      std::cout << " [PASSED]" << std::endl;
      }
    } // GotTransform
  else
    {
    testResult = EXIT_FAILURE;
    std::cout << "No transform computed. [FAILED]" << std::endl;
    } // GotTransform failed

  ObjectType::Pointer coordinateSystemB = ObjectType::New();
  coordinateSystemB->SetName( "B" );
  coordinateSystemB->SetLogger( logger );

  ObserverType::Pointer coordSystemBObserver = ObserverType::New();
  coordSystemBObserver->ObserveTransformEventsFrom( coordinateSystemB );

  translation[0] = 3;
  translation[1] = 5;
  translation[2] = 7;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  errorValue = 0.01; // 10 microns

  igstk::Transform transform2;

  transform2.SetTranslationAndRotation(
    translation, rotation, errorValue, validityTimeInMilliseconds );

  coordinateSystemB->RequestSetTransformAndParent( transform2,
                                                   coordinateSystemA );

  // Verify the computation of the transform to the root
  igstk::Transform  transform3a  = igstk::Transform::TransformCompose(
                                                    transform1, transform2 );

  coordinateSystemB->RequestComputeTransformTo(coordinateSystemRoot);

  /** Need to get transform from an observer */
  igstk::Transform  transform3b;

  std::cout << "Testing Set/GetTransform(): ";

  if( coordSystemBObserver->GotTransform() )
    {
    transform3b = coordSystemBObserver->GetTransform();

    typedef igstk::Transform::VectorType TransformVectorType;

    TransformVectorType translation3a = transform3a.GetTranslation();
    TransformVectorType translation3b = transform3b.GetTranslation();

    for( unsigned int i=0; i<3; i++ )
      {
      if( fabs( translation3a[i]  - translation3b[i] ) > tolerance )
        {
        std::cerr << "Translation component is out of range [FAILED]"
                  << std::endl;
        std::cerr << "input  translation = " << translation3a << std::endl;
        std::cerr << "output translation = " << translation3b << std::endl;
        testResult = EXIT_FAILURE;
        }
      }

    igstk::Transform::VersorType rotation3a = transform3a.GetRotation();
    igstk::Transform::VersorType rotation3b = transform3b.GetRotation();

    if( fabs( rotation3a.GetX() - rotation3b.GetX() ) > tolerance ||
        fabs( rotation3a.GetY() - rotation3b.GetY() ) > tolerance ||
        fabs( rotation3a.GetZ() - rotation3b.GetZ() ) > tolerance ||
        fabs( rotation3a.GetW() - rotation3b.GetW() ) > tolerance     )
      {
      std::cerr << "Rotation component is out of range [FAILED]"
                << std::endl;
      std::cerr << "input  rotation = " << rotation3a << std::endl;
      std::cerr << "output rotation = " << rotation3b << std::endl;
      testResult = EXIT_FAILURE;
      }

    if (testResult == EXIT_SUCCESS)
      {
      std::cout << " [PASSED]" << std::endl;
      }
    } // GotTransform
  else
    {
    testResult = EXIT_FAILURE;
    std::cout << "No transform computed. [FAILED]" << std::endl;
    } // GotTransform failed

  ObserverType::Pointer coordSystemDisconnectedObserver = ObserverType::New();
  ObjectType::Pointer coordinateSystemDisconnected = ObjectType::New();
  coordinateSystemDisconnected->SetName( "Disconnected" );

  coordSystemDisconnectedObserver->ObserveTransformEventsFrom(
    coordinateSystemDisconnected );

  coordinateSystemDisconnected->RequestComputeTransformTo( coordinateSystemA );

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Test Failing due to error messages ";
    std::cout << " received in vtkLoggerOutput" << std::endl;
    testResult = EXIT_FAILURE;
    }

  return testResult;
}
