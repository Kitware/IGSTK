/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
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
#pragma warning( disable : 4284 )
#endif


#include <stdlib.h>
#include <iostream>

#include "Tracking.h"
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkTrackerConfigurationParser.h"



int main(int argc, char** argv)
{ 
    if (argc < 3 || argc > 4)
    {
        cerr << "Usage: TrackerConfiguration configXMLFile framesPerSeconds [portNumber (default: 18944)]\n";
        return 1;
    }

    igstk::RealTimeClock::Initialize();

    igstkTrackerConfigurationParser *parser = igstkTrackerConfigurationParser::New();
    parser->SetFileName(argv[1]);
    bool updated = parser->CreateConfiguration();
    if (! updated)
    {
        cerr << "Creating tracker configuration failed with xml file: " << argv[1] << endl;
        return 2;
    }

    double fps = atof(argv[2]);

    int port = 18944;
    if (argc == 4)
    {
        port = atoi(argv[3]);
    }
 
    Tracking *tr = new Tracking;
    tr->SetTrackerConfiguration(parser->GetTrackerConfig());
    tr->SetPort(port);
    tr->SetFramesPerSecond(fps);

    tr->SetHostName(parser->GetHostName());
    tr->InitializeTracking();
    tr->StartTracking();
    
    while (1)
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    }


    parser->Delete();
    delete tr;
    return EXIT_SUCCESS;
}
