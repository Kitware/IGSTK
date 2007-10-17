/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTokenTest.cxx
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

#include "igstkToken.h"
#include "igstkRealTimeClock.h"

int igstkTokenTest( int argc , char * argv [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Token  TokenType;
    
  TokenType token;

  std::cout << token << std::endl;

  if( argc > 1 )
    {
    if( atoi( argv[1] ) )
      {
      // Test the case when the token identifier rolls over.
      // This is a time-consuming test. It is reserved to be
      // run weekly. Once Dart2 becomes available.
      std::cout << "Test the rollup of the number 2^32" << std::endl;
      const unsigned int biggest = (unsigned int)(-1);
      // Invoking the constructor of the token up to reaching the numeric limit.
      for(unsigned int i=0; i<biggest; i++)
        {
        TokenType createOneMoreToken;
        }
      }
    }
  else
    {
    // Create some tokens
    const unsigned int numberOfTokens = 10000;
    for(unsigned int j=0; j<numberOfTokens; j++)
      {
      TokenType createOneMoreToken;
      }
    }
  return EXIT_SUCCESS;
}
