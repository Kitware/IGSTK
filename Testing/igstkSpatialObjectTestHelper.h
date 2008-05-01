/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectTestHelper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSpatialObjectTestHelper_h
#define __igstkSpatialObjectTestHelper_h

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkConfigure.h"
#include "igstkView2D.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkDefaultWidget.h"
#include "igstkTransformObserver.h"


namespace igstk
{

template < class TObject, class TRepresentation>
class SpatialObjectTestHelper
{
public:

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
  typedef TObject                               ObjectType;
  typedef TRepresentation                       RepresentationType;
  typedef typename ObjectType::Pointer          ObjectPointer;
  typedef typename RepresentationType::Pointer  RepresentationPointer;
  typedef igstk::View2D                         View2DType;
  typedef igstk::DefaultWidget                  WidgetType;
  typedef igstk::TransformObserver              TransformObserverType;


public:

  /** Constructor */
  SpatialObjectTestHelper()
    {
    igstk::RealTimeClock::Initialize();

    this->m_Logger    = LoggerType::New();
    this->m_LogOutput = LogOutputType::New();

    // logger object created for logging mouse activities
    this->m_LogOutput->SetStream( std::cout );
    this->m_Logger->AddLogOutput( this->m_LogOutput );
    this->m_Logger->SetPriorityLevel( LoggerType::CRITICAL );

    // Create an igstk::VTKLoggerOutput and then test it.
    this->m_VtkLoggerOutput = igstk::VTKLoggerOutput::New();
    this->m_VtkLoggerOutput->OverrideVTKWindow();
    // redirect messages from VTK OutputWindow -> logger
    this->m_VtkLoggerOutput->SetLogger(this->m_Logger);

    this->m_Representation = RepresentationType::New();
    this->m_Representation->SetLogger( this->m_Logger );

    this->m_SpatialObject = ObjectType::New();
    this->m_SpatialObject->SetLogger( this->m_Logger );

    ConnectObjectToRepresentationMacro(
      this->m_SpatialObject, this->m_Representation );

    // Setup the Widget and View
    this->m_View   = View2DType::New();
    this->m_Widget = new WidgetType( 280, 280 );

    // End of the GUI creation

    this->m_View->SetLogger( this->m_Logger );

    this->m_TransformObserver = TransformObserverType::New();

    this->m_TestPassed = true;
    }

  ~SpatialObjectTestHelper()
    {
    delete this->m_Widget;
    }

  ObjectType * GetSpatialObject()
    {
    return this->m_SpatialObject;
    }

  RepresentationType * GetRepresentation()
    {
    return this->m_Representation;
    }

  void TestRepresentationProperties()
    {
    // Test Property
    std::cout << "Testing Property : ";
    this->m_Representation->SetColor(0.1,0.2,0.3);
    this->m_Representation->SetOpacity(0.4);
    if(fabs(this->m_Representation->GetRed()-0.1)>0.00001)
      {
      std::cerr << "GetRed() [FAILED]" << std::endl;
      this->m_TestPassed = false;
      }
    if(fabs(this->m_Representation->GetGreen()-0.2)>0.00001)
      {
      std::cerr << "GetGreen()[FAILED]" << std::endl;
      this->m_TestPassed = false;
      }
    if(fabs(this->m_Representation->GetBlue()-0.3)>0.00001)
      {
      std::cerr << "GetBlue() [FAILED]" << std::endl;
      this->m_TestPassed = false;
      }
    if(fabs(this->m_Representation->GetOpacity()-0.4)>0.00001)
      {
      std::cerr << "GetOpacity() [FAILED]" << std::endl;
      this->m_TestPassed = false;
      }
    std::cout << "[PASSED]" << std::endl;
    this->m_TestPassed = true;
    }


  void ExercisePrintSelf()
    {
    // Testing PrintSelf()
    this->m_Representation->Print(std::cout);
    this->m_SpatialObject->Print(std::cout);
    this->m_SpatialObject->GetNameOfClass();
    this->m_Representation->GetNameOfClass();
    }


