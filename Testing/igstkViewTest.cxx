/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewTest.cxx
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
#include "igstkView3D.h"

int igstkViewTest( int, char * [] )
{
  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  try
    {

    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(600,300,"View3D Test");
    
    View2DType * view2D = new View2DType( 10,10,280,280,"2D View");
    View3DType * view3D = new View3DType(310,10,280,280,"3D View");

    form->end();
    // End of the GUI creation

    form->show();
    
    view2D->RequestResetCamera();
    view2D->RequestEnableInteractions();
    
    view3D->RequestResetCamera();
    view3D->RequestEnableInteractions();
    
    
    for(unsigned int i=0; i<100; i++)
      {
      view2D->Update();  // schedule redraw of the view
      view3D->Update();  // schedule redraw of the view
      Fl::check();       // trigger FLTK redraws
      }

    view2D->RequestDisableInteractions();
    view3D->RequestDisableInteractions();

    delete view2D;
    delete view3D;

    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


