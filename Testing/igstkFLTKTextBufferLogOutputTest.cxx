/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKTextBufferLogOutputTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkFLTKTextBufferLogOutput.h"
#include "igstkRealTimeClock.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>


int igstkFLTKTextBufferLogOutputTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  try
    {

    // Create an igstk::FLTKTextBufferLogOutput
    igstk::FLTKTextBufferLogOutput::Pointer 
                              output = igstk::FLTKTextBufferLogOutput::New();

    std::cout << "Testing igstk::FLTKTextBufferLogOutput" << std::endl;
    std::cout.precision(20);

    Fl_Window *win = new Fl_Window(0,0,400,300,
                                   "igstkFLTKTextBufferLogOutputTest");
    Fl_Text_Display *texts = new Fl_Text_Display(0,0,400,300,NULL);
    Fl_Text_Buffer *textBuffer = new Fl_Text_Buffer();
    texts->buffer(textBuffer);
    win->end();
    win->show();

    std::cout << output << std::endl;
    output->SetStream(*textBuffer);
    igstk::FLTKTextBufferLogOutput::StreamPointerType stream;
    stream = output->GetStream();
    if( stream != textBuffer )
      {
      std::cout << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    output->Write(1.2345);
    output->Write("This is the test message.\n");

    std::cout << output << std::endl;

    double stamp;
    
    for( stamp = 1; stamp < 100; stamp += 1 )
      {
      output->Write("This is the test message.\n", stamp);
      output->Flush();
      Fl::check();
      }

    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
