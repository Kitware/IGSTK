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

  // Testing Update
  cylinderRepresentation->IsModified();

  // Testing again in order to exercise the other half of an if().
  cylinderRepresentation->IsModified();
  cylinderRepresentation->SetColor(0.3,0.7,0.2);
  if( !cylinderRepresentation->IsModified() )
    {
    std::cerr << "IsModified() failed to be true after a SetColor()" << std::endl;
    return EXIT_FAILURE;
    }

  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 2000.0;
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  transform.SetTranslationAndRotation( translation, rotation, validityTimeInMilliseconds );
  cylinderObject->SetTransform( transform );
  igstk::Transform  transform2 = cylinderObject->GetTransform();
  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" << std::endl;
      std::cerr << "input  translation = " << translation << std::endl;
      std::cerr << "output translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
      fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
      fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
      fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "Rotation component is out of range [FAILED]" << std::endl;
    std::cerr << "input  rotation = " << rotation << std::endl;
    std::cerr << "output rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }



  // Exercise Copy() method
  ObjectRepresentationType::Pointer cylinderRepresentation2 = cylinderRepresentation->Copy();
  scene->RequestAddObject( view2D, cylinderRepresentation2 );
  if(cylinderRepresentation2->GetOpacity() != cylinderRepresentation->GetOpacity())
    {
    std::cerr << "Copy() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetCylinderObject() with a null pointer as argument
  std::cout << "Testing RequestSetCylinderObject() with NULL argument: ";
  ObjectRepresentationType::Pointer cylinderRepresentation3 = ObjectRepresentationType::New();
  cylinderRepresentation3->RequestSetCylinderObject( 0 );

  // Exercise RequestSetCylinderObject() called twice. The second call should be ignored.
  std::cout << "Testing RequestSetCylinderObject() called twice: ";
  ObjectRepresentationType::Pointer cylinderRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer cylinderObjectA = ObjectType::New();
  ObjectType::Pointer cylinderObjectB = ObjectType::New();
  cylinderRepresentation4->RequestSetCylinderObject( cylinderObjectA );
  cylinderRepresentation4->RequestSetCylinderObject( cylinderObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  cylinderRepresentation->SetColor(0.9,0.7,0.1);
  cylinderRepresentation->SetOpacity(0.8);

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
