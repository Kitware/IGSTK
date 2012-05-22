/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: MultiTrackerLogger.cxx,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// BeginLatex
// Here we show parts of the implementation part of the TrackerLogger
// application. One important concept is that we collect all the available
// specific trackers as a list of their identifiers. We define a static method
// in the TrackerInitializer class so that we can query the available tracker
// implementations. It also requires a static member m_TrackerTypes, which has
// to be defined here.
// EndLatex

// BeginCodeSnippet
#include <iostream>
#include <fstream>

#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"
#include "igstkAxesObject.h"

#include "MultiTrackerLogger.h"

// data storage of available trackers
std::list<std::string> TrackerInitializer::m_TrackerTypes;
// EndCodeSnippet

// BeginLatex
// The main function of our application defines the application logic in a
// tracker-independent way using only the TrackerInitializer interface to
// create and initialize the appropriate IGSTK Tracker object along with the
// required TrackerTool based objects. In the followings we'll show how to
// query the identifiers of the available tracker implementations,
// and how to initialize a tracker with the given identifier.
// EndLatex

int main( int argc, char **argv )
{
  if( argc == 1 ){
    std::cout << "Usage : " << std::endl;
    std::cout << argv[0] <<
            " -t <tracker_id> [-1 | -2] sample_count outfile.txt" << std::endl;
    std::cout << "Available Tracker ID's: " << std::endl;

// BeginLatex
// First, we query the identifier list of the available tracker implementations.
// EndLatex
// BeginCodeSnippet
    std::list<std::string> trackerIDs =
                                TrackerInitializer::GetAvailableTrackerTypes();
    for( std::list<std::string>::iterator it =
                            trackerIDs.begin(); it != trackerIDs.end(); ++it ){
      std::cout << "\t" << *it << std::endl;
    }
// EndCodeSnippet

    std::cout << " -1: Read data from one tool. " << std::endl;
    std::cout <<
      " -2: Read data from tool 0, and use tool 1 as reference." << std::endl;

    return -1;
  }

  std::string trackerID;
  unsigned int sampleCount;
  std::string outFileName;
  bool bRefMode;
  bool bModeSet(false);
  bool bTrackerIDSet(false);
  bool bSampleCountSet(false);
  bool bFileNameSet(false);

  for( int i = 1; i < argc; ++i ){
    if( !strncmp(argv[i], "-t", 2) ){
      trackerID = argv[i+1];
      bTrackerIDSet = true;
      ++i;
      continue;
    }

    if( !strncmp(argv[i], "-1", 2) ){
      if(bModeSet){
        std::cout <<
         "Error: [-1 | -2] can only be set exclusively and once." << std::endl;
        return -1;
      }

      bModeSet = true;
      bRefMode = false;
      continue;
    }

    if( !strncmp(argv[i], "-2", 2) ){
      if(bModeSet){
        std::cout <<
          "Error: [-1 | -2] can only be set exclusively and once." << std::endl;
        return -1;
      }

      bModeSet = true;
      bRefMode = true;
      continue;
    }

    if( !bSampleCountSet ){
      std::istringstream inpStream(argv[i]);

      if( inpStream >> sampleCount ){
        bSampleCountSet = true;
        continue;
        } else {
        std::cout << "Error: sample number format error." << std::endl;
        return -1;
      }
    }

    if( !bFileNameSet ){
      outFileName = std::string(argv[i]);
      bFileNameSet = true;
      continue;
    }

    std::cout
          << "Error: extra parameter '" << argv[i] << " given." << std::endl;
    return -1;
  }

  if( !bTrackerIDSet ){
    std::cout << "Error: missing -t option!" << std::endl;
    return -1;
  }

  if( !bSampleCountSet ){
    std::cout << "Error: missing sample count!" << std::endl;
    return -1;
  }

  if( !bFileNameSet ){
    std::cout << "Error: missing output filename!" << std::endl;
    return -1;
  }


  // initialize the given tracker
  bool trackerIDFound(false);
  std::list<std::string> trackerIDs =
                              TrackerInitializer::GetAvailableTrackerTypes();
  for( std::list<std::string>::iterator it =
                          trackerIDs.begin(); it != trackerIDs.end(); ++it ){
    if( *it == trackerID ){
      trackerIDFound = true;
      break;
      }
    }
  if( !trackerIDFound ){
    std::cout <<
    "Error: given tracker ID is not supported by the application!" << std::endl;
    return -1;
  }

  // init igstk
  std::cout << "init igstk\n" << std::endl;
  igstk::RealTimeClock::Initialize();

  // create world reference
  igstk::AxesObject::Pointer worldReference;
  worldReference = igstk::AxesObject::New();
  igstk::Transform transformWorld;
  transformWorld.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

// BeginLatex
// Once we know the identifier of the desired tracker, we can create the device
// specific TrackerInitializer derived object using the static factory method
// from the TrackerInitializer class.
// EndLatex
// BeginCodeSnippet
  TrackerInitializer *trackerInitializer =
                        TrackerInitializer::CreateTrackerInitializer(trackerID);
// EndCodeSnippet

// BeginLatex
// Now we have the specialization of the initializer for the desired tracker
// object, so we can request the objects required by the IGSTK framework.
// EndLatex
// BeginCodeSnippet
  igstk::Tracker::Pointer tracker = trackerInitializer->CreateTracker();
  tracker->RequestSetTransformAndParent( transformWorld, worldReference );

  tracker->RequestOpen();

  igstk::TrackerTool::Pointer tool0 = trackerInitializer->CreateTrackerTool(0);
  tool0->RequestConfigure();
  tool0->RequestAttachToTracker( tracker );

  igstk::TrackerTool::Pointer tool1;

  if( bRefMode ){
    tool1 = trackerInitializer->CreateTrackerTool(1);
    tool1->RequestConfigure();
    tool1->RequestAttachToTracker( tracker );

    tracker->RequestSetReferenceTool(tool1);
    }

  // EndCodeSnippet
  // BeginLatex
  // Everything is in place, we can use the standard IGSTK tracker framework
  // with the objects we've received from the tracker initializer.
  // EndLatex
  // BeginCodeSnippet
  std::cout << "start tracking\n" << std::endl;
  tracker->RequestStartTracking();
  std::cout << "start tracking done\n" <<  std::endl;
  // EndCodeSnippet

  //Add observer to listen to transform events
  typedef igstk::TransformObserver      ObserverType;
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( tool0 );

  std::ofstream of(outFileName.c_str());

  // get the samples
  for(unsigned int i=0; i<sampleCount; i++)
    {
    igstk::PulseGenerator::Sleep( 200 );
    igstk::PulseGenerator::CheckTimeouts();

    igstk::Transform transform;
    igstk::Transform::VectorType position;

    coordSystemAObserver->Clear();

    if( bRefMode )
    tool0->RequestComputeTransformTo( tool1 );
    else
    tool0->RequestComputeTransformTo( worldReference );

    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if( !transform.IsValidNow() )
      continue;

      position = transform.GetTranslation();
      std::cout << "Trackertool :"
              << tool0->GetTrackerToolIdentifier()
              << "\t\t  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

      igstk::Transform::VersorType v = transform.GetRotation();

    of << position[0] << " " << position[1] << " " << position[2] << " ";

    std::cout << "quaternion: " << v.GetX() << " "
                                << v.GetY() << " "
                                << v.GetZ() << " "
                                << v.GetW() << std::endl;
    of << v.GetX() << " "
         << v.GetY() << " "
       << v.GetZ() << " "
       << v.GetW() << std::endl;
      }
      else {
      std::cout << "got no transform" << std::endl;
      }
    }

  of.close();

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();
}
// EndCodeSnippet
