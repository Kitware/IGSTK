/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkLandmarkRegistrationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmarkRegistration.h"

int igstkLandmarkRegistrationTest( int argv, char * argc[] )
{
    std::cout << "Testing igstk::LandmarkRegistration" << std::endl;

    typedef igstk::LandmarkRegistration<3>                                 LandmarkRegistrationType;
    typedef igstk::LandmarkRegistration<3>::LandmarkPointContainerType     LandmarkPointContainerType;
    typedef igstk::LandmarkRegistration<3>::LandmarkPointType              LandmarkPointType;

    LandmarkRegistrationType::Pointer landmarkRegister = LandmarkRegistrationType::New();    
    LandmarkPointContainerType  fpointcontainer;
    LandmarkPointContainerType  mpointcontainer;
    LandmarkPointType fixedPoint;
    LandmarkPointType movingPoint;

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

    Rigid3DTransformType::OffsetType ioffset;
    ioffset.Fill( 10.0f );

    rigid3DTransform->SetOffset( ioffset );

    fixedPoint[0] =  50.0;
    fixedPoint[1] =  1.0;
    fixedPoint[2] =  0.0;

    fpointcontainer.push_back(fixedPoint);
  
    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    fixedPoint[0] =  50.0;
    fixedPoint[1] =  2.0;
    fixedPoint[2] =  0.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);


    fixedPoint[0] =  50.0;
    fixedPoint[1] =  3.0;
    fixedPoint[2] =  0.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    fixedPoint[0] =  50.0;
    fixedPoint[1] =  4.0;
    fixedPoint[2] =  0.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    //Set the fixed and moving coordinates

    landmarkRegister->SetTrackerLandmarks(fpointcontainer);
    landmarkRegister->SetTrackerImageLandmarks(mpointcontainer);

    //Print out the two sets of coordinates 
    landmarkRegister->Print(std::cout);

    //Determine the transformation parameters
    landmarkRegister->EvaluateTransform();

    landmarkRegister->GetTransform()->DebugOn();

    //Check if the transformation parameters were evaluated correctely
    LandmarkRegistrationType::PointsContainerConstIterator
      fitr = fpointcontainer.begin();
    LandmarkRegistrationType::PointsContainerConstIterator
      mitr = mpointcontainer.begin();

    typedef LandmarkRegistrationType::TransformType::OutputVectorType  OutputVectorType;
    OutputVectorType error;
    OutputVectorType::RealValueType tolerance = 0.1;
    bool failed = false;

    while( mitr != mpointcontainer.end() )
      {
      std::cout << "  Tracker Landmark: " << *fitr << " TrackerImage landmark " << *mitr
        << " Transformed trackerLandmark : " <<
        landmarkRegister->GetTransform()->TransformPoint( *fitr ) << std::endl;

      error = *mitr - landmarkRegister->GetTransform()->TransformPoint( *fitr);
      if( error.GetNorm() > tolerance )
        {
        failed = true;
        }
      ++mitr;
      ++fitr;
      }

     if( failed )
      {
      // Hang heads in shame
      std::cout << "  Tracker landmarks transformed by the transform did not match closely "
        << " enough with the moving landmarks.  The transform computed was: ";
      landmarkRegister->GetTransform()->Print(std::cout);
      std::cout << "  [FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" << std::endl;
      }

    return EXIT_SUCCESS;
}


