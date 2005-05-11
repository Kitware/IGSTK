/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkFLTKTextLogOutputTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include "igstkFLTKTextLogOutput.h"
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Text_Display.h>
#include <FL/Fl_Text_Buffer.h>


int igstkFLTKTextLogOutputTest( int, char * [] )
{
  try
    {

    // Create an igstk::FLTKTextLogOutput
    igstk::FLTKTextLogOutput::Pointer output = igstk::FLTKTextLogOutput::New();

    std::cout << "Testing igstk::FLTKTextLogOutput" << std::endl;
    std::cout.precision(20);

    Fl_Window *win = new Fl_Window(0,0,400,300,"igstkFLTKTextLogOutputTest");
    Fl_Text_Display *texts = new Fl_Text_Display(0,0,400,300,NULL);
    Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
    texts->buffer(textBuffer);
    win->end();
    win->show();

    output->SetStream(*texts);
    output->Write(1.2345);
    output->Write("This is the test message.\n");
    double stamp;
    for( stamp = 1; stamp < 100; stamp += 1 )
    {
      output->Write("This is the test message.\n", stamp);
    }
    output->Flush();

    Fl::run();
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}


