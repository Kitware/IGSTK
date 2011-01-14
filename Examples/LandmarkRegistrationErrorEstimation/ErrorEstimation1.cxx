/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ErrorEstimation1.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
//
// This example illustrates how to estimate the registration error of a target
// point that has been registered using transformation parameters that were 
// computed using landmark-based registration.
// 
// The error estimation is based on the closed-form equation developed by West
// et al.~\cite{West2001}.  The target point registration error is
// dependent on the location of the target point, the registration error of the
// landmark points (RMS error), and the configuration of the
// landmark points.
//
// EndLatex


#include <iostream>

// BeginLatex
//
// To use the IGSTK component for computing the registration error, the 
// following  \doxygen{Landmark3DRegistrationErrorEstimator} header file must
// be added:
//
// EndLatex
#include "igstkLandmark3DRegistration.h"
// BeginCodeSnippet
#include "igstkLandmark3DRegistrationErrorEstimator.h"
// EndCodeSnippet
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMacro.h"

class ErrorEstimationGetErrorCallback: public itk::Command
{
public:
  typedef ErrorEstimationGetErrorCallback    Self;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::Command                       Superclass;

  itkNewMacro(Self);

  typedef igstk::LandmarkRegistrationErrorEvent 
                         LandmarkRegistrationErrorEventType;

  void Execute( const itk::Object *caller, const itk::EventObject & event )
    {
    }
  void Execute( itk::Object *caller, const itk::EventObject & event )
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
  ErrorEstimationGetErrorCallback()
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

  void Execute( const itk::Object *caller, const itk::EventObject & event )
    {
    }
  void Execute( itk::Object *caller, const itk::EventObject & event )
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


int main( int argv, char * argc[] )
{

  igstk::RealTimeClock::Initialize();

  std::cout << "Landmark3DRegistrationErrorEstimator" << std::endl;


  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

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
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  typedef igstk::VTKLoggerOutput LoggerOutputType;
  LoggerOutputType::Pointer vtkLoggerOutput = LoggerOutputType::New();
  vtkLoggerOutput->OverrideVTKWindow();
  // redirect messages from VTK OutputWindow -> logger
  vtkLoggerOutput->SetLogger(logger);


  landmarkRegister->SetLogger( logger );


  // Define the 3D rigid body transformation 
  typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;

  Rigid3DTransformType::Pointer rigid3DTransform = 
                                     Rigid3DTransformType::New();

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


  // Estimate the error 
  // BeginLatex
  //
  // The registration error estimator type is defined and an object is
  // instantiated, as follows:
  //
  // EndLatex


  // BeginCodeSnippet
  typedef igstk::Landmark3DRegistrationErrorEstimator   ErrorEstimatorType;
  // EndCodeSnippet


  typedef ErrorEstimatorType::TargetPointType   TargetPointType;
  typedef ErrorEstimatorType::ErrorType         ErrorType;

  // BeginCodeSnippet
  ErrorEstimatorType::Pointer errorEstimator = ErrorEstimatorType::New();
  // EndCodeSnippet

  errorEstimator->SetLogger( logger );

  // BeginLatex
  //
  // The landmark point container is set as follows:
  //
  // EndLatex

  // BeginCodeSnippet
  errorEstimator->RequestSetLandmarkContainer( fpointcontainer );
  // EndCodeSnippet
 


  // Compute the landmark registration error
  // BeginLatex
  //
  // Next, the landmark registration error is set. The landmark registration
  // component is used to compute this parameter. This error parameter
  // is basically the RMS error of the landmark registration. The commands are 
  // as follows:
  //
  // EndLatex

  ErrorType                   landmarkRegistrationError;

  // BeginCodeSnippet
  Landmark3DRegistrationGetRMSErrorCallback::Pointer lRmscb =
                            Landmark3DRegistrationGetRMSErrorCallback::New();

  landmarkRegister->AddObserver( igstk::DoubleTypeEvent(), lRmscb );
  landmarkRegister->RequestGetRMSError();

  landmarkRegistrationError = lRmscb->GetRMSError();

  errorEstimator->RequestSetLandmarkRegistrationError( 
                                                landmarkRegistrationError );
  // EndCodeSnippet

  // Estimate target registration error 
  // BeginLatex
  // Next, the target point, which we will use for estimating the registration
  // error, is set.
  //
  // EndLatex

  // BeginCodeSnippet
  TargetPointType targetPoint;
  targetPoint[0] =  10.0;
  targetPoint[1] =  20.0;
  targetPoint[2] =  8.0;
  errorEstimator->RequestSetTargetPoint( targetPoint );
  // EndCodeSnippet


  // BeginLatex
  //
  // Finally, the registration error for the target
  // point is estimated:
  //
  // EndLatex

  // BeginCodeSnippet
  ErrorType       targetRegistrationError;
  errorEstimator->RequestEstimateTargetPointRegistrationError( );
  // EndCodeSnippet


  // Setup an observer to get the registration error 
  //
  // BeginLatex
  //
  // To receive the error value, an observer is set up to listen to
  // an event loaded with error value as follows:
  //
  // EndLatex

  // BeginCodeSnippet
  ErrorEstimationGetErrorCallback::Pointer lrtcb =
                          ErrorEstimationGetErrorCallback::New();

  errorEstimator->AddObserver( igstk::LandmarkRegistrationErrorEvent(), lrtcb );
  errorEstimator->RequestGetTargetPointRegistrationErrorEstimate();

  if( !lrtcb->GetEventReceived() )
    {
    std::cerr << "LandmarkRegsistrationErrorEstimator class failed to "
              << "throw a landmark registration error event" << std::endl;
    return EXIT_FAILURE;
    }

  targetRegistrationError = lrtcb->GetError();
  // EndCodeSnippet
 
  std::cout << "Target registration error:";
  std::cout << targetRegistrationError << std::endl;

  errorEstimator->Print( std::cout );

 
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
