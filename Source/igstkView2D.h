/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _igstkView2D_h
#define _igstkView2D_h

#include "igstkView.h"

namespace igstk {


  /**
   * \brief View2D provies the functionality of rendering a scene in a 2D window.
   *
   */
class View2D : public View 
{
public:
   
  View2D( int x, int y, int w, int h, const char *l="");
  ~View2D( void );

protected:

  int  handle( int event );

};

} // end namespace igstk

#endif

