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

int igstkMouseTrackerTest( int, char * [] )
{
  typedef igstk::Transform         TransformType;
  typedef igstk::MouseTracker      MouseTrackerType;
  typedef igstk::Logger            LoggerType; 
    
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  logger->AddOutputStream( std::cout );
  logger->SetPriorityLevel( igstk::Logger::DEBUG );

  MouseTrackerType::Pointer tracker = MouseTrackerType::New();
  tracker->SetLogger( logger );
  tracker->Initialize();

  tracker->StartTracking();

  tracker->UpdateStatus();

  TransformType transform;

  tracker->GetTransform( transform );

  tracker->Reset();

  tracker->StopTracking();

  TransformType::VectorType position = transform.GetTranslation();
  
  std::cout << "Mouse Position -> ( " << position[0] << "," << position[1] << "," << position[2] << ")" << std::endl;

  tracker->Close();

  return EXIT_SUCCESS;
}


