/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToSocketRelayTest.cxx
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
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkRealTimeClock.h"
#include "igstkCircularSimulatedTracker.h"
#include "igstkSimulatedTrackerTool.h"
#include "igstkTrackerToolObserverToSocketRelay.h"

int igstkTrackerToolObserverToSocketRelayTest( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " hostname portnumber " << std::endl;
    return EXIT_FAILURE;
    }

  igstk::RealTimeClock::Initialize();

  typedef igstk::CircularSimulatedTracker               TrackerType;
  typedef igstk::SimulatedTrackerTool                   TrackerToolType;
  typedef igstk::TrackerToolObserverToSocketRelay       ObserverType;

  TrackerType::Pointer      tracker      = TrackerType::New();
  TrackerToolType::Pointer  trackerTool  = TrackerToolType::New();
  ObserverType::Pointer     toolObserver = ObserverType::New();

  tracker->RequestOpen();

  trackerTool->RequestSetName("Tool_1");
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  toolObserver->RequestSetTrackerTool( trackerTool );
  toolObserver->RequestSetHostName( argv[1] );
  toolObserver->RequestSetPort( atoi( argv[2] ) );
  toolObserver->RequestStart();

  tracker->RequestStartTracking();

  for( unsigned int i = 0; i < 100; i++ )
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  tracker->RequestStopTracking();
  tracker->RequestReset();
  tracker->RequestClose();

  return EXIT_SUCCESS;
}
