/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _igstkView3D_h
#define _igstkView3D_h

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include "igstkView.h"

namespace igstk{

class View3D : public View 
{
public:
   
  View3D( int x, int y, int w, int h, const char *l="");
  ~View3D( void );

protected:

  int  handle( int event );

};

} // end namespace igstk

#endif
