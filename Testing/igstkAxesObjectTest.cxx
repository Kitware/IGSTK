/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAxesObjectTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetAxesObject( object );


#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkAxesObjectTest( int, char * [] )
{

  typedef igstk::AxesObject                ObjectType;
  typedef igstk::AxesObjectRepresentation  RepresentationType;

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
  // Test Set/Get() Parameters
  std::cout << "Testing Set/GetSize() : ";
  object->SetSize(10,20,30);

  if(object->GetSizeX() != 10.0)
    {
    std::cerr << "SizeX error : " << object->GetSizeX() 
              << " v.s " << 10.0 << std::endl; 
    return EXIT_FAILURE;
    }
   
  if(object->GetSizeY() != 20.0)
    {
    std::cerr << "SizeX error : " << object->GetSizeX()
              << " v.s " << 20.0 << std::endl; 
    return EXIT_FAILURE;
    }
 
  if(object->GetSizeZ() != 30.0)
    {
    std::cerr << "SizeX error : " << object->GetSizeX() 
              << " v.s " << 30.0 << std::endl; 
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
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  object->SetSize( 20.0, 30.0, 40.0 );
  
  // Note that this is done here only for the formality of the test.
  // In practice, this call will not actually change the colors of the
  // Axes object representation. See vtkAxesActor for an explanation.
  representation->SetColor(0.3,0.7,0.2);

  testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();
}
