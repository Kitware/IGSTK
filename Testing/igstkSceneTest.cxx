/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneTest.cxx
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
#include "igstkScene.h"
#include "igstkView3D.h"
#include "igstkEllipsoidObjectRepresentation.h"

int igstkSceneTest( int, char * [] )
{
  typedef igstk::Scene  SceneType;
  SceneType::Pointer scene = SceneType::New();

  typedef igstk::EllipsoidObjectRepresentation  ObjectType;
  ObjectType::Pointer ellipsoid = ObjectType::New();

  igstk::View3D* view = new igstk::View3D(0,0,0,0);

  // Testing number of object
  std::cout << "Testing GetNumberOfObjects: ";
  if(scene->GetNumberOfObjects() != 0)
    { 
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Add Object
  std::cout << "Testing RequestAddObject: ";
  scene->RequestAddObject(view,ellipsoid); 
  if(scene->GetNumberOfObjects() != 1)
    { 
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Remove object
  std::cout << "Testing RequestRemoveObject: ";
  scene->RequestRemoveObject(view,ellipsoid); 
  if(scene->GetNumberOfObjects() != 0)
    { 
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing the igstkSpatialObject
  std::cout << "Testing SpatialObject: ";
  igstk::SpatialObject::Pointer object = igstk::SpatialObject::New();
  object->SetOffset(0,1,2);
  object->GetOffset();
  object->GetMatrix();
  object->GetNameOfClass();
  std::cout << "[PASSED]" << std::endl;

  // Testing Removing all object
  scene->RequestAddObject(view,ellipsoid); 
  std::cout << "Testing Removing all object: ";
  scene->RequestRemoveAllObjects(); 
  if(scene->GetNumberOfObjects() != 0)
    { 
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing printself
  scene->Print(std::cout);
  scene->GetNameOfClass();

  delete view;

  return EXIT_SUCCESS;
}


