/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAtamaiNDITrackerTest.cxx
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
#include <set>

#include "itkCommand.h"

#include "igstkLogger.h"
#include "igstkAtamaiNDITracker.h"


int igstkAtamaiNDITrackerTest( int, char * [] )
{
  typedef igstk::Transform         TransformType;
  typedef igstk::AtamaiNDITracker  AtamaiNDITrackerType;
  typedef igstk::Logger            LoggerType; 
   
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  logger->AddOutputStream( std::cout );
  logger->SetPriorityLevel( igstk::Logger::DEBUG );

  AtamaiNDITrackerType::Pointer tracker = AtamaiNDITrackerType::New();


  tracker->SetLogger( logger );

  tracker->Initialize();

  tracker->StartTracking();

  for(int i=0; i<10; i++)
  {
    tracker->UpdateStatus();
  }

  tracker->Reset();

  tracker->StopTracking();

  tracker->Close();

  return EXIT_SUCCESS;
}
