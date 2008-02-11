/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEllipsoidObjectTest.cxx
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

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetEllipsoidObject( object );


#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkEllipsoidObjectTest( int, char * [] )
{

  typedef igstk::EllipsoidObject                ObjectType;
  typedef igstk::EllipsoidObjectRepresentation  RepresentationType;

  typedef igstk::SpatialObjectTestHelper<
    ObjectType, RepresentationType > TestHelperType;

  //
  // The helper constructor intializes all the elements needed for the test.
  //
  TestHelperType  testHelper;

  ObjectType         * object         = testHelper.GetSpatialObject();
  RepresentationType * representation = testHelper.GetRepresentation();

  //
  //  Tests that are specific to this type of SpatialObject
  //
  //
  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  ObjectType::ArrayType radius;
  radius[0] = 1;
  radius[1] = 2;
  radius[2] = 3;
  object->SetRadius(radius);

  igstk::EllipsoidObject::ArrayType radiusRead = object->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radiusRead[i] != radius[i])
      {
      std::cerr << "Radius error : " << radius[i] 
                << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }

  object->SetRadius(2,3,4);
 
  radiusRead = object->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radiusRead[i] != radius[i]+1)
      {
      std::cerr << "Radius error : " << radius[i] 
                << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }
  std::cout << "[PASSED]" << std::endl;


  testHelper.TestRepresentationProperties();
  testHelper.ExercisePrintSelf();
  testHelper.TestTransform();
  testHelper.ExerciseDisplay();

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  object->SetRadius( 20.0, 37.0, 39.0 );
  
  representation->SetColor(0.3,0.7,0.2);

  testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();
}
