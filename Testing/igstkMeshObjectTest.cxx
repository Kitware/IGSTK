/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObjectTest.cxx
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

#include "igstkMeshObject.h"
#include "igstkMeshObjectRepresentation.h"
#include "igstkView2D.h"


int igstkMeshObjectTest( int, char * [] )
{
  typedef igstk::MeshObjectRepresentation  ObjectRepresentationType;
  ObjectRepresentationType::Pointer MeshRepresentation = ObjectRepresentationType::New();

  typedef igstk::MeshObject ObjectType;
  typedef ObjectType::PointType MeshPointType;

  ObjectType::Pointer MeshObject = ObjectType::New();

  MeshRepresentation->RequestSetMeshObject( MeshObject );

  MeshObject->AddPoint(0,0,0,0);
  MeshObject->AddPoint(1,9,0,0);
  MeshObject->AddPoint(2,9,9,0);
  MeshObject->AddPoint(3,0,0,9);
  MeshObject->AddTetrahedronCell(0,0,1,2,3);

  // Test Property
  std::cout << "Testing Property : ";
  MeshRepresentation->SetColor(0.1,0.2,0.3);
  MeshRepresentation->SetOpacity(0.4);
  if(fabs(MeshRepresentation->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(MeshRepresentation->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  MeshRepresentation->Print(std::cout);
  MeshRepresentation->GetNameOfClass();
  MeshObject->GetNameOfClass();
  MeshObject->Print(std::cout);

  // Testing CreateActors()
  std::cout << "Testing actors : ";

  typedef igstk::View2D  View2DType;
  View2DType * view2D = new View2DType(0,0,200,200,"View 2D");
  
  // this will indirectly call CreateActors() 
  view2D->RequestAddObject( MeshRepresentation );

  // Testing Update
  MeshRepresentation->IsModified();

  // Testing again in order to exercise the other half of an if().
  MeshRepresentation->IsModified();
  MeshRepresentation->SetColor(0.3,0.7,0.2);
  if( !MeshRepresentation->IsModified() )
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

  MeshObject->RequestSetTransform( transform );
  igstk::Transform  transform2 = MeshObject->GetTransform();
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
  ObjectRepresentationType::Pointer MeshRepresentation2 = MeshRepresentation->Copy();
  view2D->RequestAddObject( MeshRepresentation2 );
  if(MeshRepresentation2->GetOpacity() != MeshRepresentation->GetOpacity())
    {
    std::cerr << "Copy() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Exercise RequestSetMeshObject() with a null pointer as argument
  std::cout << "Testing RequestSetMeshObject() with NULL argument: ";
  ObjectRepresentationType::Pointer MeshRepresentation3 = ObjectRepresentationType::New();
  MeshRepresentation3->RequestSetMeshObject( 0 );

  // Exercise RequestSetMeshObject() called twice. The second call should be ignored.
  std::cout << "Testing RequestSetMeshObject() called twice: ";
  ObjectRepresentationType::Pointer MeshRepresentation4 = ObjectRepresentationType::New();
  ObjectType::Pointer MeshObjectA = ObjectType::New();
  ObjectType::Pointer MeshObjectB = ObjectType::New();
  MeshRepresentation4->RequestSetMeshObject( MeshObjectA );
  MeshRepresentation4->RequestSetMeshObject( MeshObjectB );

  // Set properties again in order to exercise the loop that goes through
  // Actors
  std::cout << "Testing set properties : ";
  MeshRepresentation->SetColor(0.9,0.7,0.1);
  MeshRepresentation->SetOpacity(0.8);

  std::cout << MeshRepresentation << std::endl;
  std::cout << MeshObjectA << std::endl;

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
