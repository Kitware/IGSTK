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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkTransform.h"

int igstkTransformTest( int, char * [] )
{

  try
    {
    typedef igstk::Transform    TransformType;
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

    PointType  center;
    center[0] = 0.0;
    center[1] = 0.0;
    center[2] = 0.0;

    const double validityPeriod = 10.0; // milliseconds
    
    igstk::Transform::ErrorType errorValue = 0.01; // 10 microns
  
    t1.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityPeriod );

    t1.SetRotation( rotation, errorValue, validityPeriod );

    t1.SetTranslation( translation,  errorValue, validityPeriod );

    t1.SetCenter( center, errorValue, validityPeriod );

    VectorType translationSet = t1.GetTranslation();
    VersorType rotationSet    = t1.GetRotation();
    PointType  centerSet      = t1.GetCenter();

    std::cout << t1 << std::endl;
    std::cout << translationSet << std::endl;
    std::cout << rotationSet << std::endl;
    std::cout << centerSet << std::endl;

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

    }
  catch(...)
    {
    std::cerr << "Exception catched !!!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED ! " << std::endl;

  return EXIT_SUCCESS;
}


