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
  const unsigned int DEFAULT_DELAY = 5000;

  if( argc<2 || argc>3 )
  {
    std::cerr<<"Usage: "<<argv[0];
    std::cerr<<" tracker_xml_configuration_file [delay_in_whole_millisec].\n";
    return EXIT_FAILURE;
  }

  try 
    {
    unsigned int tmpDelay, delay = DEFAULT_DELAY;
      //user specified delay
    if( argc == 3 )
      {
      std::istringstream instr;
      instr.str( argv[2] );
      instr>>tmpDelay;
      if( !instr.eof() )
        {
        std::cerr<<"Delay not given as whole, positive, numeric value.\n";
        std::cerr<<"Usage: "<<argv[0];
        std::cerr<<" tracker_xml_configuration_file [delay_in_whole_millisec].\n";
        return EXIT_FAILURE;
        }
      delay = tmpDelay;
      }

       //print to screen and update the time till data is acquired
    std::stringstream s;
    unsigned int len = 0;
    for(unsigned int i=delay; i>0; i-=1000 )
      {      //remove line
      std::cout<<std::string(len,'\b');
      s<<"Data acquisition starts in "<<(int)(i/1000)<<" seconds.";
      len = s.str().length();
      std::cout<<s.str();
          //clear the stream
      s.str("");
      igstk::PulseGenerator::Sleep(1000);
      }
              //go to beginning of line and overwrite previous line with spaces
    std::cout<<std::string(len,'\b')<<std::string(len,' ');
    std::cout<<std::string(len,'\b')<<"Data acquisition starts now.\n";


          //start the actual work
    TrackerDataLogger *b = new TrackerDataLogger( std::string( argv[1] ) );
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
