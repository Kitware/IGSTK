/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimatorTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLandmark3DRegistrationErrorEstimator.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMacro.h"

int igstkLandmark3DRegistrationErrorEstimatorTest( int argv, char * argc[] )
{
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
    landmarkRegister->SetLogger( logger );


    // Define the 3D rigid body transformation 
    typedef itk::Rigid3DTransform< double > Rigid3DTransformType;

    Rigid3DTransformType::Pointer   rigid3DTransform = Rigid3DTransformType::New();

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


    //Estimate the error 
    typedef igstk::Landmark3DRegistrationErrorEstimator     
                              Landmark3DRegistrationErrorEstimatorType;
    typedef igstk::Landmark3DRegistrationErrorEstimator::LandmarkPointContainerType
                              LandmarkPointContainerType;
    typedef igstk::Landmark3DRegistrationErrorEstimator::LandmarkPointType 
                              LandmarkPointType;
    typedef igstk::Landmark3DRegistrationErrorEstimator::TargetPointType   TargetPointType;
    typedef igstk::Landmark3DRegistrationErrorEstimator::ErrorType         ErrorType;
     
    Landmark3DRegistrationErrorEstimatorType::Pointer landmarkRegistrationErrorEstimator = 
                                        Landmark3DRegistrationErrorEstimatorType::New();    

    landmarkRegistrationErrorEstimator->SetLogger( logger );
    landmarkRegistrationErrorEstimator->SetImageLandmarks( fpointcontainer );

    // Compute the landmark registration error
    ErrorType                   landmarkRegistrationError;
    landmarkRegistrationError = landmarkRegister->ComputeRMSError();
    landmarkRegistrationErrorEstimator->SetLandmarkRegistrationError( landmarkRegistrationError );

    //Compute all error parameters necessary for target registration error estimation
    landmarkRegistrationErrorEstimator->ComputeErrorParameters();

    //Estimate target regsitration error 
    TargetPointType       targetPoint;
    targetPoint[0] =  10.0;
    targetPoint[1] =  20.0;
    targetPoint[2] =  8.0;

    ErrorType       targetRegistrationError;
    targetRegistrationError = landmarkRegistrationErrorEstimator->EstimateTargetRegistrationError( targetPoint );
    std::cout << "Target registration error:" << targetRegistrationError << std::endl;

    landmarkRegistrationErrorEstimator->Print(std::cout);
    return EXIT_SUCCESS;
}


