/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView3DTest.cxx
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

#include "igstkView3D.h"

int igstkView3DTest( int, char * [] )
{
  typedef igstk::View3D  Viewer3DType;  
  Viewer3DType* view3D = new Viewer3DType(0,0,100,100,"test");

  igstk::Scene::Pointer scene = igstk::Scene::New();
  view3D->SetScene(scene);
  view3D->Update();

  delete view3D;
  return EXIT_SUCCESS;
}
