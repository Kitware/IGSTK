/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionObjectTest.cxx
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
  representation->RequestSetToolProjectionObject( object );


#include "igstkToolProjectionObject.h"
#include "igstkToolProjectionRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkToolProjectionObjectTest( int, char * [] )
{

  typedef igstk::ToolProjectionObject                ObjectType;
  typedef igstk::ToolProjectionRepresentation  RepresentationType;

  ObjectType::Pointer ToolProjectionObject = ObjectType::New();

  typedef igstk::SpatialObjectTestHelper<
    ObjectType, RepresentationType > TestHelperType;

  //
  // The helper constructor intializes all the elements needed for the test.
  //
  TestHelperType  testHelper;

  ObjectType         * object         = testHelper.GetSpatialObject();
  RepresentationType * representation = testHelper.GetRepresentation();

  if( !object )
    {
    std::cerr << "Failure to get object with GetSpatialObject()" << std::endl;
    return EXIT_FAILURE;
    }

  if( !representation )
    {
    std::cerr << "Failure to get representation with GetRepresentation()" << std::endl;
    return EXIT_FAILURE;
    }

  //
  //  Tests that are specific to this type of SpatialObject
  //
  //

  testHelper.TestRepresentationProperties();
  testHelper.ExercisePrintSelf();
  testHelper.TestTransform();
  testHelper.ExerciseDisplay();

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();
}
