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
  ellipsoidRepresentation->SetEllipsoid(ellipsoidObject);
  ellipsoidRepresentation->Print(std::cout);
  ellipsoidObject->Print(std::cout);
  ellipsoidObject->GetNameOfClass();
  ellipsoidRepresentation->GetNameOfClass();

  // Testing CreateActors()
  std::cout << "Testing actors : ";
  ellipsoidRepresentation->CreateActors();
  std::cout << "[PASSED]" << std::endl;

  // Testing Update
  ellipsoidRepresentation->IsModified();

  // Test GetOffset()
  std::cout << "Testing Set/GetOffset(): ";
  ellipsoidObject->SetOffset(0,1,2);
  igstk::SpatialObject::VectorType offset = ellipsoidObject->GetOffset();
  for(unsigned int i=0;i<3;i++)
    {
    if(offset[i] != i)
      {
      std::cout << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing Set/GetMatrix(): ";
  const igstk::SpatialObject::MatrixType matrix = ellipsoidObject->GetMatrix();
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
