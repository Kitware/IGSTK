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

int igstkEllipsoidObjectTest( int, char * [] )
{
  typedef igstk::EllipsoidObject  ObjectType;
  ObjectType::Pointer ellipsoid = ObjectType::New();

  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  ellipsoid->SetRadius(1,2,3);
  igstk::EllipsoidObject::ArrayType radius = ellipsoid->GetRadius();
  for(unsigned int i=0;i<3;i++)
    {
    if(radius[i] != i+1)
      {
      std::cout << "Radius error : " << radius[i] << " v.s " << i << std::endl; 
      return EXIT_FAILURE;
      }
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  ellipsoid->GetProperty()->SetColor(0.1,0.2,0.3,0.4);
  if(fabs(ellipsoid->GetProperty()->GetRed()-0.1)>0.00001)
    {
    std::cout << ellipsoid->GetProperty()->GetRed() << " != 0.1 [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoid->GetProperty()->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoid->GetProperty()->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(ellipsoid->GetProperty()->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  ellipsoid->Print(std::cout);

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
