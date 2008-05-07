/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSimulatedTrackerTest.cxx
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

#include "igstkSimulatedTracker.h"
#include "igstkSimulatedTrackerTool.h"

int igstkSimulatedTrackerTest( int, char * [] )
{
  typedef igstk::SimulatedTracker     TrackerType;
  typedef igstk::SimulatedTrackerTool ToolType;

  TrackerType::Pointer tracker = TrackerType::New();
  tracker->RequestSetFrequency( 30 );
  tracker->RequestOpen();

  ToolType::Pointer tool = ToolType::New();
  tool->RequestSetName( "tool" );
  tool->GetName();
  tool->RequestConfigure();
  tool->RequestAttachToTracker( tracker );

  tracker->RequestStartTracking();
  tracker->RequestStopTracking();
  tracker->RequestSetReferenceTool( tool );
  tracker->RequestStartTracking();
  tracker->RequestReset();

  tool->RequestDetachFromTracker();

  tracker->RequestStopTracking();
  tracker->RequestReset();
  tracker->RequestClose();

  tool->RequestDetachFromTracker();


  tracker->Print( std::cout );
  tool->Print( std::cout );

  return EXIT_SUCCESS;
}