  void TestTransform()
    {
    // Test GetTransform()
    std::cout << "Testing Set/GetTransform(): ";

    const double tolerance = 1e-8;
    double validityTimeInMilliseconds = 20000.0; // 20 seconds
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

    this->m_TransformObserver->ObserveTransformEventsFrom( 
      this->m_SpatialObject );

    this->m_TransformObserver->Clear();

    this->m_SpatialObject->RequestSetTransformAndParent(
      transform, this->m_View );

    this->m_SpatialObject->RequestGetTransformToParent();

    if( !this->m_TransformObserver->GotTransform() )
      {
      std::cerr << "The SpatialObject did not returned a Transform event"
                << std::endl;
      this->m_TestPassed = false;
      }

    igstk::Transform  transform2 = this->m_TransformObserver->GetTransform();

    igstk::Transform::VectorType translation2 = transform2.GetTranslation();
    for( unsigned int i=0; i<3; i++ )
      {
      if( fabs( translation2[i]  - translation[i] ) > tolerance )
        {
        std::cerr << "Translation component is out of range [FAILED]"
                  << std::endl;
        std::cerr << "input  translation = " << translation << std::endl;
        std::cerr << "output translation = " << translation2 << std::endl;
        this->m_TestPassed = false;
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
      this->m_TestPassed = false;
      }

    std::cout << "[PASSED]" << std::endl;
    }


  void ExerciseDisplay()
    {
    // testing actors
    std::cout << "Testing actors : ";

    this->m_Widget->RequestSetView( this->m_View );

    // this will indirectly call CreateActors()
    this->m_View->RequestAddObject( this->m_Representation );

    this->m_View->RequestResetCamera();
    this->m_View->SetRefreshRate( 30 );
    this->m_View->RequestStart();

    const unsigned int millisecondsWait = 10;

    // Do manual redraws
    for(unsigned int i=0; i<100; i++)
      {
      igstk::PulseGenerator::Sleep( millisecondsWait );
      igstk::PulseGenerator::CheckTimeouts();
      }

    std::cout << "[PASSED]" << std::endl;
    }


  void TestRepresentationCopy()
    {
    // Testing the Copy() function
    std::cout << "Testing Copy(): ";
    RepresentationPointer copy = this->m_Representation->Copy();
    if(copy->GetOpacity() != this->m_Representation->GetOpacity())
      {
      std::cerr << "[FAILED]" << std::endl;
      this->m_TestPassed = false;
      }
    std::cout << "[PASSED]" << std::endl;

    // Exercise RequestSet....Object() with a null pointer as argument
    std::cout << "Testing RequestSet....Object() with NULL argument: ";
    RepresentationPointer representation3 = RepresentationType::New();

    ConnectObjectToRepresentationMacro( 0, representation3 );

    // Exercise RequestSet....Object() called twice.
    // The second call should be ignored.
    std::cout << "Testing RequestSet....Object() called twice: ";
    RepresentationPointer representation4 = RepresentationType::New();
    ObjectPointer objectA = ObjectType::New();
    ObjectPointer objectB = ObjectType::New();

    ConnectObjectToRepresentationMacro( objectA, representation4 );
    ConnectObjectToRepresentationMacro( objectB, representation4 );

    std::cout << this->m_Representation << std::endl;
    std::cout << objectA << std::endl;
    }

  void ExerciseScreenShot()
    {
    std::string filename = "igstk";
    filename += this->m_SpatialObject->GetNameOfClass();
    filename += "TestScreenshot1.png";

    // Exercise the screenshot option with a valid filename
    this->m_View->RequestSaveScreenShot( filename );
    }

  int GetFinalTestStatus()
    {
    if( this->m_VtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
      {
      return EXIT_FAILURE;
      }
    else
      {
      if( this->m_TestPassed == true )
        {
        return EXIT_SUCCESS;
        }
      else
        {
        return EXIT_FAILURE;
        }
      }
    }


private:

  LoggerType::Pointer                  m_Logger;
  LogOutputType::Pointer               m_LogOutput;
  VTKLoggerOutput::Pointer             m_VtkLoggerOutput;
  ObjectPointer                        m_SpatialObject;
  RepresentationPointer                m_Representation;
  View2DType::Pointer                  m_View;
  WidgetType                         * m_Widget;
  TransformObserverType::Pointer       m_TransformObserver;
  bool                                 m_TestPassed;

};

} // end namespace igstk

#endif // __igstkSpatialObjectTestHelper_h
