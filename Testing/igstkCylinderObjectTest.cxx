/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCylinderObjectTest.cxx
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

#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkScene.h"
#include "igstkView2D.h"


int igstkCylinderObjectTest( int, char * [] )
{
  typedef igstk::CylinderObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer cylinderRepresentation = ObjectRepresentationType::New();

  typedef igstk::CylinderObject ObjectType;
  ObjectType::Pointer cylinderObject = ObjectType::New();

  cylinderRepresentation->RequestSetCylinderObject( cylinderObject );

  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  cylinderObject->SetRadius(1.0);
  double radius = cylinderObject->GetRadius();
 
  if(radius != 1.0)
    {
    std::cout << "Radius error : " << radius << " v.s 1.0" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Set/GetHeight()
  std::cout << "Testing Set/GetHeight() : ";
  cylinderObject->SetHeight(5.0);
  double height = cylinderObject->GetHeight();
 
  if(height != 5.0)
    {
    std::cout << "Height error : " << height<< " v.s 5.0" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  cylinderRepresentation->SetColor(0.1,0.2,0.3);
  cylinderRepresentation->SetOpacity(0.4);
  if(fabs(cylinderRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(cylinderRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(cylinderRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(cylinderRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  cylinderRepresentation->Print(std::cout);
  cylinderRepresentation->GetNameOfClass();
  cylinderObject->GetNameOfClass();
  cylinderObject->Print(std::cout);

  // Testing CreateActors()
  std::cout << "Testing actors : ";
  typedef igstk::Scene   SceneType;
  SceneType::Pointer scene = SceneType::New();

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  
  // this will indirectly call CreateActors() 
  scene->RequestAddObject( view2D, cylinderRepresentation );

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
