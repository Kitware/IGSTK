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
#include "igstkView3D.h"

int igstkViewTest( int, char * [] )
{
  typedef igstk::View3D  ViewType;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(510+300,300,"View3D Test");
  
  ViewType * view = new ViewType(10,10,280,280,"Axial View");

  form->end();
  // End of the GUI creation

  form->show();
  
  view->RequestResetCamera();
  view->RequestEnableInteractions();
  
  
  for(unsigned int i=0; i<10; i++)
    {
    view->Update();  // schedule redraw of the view
    Fl::check();     // trigger FLTK redraws
    }

  view->RequestDisableInteractions();

  delete view;

  return EXIT_SUCCESS;
}


