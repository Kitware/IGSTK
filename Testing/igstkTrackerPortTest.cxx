/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerPortTest.cxx
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

#include "igstkTrackerPort.h"

int igstkTrackerPortTest( int, char * [] )
{
    typedef igstk::TrackerTool  TrackerToolType;
    typedef igstk::TrackerPort  TrackerPortType;
    
    TrackerToolType::Pointer trackerTool = TrackerToolType::New();

    TrackerPortType::Pointer trackerPort = TrackerPortType::New();

    trackerPort->ClearTools();

    trackerPort->AddTool( trackerTool );

    const unsigned int nt = trackerPort->GetNumberOfTools();
    std::cout << "Number of tools = " << nt << std::endl;

    TrackerToolType::Pointer trackerTool2 = trackerPort->GetTool(0);

    // Exercise the const methods for GetTool()
    TrackerPortType::ConstPointer trackerPort2 = trackerPort.GetPointer();
    TrackerToolType::ConstPointer trackerTool3 = trackerPort2->GetTool(0);

    trackerPort->ClearTools();

    return EXIT_SUCCESS;
}


