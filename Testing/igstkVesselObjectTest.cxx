/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVesselObjectTest.cxx
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
  representation->RequestSetVesselObject( object );


#include "igstkVesselObject.h"
#include "igstkVesselObjectRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkVesselObjectTest( int, char * [] )
{

  typedef igstk::VesselObject                ObjectType;
  typedef igstk::VesselObjectRepresentation  RepresentationType;

  ObjectType::Pointer VesselObject = ObjectType::New();

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
  typedef ObjectType::PointType   TubePointType;
  TubePointType p1;
  p1.SetPosition(0,0,0);
  p1.SetRadius(2);
  VesselObject->AddPoint(p1);
  TubePointType p2;
  p2.SetPosition(10,10,10);
  p2.SetRadius(10);
  VesselObject->AddPoint(p2);

  std::cout << "Testing the GetPoints() method: ";
  ObjectType::PointListType points = VesselObject->GetPoints();
  if(points.size() != 2)
    {
    std::cout << "GetPoints error : " << points.size() << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetNumberOfPoints: ";
  unsigned int nPoints = VesselObject->GetNumberOfPoints();
  if(nPoints != 2)
    {
    std::cout << "GetNumberOfPoints error : " << nPoints << " v.s 2" 
              << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing GetPoint: ";
  const TubePointType* pt = VesselObject->GetPoint(1);
  if(!pt)
    {
    std::cout << "GetPoint error" << std::endl; 
    return EXIT_FAILURE;
    }
  const TubePointType* ptout = VesselObject->GetPoint(10);
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

  std::cout << "Testing Clear(): ";
  VesselObject->Clear();
  if(VesselObject->GetNumberOfPoints()>0)
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
