/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ErrorEstimation2.cxx
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


int main( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  if ( argc < 4 )
    {
    std::cerr << "Missing command line arguments: " 
              << argv[0] << "\tImageLandmark_input_file\t"
              << "TrackerLandmark_input_file\t"
              << "TargetPoints_input_file\t" << std::endl;
    return EXIT_FAILURE;
    }


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


  std::ifstream inputImageLandmarkFileStream; 
  std::string imageLandmarkFile = argv[1];
  std::cout << "Reading image landmarks point file: " << imageLandmarkFile << std::endl;
  inputImageLandmarkFileStream.open( imageLandmarkFile.c_str() );
  if( inputImageLandmarkFileStream.fail() )
    {
    std::cerr << "Encountered a problem opening: " << imageLandmarkFile << std::endl;
    return EXIT_FAILURE;
    }
 
  const int stringMaximumSize = 4096;
  char parametersString[stringMaximumSize];
  double x;
  double y;
  double z; 

  while( !inputImageLandmarkFileStream.getline( parametersString, stringMaximumSize).eof()  )
    {
    if ( sscanf( parametersString, "%lf\t%lf\t%lf", &x, &y, &z) == 3 ) 
        {
        fixedPoint[0] =  x;
        fixedPoint[1] =  y;
        fixedPoint[2] =  z;
        fpointcontainer.push_back(fixedPoint);
        std::cout << "Read in image landmark: " << fixedPoint << std::endl;
        }
    }

  std::ifstream inputTrackerLandmarkFileStream; 
  std::string trackerLandmarkFile = argv[2];
  std::cout << "Reading tracker landmarks point file: " << imageLandmarkFile << std::endl;
  inputTrackerLandmarkFileStream.open( trackerLandmarkFile.c_str() );
  if( inputTrackerLandmarkFileStream.fail() )
    {
    std::cerr << "Encountered a problem opening: " << trackerLandmarkFile << std::endl;
    return EXIT_FAILURE;
    }
  while( ! inputTrackerLandmarkFileStream.getline( parametersString, stringMaximumSize).eof()  )
    {
    if ( sscanf( parametersString, "%lf\t%lf\t%lf", &x, &y, &z) == 3 ) 
        {
        movingPoint[0] =  x;
        movingPoint[1] =  y;
        movingPoint[2] =  z;
        mpointcontainer.push_back(movingPoint);
        std::cout << "Read in tracker landmark: " << movingPoint << std::endl;
        }
    }

  typedef LandmarkPointContainerType::const_iterator PointsContainerConstIterator;
  PointsContainerConstIterator
      fitr = fpointcontainer.begin();
  PointsContainerConstIterator
      mitr = mpointcontainer.begin();

  while( fitr != fpointcontainer.end())
    {
    landmarkRegister->RequestAddImageLandmarkPoint( *fitr ); 
    landmarkRegister->RequestAddTrackerLandmarkPoint( *mitr );
    ++fitr; 
    ++mitr;
    }
 
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

  std::ifstream inputTargetLandmarkFileStream; 
  std::string targetLandmarkFile = argv[3];
  std::cout << "Reading target landmarks point file: " << targetLandmarkFile << std::endl;
  inputTargetLandmarkFileStream.open( targetLandmarkFile.c_str() );
  if( inputTargetLandmarkFileStream.fail() )
    {
    std::cerr << "Encountered a problem opening: " << targetLandmarkFile << std::endl;
    return EXIT_FAILURE;
    }
 
   while( ! inputTargetLandmarkFileStream.getline( parametersString, stringMaximumSize).eof()  )
    {
    LandmarkImagePointType targetPoint;

    if ( sscanf( parametersString, "%lf\t%lf\t%lf", &x, &y, &z) == 3 ) 
        {
        targetPoint[0] =  x;
        targetPoint[1] =  y;
        targetPoint[2] =  z;
        }

    std::cout<< "Compute TRE for :"<< targetPoint << std::endl;

    errorEstimator->RequestSetTargetPoint( targetPoint );

    ErrorType       targetRegistrationError;
    errorEstimator->RequestEstimateTargetPointRegistrationError( );
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
   
    std::cout << "Target registration error:";
    std::cout << targetRegistrationError << std::endl;

    }

 
  errorEstimator->Print( std::cout );
  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
