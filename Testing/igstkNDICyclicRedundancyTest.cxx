/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICyclicRedundancyTest.cxx
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

#include "igstkNDICyclicRedundancy.h"

int igstkNDICyclicRedundancyTest( int, char * [] )
{
  typedef igstk::NDICyclicRedundancy   CyclicRedundancyType;

  CyclicRedundancyType  cyclicRedundancy;

  char   commandStr[256];

  sprintf(commandStr, "START");

  std::cout << "Cyclic Redundancy of " << commandStr << " = " << cyclicRedundancy.ComputeCyclicRedundancyOfStringInTextMode(commandStr, strlen(commandStr)) << std::endl;

  cyclicRedundancy.AppendCyclicRedundancyTermToCommand(commandStr, 256-6);

  commandStr[strlen(commandStr)] = 0xD;

  std::cout << "Cyclic Redundancy appended string = " << commandStr << std::endl;

  if (cyclicRedundancy.ValidateCyclicRedundancyOfReplyMessage(commandStr, 256))
  {
    std::cout << "Cyclic Redundancy test succeeded." << std::endl;
  }
  else
  {
    std::cout << "Cyclic Redundancy test failed." << std::endl;
  }

  return EXIT_SUCCESS;
}


