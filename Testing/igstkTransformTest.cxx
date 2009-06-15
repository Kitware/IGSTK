/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include "igstkTransform.h"
#include "igstkPulseGenerator.h"


int igstkTransformTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();


  try
    {
    typedef igstk::Transform            TransformType;
    typedef TransformType::VectorType   VectorType;
    typedef TransformType::VersorType   VersorType;
    typedef TransformType::PointType    PointType;

    TransformType t1;
    
    // Test the SetToIdentity() method
    const double identityValidityPeriod = 100.0; // milliseconds
    t1.SetToIdentity( identityValidityPeriod );

    // Exercise the IsIdentity() method
    if( ! t1.IsIdentity() )
      {
      std::cerr << "SetIdentity()/IsIdentity() pair failed" << std::endl;
      return EXIT_FAILURE;
      }

    VectorType translation;
    translation[0] = 10.0;
    translation[1] = 20.0;
    translation[2] = 30.0;

    VersorType rotation;
    rotation.Set(0.0, 0.0, 1.0, 0.0); // 90 degrees around Z

    const double validityPeriod = 100.0; // milliseconds
    
    igstk::Transform::ErrorType errorValue = 0.01; // 10 microns
  
    const double tolerance = 1e-7;

    t1.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityPeriod );

    if( !t1.IsValidNow() )
    {
      std::cerr << "Error in IsValidNow() test" << std::endl;
      std::cerr << "Expected to be valid, but returned invalid" << std::endl;
      return EXIT_FAILURE;
    }

    VectorType translationSet = t1.GetTranslation();
    VersorType rotationSet    = t1.GetRotation();


    std::cout << t1 << std::endl;
    std::cout << translationSet << std::endl;
    std::cout << rotationSet << std::endl;

    double timeToCheck = t1.GetStartTime() + validityPeriod / 2.0;

    bool validAtTime = t1.IsValidAtTime( timeToCheck );
    if( !validAtTime )
      {
      std::cerr << "Error in IsValid() test" << std::endl;
      std::cerr << "Expected to be valid, but returned invalid" << std::endl;
      return EXIT_FAILURE;
      }


    for( unsigned int k=0; k < 3; k++ )
      {
      if( vnl_math_abs( translationSet[k] - translation[k] ) > tolerance )
        {
        std::cerr << "Pair:SetTranslationAndRotation():GetTranslation() failed"
                  << std::endl;

        return EXIT_FAILURE;
        }
      } 

    if( ( vnl_math_abs( rotationSet.GetX() - rotation.GetX() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetY() - rotation.GetY() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetZ() - rotation.GetZ() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetW() - rotation.GetW() ) > tolerance )  )
      {
      std::cerr << "Pair : SetTranslationAndRotation() : GetRotation() failed " 
                << std::endl;
      return EXIT_FAILURE;
      }


    t1.SetRotation( rotation, errorValue, validityPeriod );

    translationSet = t1.GetTranslation();
    rotationSet    = t1.GetRotation();

    for( unsigned int k=0; k < 3; k++ )
      {
      // SetRotation() must set the Translation to identity.
      if( vnl_math_abs( translationSet[k] - 0.0 ) > tolerance )
        {
        std::cerr << "Pair : SetRotation() : GetTranslation() failed " 
                  << std::endl;
        return EXIT_FAILURE;
        }
      } 

    if( ( vnl_math_abs( rotationSet.GetX() - rotation.GetX() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetY() - rotation.GetY() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetZ() - rotation.GetZ() ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetW() - rotation.GetW() ) > tolerance )  )
      {
      std::cerr << "Pair : SetRotation() : GetRotation() failed " << std::endl;
      return EXIT_FAILURE;
      }

    t1.SetTranslation( translation,  errorValue, validityPeriod );

    translationSet = t1.GetTranslation();
    rotationSet    = t1.GetRotation();

    for( unsigned int k=0; k < 3; k++ )
      {
      if( vnl_math_abs( translationSet[k] - translation[k] ) > tolerance )
        {
        std::cerr << "Pair : SetTranslation() : GetTranslation() failed " 
                  << std::endl;
        return EXIT_FAILURE;
        }
      } 


    if( ( vnl_math_abs( rotationSet.GetX() - 0.0 ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetY() - 0.0 ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetZ() - 0.0 ) > tolerance ) ||
        ( vnl_math_abs( rotationSet.GetW() - 1.0 ) > tolerance )    )
      {
      std::cerr << "Pair : SetTranslation() : GetRotation() failed " 
                << std::endl;
      return EXIT_FAILURE;
      }


    double timeExpired = t1.GetExpirationTime() + 10.0;

    bool invalidAtTime = t1.IsValidAtTime( timeExpired );
    if( invalidAtTime )
      {
      std::cerr << "Error in IsValid() test" << std::endl;
      std::cerr << "Expected to be invalid, but returned valid" << std::endl;
      return EXIT_FAILURE;
      }

    std::cout << "Testing operator == and operator != " << std::endl;
    bool shouldBeTrue  = ( t1 == t1 );
    bool shouldBeFalse = ( t1 != t1 );

    if( shouldBeTrue == false )
      {
      std::cerr << "Error in operator==" << std::endl;
      return EXIT_FAILURE;
      }

    if( shouldBeFalse == true )
      {
      std::cerr << "Error in operator!=" << std::endl;
      return EXIT_FAILURE;
      }

    // use a non-orthogonal rotation
    VersorType::VectorType axis;
    axis[0] =  1;
    axis[1] =  6;
    axis[2] = -5;
    double angle = 10.0;
    rotation.Set(axis, angle);

    translation[0] = -1000.0;
    translation[1] = 5.0;
    translation[2] = 9.0;

    t1.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityPeriod );

    std::cout << "Testing the inverse" << std::endl;
    igstk::Transform tinv = t1.GetInverse();

    std::cout << "Direct  = " << t1   << std::endl;
    std::cout << "Inverse = " << tinv << std::endl;

    // multiply to test inverse
    t1 = igstk::Transform::TransformCompose( tinv, t1 );

    std::cout << "Direct * Inverse = " << t1 << std::endl;

    translation = t1.GetTranslation();
    rotation    = t1.GetRotation();
    // verify that result to within 1 bit of double precision
    const double epsilon = 2.3e-16;
    if (fabs(translation[0]) > 1000*epsilon ||
        fabs(translation[1]) > 1000*epsilon ||
        fabs(translation[2]) > 1000*epsilon ||
        fabs(rotation.GetX()) > epsilon ||
        fabs(rotation.GetY()) > epsilon ||
        fabs(rotation.GetZ()) > epsilon ||
        fabs(rotation.GetW() - 1) > epsilon)
      {
      std::cerr << "Error in inverse" << std::endl;
      return EXIT_FAILURE;
      }

    // Testing the IsNumericallyEquivalent() method

    rotation.Set(axis, angle);

    translation[0] = -1000.0;
    translation[1] = 5.0;
    translation[2] = 9.0;
    t1.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityPeriod );

    igstk::Transform tt  = t1;
    igstk::Transform ti  = tt.GetInverse();
    igstk::Transform tii = ti.GetInverse();

    const double inverseTolerance = 1e-12;
    if( ! tt.IsNumericallyEquivalent( tii, inverseTolerance ) )
      {
      std::cerr << "Error in GetInverse()^2/IsNumericallyEquivalent() pair";
      std::cerr << std::endl;
      return EXIT_FAILURE;
      }

    if( tt.IsNumericallyEquivalent( ti ) )
      {
      std::cerr << "Error in GetInverse()/IsNumericallyEquivalent() pair";
      std::cerr << std::endl;
      return EXIT_FAILURE;
      }


    translation[0] = 0.0;
    translation[1] = 0.0;
    translation[2] = 0.0;
    rotation.Set(0.0, 0.0, 0.0, 1.0);

    double  smallerError  = 10;
    double  lagerError    = 100;
    double  shorterPeriod = 500;
    double  longerPeriod  = 1000;
    
    t1.SetTranslationAndRotation( translation, rotation, smallerError, 
                                                                shorterPeriod );

    tinv.SetTranslationAndRotation( translation, rotation, lagerError, 
                                                                longerPeriod );
    igstk::PulseGenerator::Sleep(250);

    std::cout << "Composing two transforms" << std::endl;
    std::cout << "Transform t1 = " << t1 << std::endl;
    std::cout << "Transform t2 = " << tinv << std::endl;

    t1 = igstk::Transform::TransformCompose( t1, tinv );

    std::cout << "Composed Transform  = " << t1 << std::endl;

    std::cout << "New transform = \n" << t1 << std::endl;
    if ( t1.GetError() != lagerError+smallerError )
      {
      std::cout << "Error computing transform value" << std::endl;
      std::cout << t1.GetError() << std::endl;
      return EXIT_FAILURE;
      }
    std::cout<< "Valid period (roughly 250) = " << 
                       t1.GetExpirationTime() - t1.GetStartTime() << std::endl;
   

    }
  catch(...)
    {
    std::cerr << "Exception catched !!!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED ! " << std::endl;

  return EXIT_SUCCESS;
}
