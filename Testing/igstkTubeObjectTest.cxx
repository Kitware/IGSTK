/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeObjectTest.cxx
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
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetTubeObject( object );


#include "igstkTubeObject.h"
#include "igstkTubeObjectRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkTubeObjectTest( int, char * [] )
{

  typedef igstk::TubeObject                ObjectType;
  typedef igstk::TubeObjectRepresentation  RepresentationType;

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
  typedef igstk::TubeObject::PointType      PointType;
  typedef igstk::TubeObject::PointListType  PointListType;
  PointType p1;
  p1.SetPosition(0,0,0);
  p1.SetRadius(2);
  object->AddPoint(p1);
  PointType p2;
  p2.SetPosition(10,10,10);
  p2.SetRadius(10);
  object->AddPoint(p2);

  std::cout << "Testing the GetPoints() method: ";
  PointListType points = object->GetPoints();
  if(points.size() != 2)
    {
    std::cout << "GetPoints error : " << points.size() << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetNumberOfPoints: ";
  unsigned int nPoints = object->GetNumberOfPoints();
  if(nPoints != 2)
    {
    std::cout << "GetNumberOfPoints error : " << nPoints << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetPoint: ";
  const PointType * pt = object->GetPoint(1);
  if(!pt)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }
  const PointType * ptout = object->GetPoint(10);
  if(ptout)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }

  if(pt->GetPosition()[0] != 10)
    {
    std::cout << "GetPoint error: Position is not valid" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  testHelper.TestRepresentationProperties();
  testHelper.ExercisePrintSelf();
  testHelper.TestTransform();
  testHelper.ExerciseDisplay();

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing set properties : ";
  representation->SetColor(0.9,0.7,0.1);
  representation->SetOpacity(0.8);

  std::cout << representation << std::endl;
  std::cout << object << std::endl;

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing Clear(): ";
  object->Clear();
  if(object->GetNumberOfPoints()>0)
    {
    std::cerr << "Clear() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;


  std::cout << "Test [DONE]" << std::endl;

  testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();
}
