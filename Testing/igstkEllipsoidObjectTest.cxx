/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEllipsoidObjectTest.cxx
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

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkScene.h"
#include "igstkView2D.h"

int igstkEllipsoidObjectTest( int, char * [] )
{
  typedef igstk::EllipsoidObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer ellipsoidRepresentation = ObjectRepresentationType::New();

  typedef igstk::EllipsoidObject  ObjectType;
  ObjectType::Pointer ellipsoidObject = ObjectType::New();
    
  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  ObjectType::ArrayType radius;
  radius[0] = 1;
  radius[1] = 2;
  radius[2] = 3;
  ellipsoidObject->SetRadius(radius);

  igstk::EllipsoidObject::ArrayType radiusRead = ellipsoidObject->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radiusRead[i] != radius[i])
      {
      std::cout << "Radius error : " << radius[i] << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }

  ellipsoidObject->SetRadius(2,3,4);
 
  radiusRead = ellipsoidObject->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radiusRead[i] != radius[i]+1)
      {
      std::cout << "Radius error : " << radius[i] << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  ellipsoidRepresentation->SetColor(0.1,0.2,0.3);
  ellipsoidRepresentation->SetOpacity(0.4);
  if(fabs(ellipsoidRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  ellipsoidRepresentation->RequestSetEllipsoidObject(ellipsoidObject);
  ellipsoidRepresentation->Print(std::cout);
  ellipsoidObject->Print(std::cout);
  ellipsoidObject->GetNameOfClass();
  ellipsoidRepresentation->GetNameOfClass();

  // testing actors
  std::cout << "Testing actors : ";
  typedef igstk::Scene   SceneType;
  SceneType::Pointer scene = SceneType::New();

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  
  // this will indirectly call CreateActors() 
  scene->RequestAddObject( view2D, ellipsoidRepresentation );
    
  std::cout << "[PASSED]" << std::endl;

  // Testing Update
  ellipsoidRepresentation->IsModified();

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
  ellipsoidObject->SetTransform( transform );
  igstk::Transform  transform2 = ellipsoidObject->GetTransform();
  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cout << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  for( unsigned int j=0; j<3; j++ )
    {
    if( !( rotation2 == rotation ) )
      {
      std::cout << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    }



  std::cout << "[PASSED]" << std::endl;


  // Testing the Copy() function
  std::cout << "Testing Copy(): ";
  ObjectRepresentationType::Pointer copy = ellipsoidRepresentation->Copy();
  if(copy->GetOpacity() != ellipsoidRepresentation->GetOpacity())
    {
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;


  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
