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

#include <iostream>
#include <fstream>

#include "igstkTracker.h"
#include "igstkLogger.h"

int igstkBasicTrackerTest(int argc, char * argv [])
{
    igstk::Tracker::Pointer tracker = igstk::Tracker::New();
    igstk::Logger logger;

    std::ofstream fileStream1("outputLog1.txt");
    std::ofstream fileStream2("outputLog2.txt");

    logger.AddOutputStream( std::cout );
    logger.AddOutputStream( fileStream1 );

    logger.SetPriorityLevel( igstk::Logger::CRITICAL );

    tracker->SetLogger( &logger );

    tracker->Initialize();

    tracker->StartTracking();

    tracker->UpdateToolStatus();

    tracker->StopTracking();

    return EXIT_SUCCESS;
}


