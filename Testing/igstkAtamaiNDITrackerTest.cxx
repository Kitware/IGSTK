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

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkAtamaiNDITracker.h"

int igstkAtamaiNDITrackerTest( int, char * [] )
{
  typedef igstk::Transform         TransformType;
  typedef igstk::AtamaiNDITracker  AtamaiNDITrackerType;
  typedef itk::Logger              LoggerType; 
  typedef itk::StdStreamLogOutput  LogOutputType;
   
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

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
