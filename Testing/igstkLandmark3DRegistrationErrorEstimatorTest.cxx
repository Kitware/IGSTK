/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimatorTest.cxx
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
#include "igstkLandmark3DRegistration.h"
#include "igstkLandmark3DRegistrationErrorEstimator.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMacro.h"

class Landmark3DRegistrationErrorEstimatorGetErrorCallback: public itk::Command
{
public:

  typedef Landmark3DRegistrationErrorEstimatorGetErrorCallback  Self;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::Command                                          Superclass;
  itkNewMacro(Self);

  typedef igstk::LandmarkRegistrationErrorEvent 
                                             LandmarkRegistrationErrorEventType;
  void Execute( const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {
    }
  
  void Execute( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
    {
    std::cout<< " LandmarkRegistrationErrorEvent is thrown" << std::endl;
                    
    const LandmarkRegistrationErrorEventType * errorEvent =
          dynamic_cast < const LandmarkRegistrationErrorEventType* > ( &event );
    m_Error = errorEvent->Get();
    m_EventReceived = true;
    } 
  
  bool GetEventReceived()
    {
    return m_EventReceived;
    }
  double GetError()
    {
    return m_Error;
    }  

protected:
  Landmark3DRegistrationErrorEstimatorGetErrorCallback()   
    {
    m_EventReceived = true;
    }

private:
  
  bool   m_EventReceived;
  double m_Error;
};

class Landmark3DRegistrationGetRMSErrorCallback: public itk::Command
{
public:
  typedef Landmark3DRegistrationGetRMSErrorCallback  Self;
  typedef itk::SmartPointer<Self>                    Pointer;
  typedef itk::Command                               Superclass;
  itkNewMacro(Self);

  typedef igstk::DoubleTypeEvent DoubleTypeEventType;

  void Execute( const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {
    }
  void Execute( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
    {
    std::cout<< " DoubleTypeEvent is thrown" << std::endl;
    const DoubleTypeEventType * errorEvent =
                  dynamic_cast < const DoubleTypeEventType* > ( &event );
    m_RMSError = errorEvent->Get();
    m_EventReceived = true;
    } 
  
  bool GetEventReceived()
    {
    return m_EventReceived;
    }
  
  igstk::Transform::ErrorType GetRMSError()
    {
    return m_RMSError;
    }  

protected:
  Landmark3DRegistrationGetRMSErrorCallback()   
    {
    m_EventReceived = true;
    }

private:
  bool m_EventReceived;
  igstk::Transform::ErrorType m_RMSError;
};

int igstkLandmark3DRegistrationErrorEstimatorTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();

  std::cout << "Testing igstk::Landmark3DRegistrationErrorEstimator" 
            << std::endl;

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
 
  typedef igstk::Landmark3DRegistration
                             Landmark3DRegistrationType;
  typedef igstk::Landmark3DRegistration::LandmarkPointContainerType
                              LandmarkPointContainerType;
  typedef igstk::Landmark3DRegistration::LandmarkImagePointType
                              LandmarkImagePointType;
  typedef igstk::Landmark3DRegistration::LandmarkTrackerPointType
                              LandmarkTrackerPointType;
  typedef Landmark3DRegistrationType::TransformType::OutputVectorType
                              OutputVectorType;

  Landmark3DRegistrationType::Pointer landmarkRegister = 
                                        Landmark3DRegistrationType::New();

  LandmarkPointContainerType  fpointcontainer;
  LandmarkPointContainerType  mpointcontainer;

  LandmarkImagePointType      fixedPoint;
  LandmarkTrackerPointType    movingPoint;

  // logger object
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

  landmarkRegister->SetLogger( logger );

  // Define the 3D rigid body transformation 
  typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;

  Rigid3DTransformType::Pointer   rigid3DTransform 
                                                  = Rigid3DTransformType::New();
  Rigid3DTransformType::MatrixType mrotation;
  Rigid3DTransformType::VersorType vrotation;

  mrotation.SetIdentity();

  // define rotation angle
  const double angle = 40.0 * atan( 1.0f ) / 45.0;
  const double sinth = sin( angle );
  const double costh = cos( angle );

  // Rotation around the Z axis
  mrotation[0][0] =  costh;
  mrotation[0][1] =  sinth;
  mrotation[1][0] = -sinth;
  mrotation[1][1] =  costh;

  vrotation.Set(mrotation);
  rigid3DTransform->SetRotation( vrotation );

  // Apply translation
  Rigid3DTransformType::TranslationType translation;
  translation.Fill( 10.0f );
  rigid3DTransform->SetTranslation(translation);

  // Add 1st landmark
  fixedPoint[0] =  25.0;
  fixedPoint[1] =  1.0;
  fixedPoint[2] =  15.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);


  // Add 2nd landmark
  fixedPoint[0] =  15.0;
  fixedPoint[1] =  21.0;
  fixedPoint[2] =  17.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  // Add 3d landmark
  fixedPoint[0] =  14.0;
  fixedPoint[1] =  25.0;
  fixedPoint[2] =  11.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  // Add 4th landmark
  fixedPoint[0] =  10.0;
  fixedPoint[1] =  11.0;
  fixedPoint[2] =  8.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  landmarkRegister->Print(std::cout);
   
  // Calculate transform
  landmarkRegister->RequestComputeTransform();

  //Estimate the error 
  typedef igstk::Landmark3DRegistrationErrorEstimator
                                 Landmark3DRegistrationErrorEstimatorType;
    
  typedef Landmark3DRegistrationErrorEstimatorType::TargetPointType   
                                                                TargetPointType;
  typedef Landmark3DRegistrationErrorEstimatorType::ErrorType   ErrorType;
     
  Landmark3DRegistrationErrorEstimatorType::Pointer 
                                     landmarkRegistrationErrorEstimator = 
                                Landmark3DRegistrationErrorEstimatorType::New();

  landmarkRegistrationErrorEstimator->SetLogger( logger );
  landmarkRegistrationErrorEstimator->RequestSetLandmarkContainer( 
                                                              fpointcontainer );
  // Compute the landmark registration error
  ErrorType                   landmarkRegistrationError;

  // Setup an obsever to get the RMS error 
  Landmark3DRegistrationGetRMSErrorCallback::Pointer lRmscb =
                            Landmark3DRegistrationGetRMSErrorCallback::New();

  landmarkRegister->AddObserver( igstk::DoubleTypeEvent(), lRmscb );
  landmarkRegister->RequestGetRMSError();

  if( !lRmscb->GetEventReceived() )
    {
    std::cerr << "LandmarkRegsistration class failed to throw a event "
              << "containing the RMS error " << std::endl;
    return EXIT_FAILURE;
    }
 
  landmarkRegistrationError = lRmscb->GetRMSError();
  landmarkRegistrationErrorEstimator->RequestSetLandmarkRegistrationError( 
                                                    landmarkRegistrationError );

  //Estimate target regsitration error 
  TargetPointType       targetPoint;
  targetPoint[0] =  10.0;
  targetPoint[1] =  20.0;
  targetPoint[2] =  8.0;
  landmarkRegistrationErrorEstimator->RequestSetTargetPoint( targetPoint );

  ErrorType       targetRegistrationError;
  landmarkRegistrationErrorEstimator->
                                  RequestEstimateTargetPointRegistrationError();
  
  // Setup an obsever to get the registration error 
  Landmark3DRegistrationErrorEstimatorGetErrorCallback::Pointer lrtcb =
                   Landmark3DRegistrationErrorEstimatorGetErrorCallback::New();
  landmarkRegistrationErrorEstimator->AddObserver(
                               igstk::LandmarkRegistrationErrorEvent(), lrtcb );
  landmarkRegistrationErrorEstimator->
                               RequestGetTargetPointRegistrationErrorEstimate();

  if( !lrtcb->GetEventReceived() )
    {
    std::cerr << "LandmarkRegsistrationErrorEstimator class failed "
              << "to throw a "
              << "landmark registration error event" << std::endl;
    return EXIT_FAILURE;
    }
        
  targetRegistrationError = lrtcb->GetError();
 
  std::cout << "Target registration error:" 
            << targetRegistrationError << std::endl;

  landmarkRegistrationErrorEstimator->Print(std::cout);
  
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
