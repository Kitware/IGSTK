/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

//#include <stdlib.h>
//#include <crtdbg.h>
//#define _CRTDBG_MAP_ALLOC

#include "LaparoscopyDemo.h"

int main(int , char** )
{ 
  //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
  //char *s =  new char[20];

  igstk::RealTimeClock::Initialize();

  LaparoscopyDemo * application = new LaparoscopyDemo;

  while( !application->HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
    }

  //_CrtDumpMemoryLeaks();

  delete application;

  return EXIT_SUCCESS;
}
