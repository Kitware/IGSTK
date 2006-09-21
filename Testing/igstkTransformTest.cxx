/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include "igstkTransform.h"

// includes for Sleep
#if defined (_WIN32) || defined (WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

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

    VectorType translation;
    translation[0] = 10.0;
    translation[1] = 20.0;
    translation[2] = 30.0;

    VersorType rotation;
    rotation.Set(0.0, 0.0, 1.0, 0.0); // 90 degrees around Z

    const double validityPeriod = 10.0; // milliseconds
    
    igstk::Transform::ErrorType errorValue = 0.01; // 10 microns
  
    t1.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityPeriod );

    t1.SetRotation( rotation, errorValue, validityPeriod );

    t1.SetTranslation( translation,  errorValue, validityPeriod );


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
    
#if defined (_WIN32) || defined (WIN32)
    ::Sleep( 250 );            // Windows Sleep uses miliseconds
#else
    usleep( 250 * 1000 );  // linux usleep uses microsecond
#endif

    t1 = igstk::Transform::TransformCompose( t1, tinv );

    std::cout<< "New transform = \n" << t1 << std::endl;
    if ( t1.GetError() != lagerError )
      {
      std::cout<< "Error computing transform value" << std::endl;
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
