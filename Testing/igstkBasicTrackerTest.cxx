/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBasicTrackerTest.cxx
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

#include "igstkTracker.h"
#include "igstkLogger.h"

int igstkBasicTrackerTest( int, char * [] )
{
    igstk::Tracker::Pointer tracker = igstk::Tracker::New();
    igstk::Logger logger;

    std::ofstream fileStream1("outputLog1.txt");
    std::ofstream fileStream2("outputLog2.txt");

    logger.AddOutputStream( std::cout );
    logger.AddOutputStream( fileStream1 );

    logger.SetPriorityLevel( igstk::Logger::DEBUG );

    tracker->SetLogger( &logger );

    tracker->Initialize();

    tracker->StartTracking();

    tracker->UpdateStatus();

    tracker->StopTracking();

    std::cout << tracker->GetCurrentState() << std::endl;

    tracker->Reset();

    return EXIT_SUCCESS;
}


