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

int igstkCylinderObjectTest( int, char * [] )
{
  typedef igstk::CylinderObject  ObjectType;
  ObjectType::Pointer Cylinder = ObjectType::New();

  // Test Set/GetRadius()
  std::cout << "Testing Set/GetRadius() : ";
  Cylinder->SetRadius(1.0);
  double radius = Cylinder->GetRadius();
 
  if(radius != 1.0)
    {
    std::cout << "Radius error : " << radius << " v.s 1.0" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Set/GetHeight()
  std::cout << "Testing Set/GetHeight() : ";
  Cylinder->SetHeight(5.0);
  double height = Cylinder->GetHeight();
 
  if(height != 5.0)
    {
    std::cout << "Height error : " << height<< " v.s 5.0" << std::endl; 
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test Property
  std::cout << "Testing Property : ";
  Cylinder->SetColor(0.1,0.2,0.3);
  Cylinder->SetOpacity(0.4);
  if(fabs(Cylinder->GetRed()-0.1)>0.00001)
    {
    std::cout << "GetRed() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(Cylinder->GetGreen()-0.2)>0.00001)
    {
    std::cout << "GetGreen()[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(Cylinder->GetBlue()-0.3)>0.00001)
    {
    std::cout << "GetBlue() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  if(fabs(Cylinder->GetOpacity()-0.4)>0.00001)
    {
    std::cout << "GetOpacity() [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Testing PrintSelf()
  Cylinder->Print(std::cout);

  std::cout << "Test [DONE]" << std::endl;
  return EXIT_SUCCESS;
}
