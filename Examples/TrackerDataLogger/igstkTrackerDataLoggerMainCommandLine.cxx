/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerDataLoggerMainCommandLine.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTrackerDataLogger.h"

int main( int argc, char *argv[] )
{
  if( argc!=2 )
  {
    std::cerr<<"Usage: "<<argv[0];
    std::cerr<<" tracker_xml_configuration_file.\n";
    return EXIT_FAILURE;
  }

  try 
  {
    std::string s = argv[1];
    TrackerDataLogger *b = new TrackerDataLogger( s );
    b->StartTracking();
    while( 1 ) 
    {
      igstk::PulseGenerator::Sleep( 10 );
      igstk::PulseGenerator::CheckTimeouts();
    }
    return EXIT_SUCCESS;
  }
  catch( std::exception &e )
  {
    std::cerr<<e.what()<<"\n";
    return EXIT_FAILURE;
  }
}
