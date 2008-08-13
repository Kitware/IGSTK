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


int main(int , char** )
{ 
    igstk::RealTimeClock::Initialize();


    Tracking *tr = new Tracking;
    igstk::AuroraTrackerConfiguration *config = new igstk::AuroraTrackerConfiguration;
    config->SetCOMPort( igstk::SerialCommunication::PortNumber0 );
    config->RequestSetFrequency( 30 );

    igstk::AuroraToolConfiguration auroraToolConfig;
    auroraToolConfig.SetChannelNumber( 0);     // ask patrick about channel/port issue
    auroraToolConfig.SetPortNumber( 0 );
    //auroraToolConfig.SetSROMFile( "path/filename" );

    config->RequestAddTool( &auroraToolConfig );   

    tr->SetTrackerConfiguration(config);
    tr->InitializeTracking();
    tr->StartTracking();

    while (1)
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

    return EXIT_SUCCESS;
}
