/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTest.cxx
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
#include <fstream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "igstkMouseTracker.h"
#include "igstkLogger.h"

int igstkMouseTrackerTest( int, char * [] )
{
    typedef itk::Point< double, 3 >  PositionType;
    igstk::MouseTracker::Pointer tracker = igstk::MouseTracker::New();
    igstk::Logger logger;

    Fl_Window mouseWindow(20, 20, 400, 400,"Mouse Tracker Test");
    
    mouseWindow.show();               

    logger.AddOutputStream( std::cout );

    logger.SetPriorityLevel( igstk::Logger::DEBUG );

    tracker->SetLogger( &logger );

    tracker->Initialize();

    tracker->StartTracking();

    tracker->UpdateStatus();

    PositionType pos;

    tracker->GetToolPosition( 0, 0, pos );

    tracker->StopTracking();
    std::cout << "Tool Position -> ( " << pos[0] << "," << pos[1] << "," << pos[2] << ")" << std::endl;

    tracker->Reset();

    return Fl::run();
    //return EXIT_SUCCESS;
}
