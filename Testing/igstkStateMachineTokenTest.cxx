/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineTokenTest.cxx
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

#include "igstkStateMachineToken.h"

int igstkStateMachineTokenTest( int, char * [] )
{
    typedef igstk::StateMachineToken  StateMachineTokenType;
    
    StateMachineTokenType token;

    std::cout << "Test the rollup of the number 2^32" << std::endl;
    const unsigned int biggest = (unsigned int)(-1);
    // Invoking the constructor of the token up to reaching the numeric limit.
    for(unsigned int i=0; i<biggest; i++)
      {
      StateMachineTokenType createOneMoreToken;
      }

    // and yet... do a little bit more
    for(unsigned int j=0; j<100; j++)
      {
      StateMachineTokenType createOneMoreToken;
      }

    return EXIT_SUCCESS;
}


