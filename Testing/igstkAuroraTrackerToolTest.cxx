/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerToolTest.cxx
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

#include <iostream>
#include <fstream>

#include "igstkAuroraTrackerTool.h"


int igstkAuroraTrackerToolTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::AuroraTrackerTool           TrackerToolType;
  typedef TrackerToolType::TransformType     TransformType;
    
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  //select a 5DOF type
  trackerTool->RequestSelect5DOFTrackerTool();

  //Set invalid port number
  unsigned int invalidPortNumber = 10;
  trackerTool->RequestSetPortNumber( invalidPortNumber );

  //Set valid port number
  unsigned int validPortNumber = 10;
  trackerTool->RequestSetPortNumber( validPortNumber );

  //set invalid channel number
  unsigned int invalidChannelNumber = 10;
  trackerTool->RequestSetChannelNumber( invalidChannelNumber );

  //set valid channel number
  unsigned int validChannelNumber = 0;
  trackerTool->RequestSetChannelNumber( validChannelNumber );

  //set invalid part number
  std::string inValidPartNumber= "";
  trackerTool->RequestSetPartNumber( inValidPartNumber );

  //set valid part number
  std::string validPartNumber= "NDI-156-11";
  trackerTool->RequestSetPartNumber( validPartNumber );

  //set SROM filename
  std::string invalidSROMFileName = "";
  trackerTool->RequestSetSROMFileName( invalidSROMFileName );
 
  trackerTool->RequestConfigure();

  trackerTool->Print( std::cout );

  return EXIT_SUCCESS;
}
