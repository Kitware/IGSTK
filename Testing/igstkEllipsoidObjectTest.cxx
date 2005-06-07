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
      std::cerr << "Radius error : " << radius[i] << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }

  ellipsoidObject->SetRadius(2,3,4);
 
  radiusRead = ellipsoidObject->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radiusRead[i] != radius[i]+1)
      {
      std::cerr << "Radius error : " << radius[i] << " v.s " << i << std::endl; 
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
    std::cerr << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cerr << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cerr << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoidRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cerr << "GetOpacity() [FAILED]" << std::endl;
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

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  
  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( ellipsoidRepresentation );
    
  std::cout << "[PASSED]" << std::endl;

  // Testing UpdateRepresentationFromGeometry. Changing the Spatial Object
  // geometrical parameters should trigger an update in the representation
  // class.
  std::cout << "Testing UpdateRepresentationFromGeometry() : ";
  ellipsoidObject->SetRadius( 20.0, 37.0, 39.0 );
  
  // Testing Update
  ellipsoidRepresentation->IsModified();

  // Testing again in order to exercise the other half of an if().
  ellipsoidRepresentation->IsModified();
  ellipsoidRepresentation->SetColor(0.3,0.7,0.2);
  if( !ellipsoidRepresentation->IsModified() )
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
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  ellipsoidObject->RequestSetTransform( transform );
  igstk::Transform  transform2 = ellipsoidObject->GetTransform();
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



  std::cout << "[PASSED]" << std::endl;


  // Testing the Copy() function
  std::cout << "Testing Copy(): ";
  ObjectRepresentationType::Pointer copy = ellipsoidRepresentation->Copy();
  if(copy->GetOpacity() != ellipsoidRepresentation->GetOpacity())
    {
    std::cerr << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetEllipsoidObject() with a null pointer as argument
  std::cout << "Testing RequestSetEllipsoidObject() with NULL argument: ";
  ObjectRepresentationType::Pointer ellipsoidRepresentation3 = ObjectRepresentationType::New();
  ellipsoidRepresentation3->RequestSetEllipsoidObject( 0 );

  // Exercise RequestSetEllipsoidObject() called twice. The second call should be ignored.
  std::cout << "Testing RequestSetEllipsoidObject() called twice: ";
  ObjectRepresentationType::Pointer ellipsoidRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer ellipsoidObjectA = ObjectType::New();
  ObjectType::Pointer ellipsoidObjectB = ObjectType::New();
  ellipsoidRepresentation4->RequestSetEllipsoidObject( ellipsoidObjectA );
  ellipsoidRepresentation4->RequestSetEllipsoidObject( ellipsoidObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  ellipsoidRepresentation->SetColor(0.9,0.7,0.1);
  ellipsoidRepresentation->SetOpacity(0.8);

  std::cout << ellipsoidRepresentation << std::endl;
  std::cout << ellipsoidObjectA << std::endl;

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
