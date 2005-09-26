/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolTest.cxx
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

#include "igstkTrackerTool.h"

int igstkTrackerToolTest( int, char * [] )
{
    typedef igstk::TrackerTool                 TrackerToolType;
    typedef TrackerToolType::TransformType     TransformType;
    typedef TrackerToolType::ErrorType         ErrorType;
    typedef TrackerToolType::ErrorType         TimePeriodType;
    
    TrackerToolType::Pointer trackerTool = TrackerToolType::New();

    TransformType   transform   = trackerTool->GetTransform();

    trackerTool->SetTransform( transform );

    TimePeriodType period = 10.0; // measures are valid for 10 milliseconds
    trackerTool->SetValidityPeriod( period );

    TimePeriodType period2 = trackerTool->GetValidityPeriod();

    if( fabs( period2 - period ) > 1e-6 )
      {
      std::cerr << "ERROR: inconsistency between ";
      std::cerr << " SetValidityPeriod() and GetValidityPeriod() " <<  std::endl;
      std::cerr << " set period = " << period  << std::endl;
      std::cerr << " get period = " << period2 << std::endl;
      return EXIT_FAILURE;
      }

    std::cout << trackerTool << std::endl;

    return EXIT_SUCCESS;
}


