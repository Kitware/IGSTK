/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ErrorEstimation1.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
//
// This example illustrates how to estimate registration error of a target
// point when registered using transformation parameters computed using
// landmark-based registration technique.
// 
// The error estimation is based on the closed form equation developed by West
// et al.  In this closed form solution, the target point registration error is
// dependent on the location of the target point, the registration error of the
// landmark points ( root mean square error) and the configuration of the
// landmark points.
//
// EndLatex



#include <iostream>

// BeginLatex
//
// To use the IGSTK component for registration error computation, 
// \doxygen{Landmark3DRegistrationErrorEstimator} header file needs
// to be added.
//
// EndLatex
#include "igstkLandmark3DRegistration.h"
// BeginCodeSnippet
#include "igstkLandmark3DRegistrationErrorEstimator.h"
// EndCodeSnippet
#include "igstkVTKLoggerOutput.h"
#include "itkLogger.h"
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
    };
  private:
      bool m_EventReceived;
      double m_Error;
};

int main( int argv, char * argc[] )
{

  igstk::RealTimeClock::Initialize();

    std::cout << "Testing igstk::Landmark3DRegistrationErrorEstimator" << std::endl;

  
    typedef itk::Logger                   LoggerType;
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
    typedef igstk::VTKLoggerOutput LoggerOutputType;
    LoggerOutputType::Pointer vtkLoggerOutput = LoggerOutputType::New();
    vtkLoggerOutput->OverrideVTKWindow();
    // redirect messages from VTK OutputWindow -> logger
    vtkLoggerOutput->SetLogger(logger);  


    landmarkRegister->SetLogger( logger );


    // Define the 3D rigid body transformation 
    typedef itk::Rigid3DTransform< double > Rigid3DTransformType;

    Rigid3DTransformType::Pointer rigid3DTransform = 
                                       Rigid3DTransformType::New();

    Rigid3DTransformType::MatrixType mrotation;

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

    rigid3DTransform->SetRotationMatrix( mrotation );

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
    // instantiated.
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
    // The landmark point container is set as follows
    //
    // EndLatex

    // BeginCodeSnippet
    errorEstimator->RequestSetLandmarkContainer( fpointcontainer );
    // EndCodeSnippet
   


    // Compute the landmark registration error
    // BeginLatex
    //
    // Next, the landmark regisration error is set. The landmark registration
    // component is used to compute this parametr. This error paramete
    // basically the root mean square error of the landmark registration.
    //
    // EndLatex
    
    ErrorType                   landmarkRegistrationError;

    // BeginCodeSnippet
    landmarkRegistrationError = landmarkRegister->ComputeRMSError();
    errorEstimator->RequestSetLandmarkRegistrationError( 
                                                  landmarkRegistrationError );
    // EndCodeSnippet
    


    // BeginLatex
    //
    // Next, the other error parameters needed in the error estimation are
    // computed.  This is done by invoking ComputeErrorParameter() method as
    // shown below. 
    //
    // EndLatex
    
    // BeginCodeSnippet
    errorEstimator->RequestComputeErrorParameters();
    // EndCodeSnippet

    // Estimate target regsitration error 
    // BeginLatex
    // Next, the target point that we will be estimating the registration error
    // is set
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
    // Finally, we are ready to estimate the registration error for the target
    // point.
    //
    // EndLatex
    
    // BeginCodeSnippet
    ErrorType       targetRegistrationError;
    errorEstimator->RequestEstimateTargetPointRegistrationError( );
    // EndCodeSnippet
    


    // Setup an obsever to get the registration error 
    //
    // BeginLatex
    //
    // To receive the error value, we have to set up an observer to listen to
    // an event loaded with error value as follows
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


