/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' 
// characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "FourViewsTrackingWithCT.h"

int main(int , char** )
{ 
  igstk::FourViewsTrackingWithCT::Pointer application 
                                       = igstk::FourViewsTrackingWithCT::New();

  while(1)
    {
    Fl::wait( 0.01 );
    igstk::PulseGenerator::CheckTimeouts();
    Fl::check();   // trigger FLTK redraws
    }

  return 0;
}
