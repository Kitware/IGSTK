/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMultipleOutputTest.cxx
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

#include "igstkMultipleOutput.h"

int igstkMultipleOutputTest( int, char * [] )
{
    typedef igstk::MultipleOutput  MultipleOutputType;
    
    MultipleOutputType moutput;

    std::ofstream file1("fileMultipleOutput1.txt");
    std::ofstream file2("fileMultipleOutput2.txt");
      
    if( file1.fail() || file2.fail() )
      {
      std::cerr << "Problem opening file" << std::endl;
      return EXIT_FAILURE;
      }

    moutput.AddOutputStream( std::cout );
    moutput.AddOutputStream( file1 );
    moutput.AddOutputStream( file2 );

    moutput << 12.4;
    moutput << " text ";
    moutput << 23;

    return EXIT_SUCCESS;
}


