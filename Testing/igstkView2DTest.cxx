/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView2DTest.cxx
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

#include "igstkView2D.h"

int igstkView2DTest( int, char * [] )
{
  typedef igstk::View2D  Viewer2DType;  
  Viewer2DType* view2D = new Viewer2DType(0,0,100,100,"test");
  igstk::Scene::Pointer scene = igstk::Scene::New();
  view2D->SetScene(scene);
  view2D->Update();
  
  delete view2D;

  return EXIT_SUCCESS;
}
