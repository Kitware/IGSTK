/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMultipleOutputTest.cxx
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
#include <cstdlib>
#include "igstkMultipleOutput.h"
#include "igstkRealTimeClock.h"

int igstkMultipleOutputTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::MultipleOutput  MultipleOutputType;  
  MultipleOutputType multipleOutput;

  std::ofstream file1("fileMultipleOutput1.txt");
  std::ofstream file2("fileMultipleOutput2.txt");
      
  if( file1.fail() || file2.fail() )
    {
    std::cerr << "Problem opening file" << std::endl;
    return EXIT_FAILURE;
    }

  multipleOutput.AddOutputStream( std::cout );
  multipleOutput.AddOutputStream( file1 );
  multipleOutput.AddOutputStream( file2 );

  std::ostringstream messageStream; 

  multipleOutput << 12.4;
  multipleOutput << " text ";
  multipleOutput.Flush();
  multipleOutput << 23;
  multipleOutput << " reload ";
  messageStream << 12.4;
  messageStream << " text ";
  messageStream << 23;
  messageStream << " reload ";

  std::string message = messageStream.str();

  file1.close();
  file2.close();

  // Regression test
  // 
  // open the files again
  //
  std::ifstream ifile1("fileMultipleOutput1.txt");
  std::ifstream ifile2("fileMultipleOutput2.txt");
      
  if( ifile1.fail() )
    {
    std::cerr << "fileMultipleOutput1.txt failed to open" << std::endl;
    return EXIT_FAILURE;
    }

  if( ifile2.fail() )
    {
    std::cerr << "fileMultipleOutput2.txt failed to open" << std::endl;
    return EXIT_FAILURE;
    }

  unsigned int i=0;
  char c1 = ifile1.get();
  char c2 = ifile2.get();
  while( !ifile1.eof() && !ifile2.eof() )
    {
    if( c1 != c2 )
      {
      std::cerr << "The Two files differ !" << std::endl;
      return EXIT_FAILURE;
      }
    if( c1 != message[i] )
      {
      std::cerr << "The files do not have the right message !" << std::endl;
      std::cerr << "Character number " << i << std::endl;
      std::cerr << "file is " << int(c1) << std::endl;
      std::cerr << "message is " << message[i] << std::endl;
      return EXIT_FAILURE;
      }
    i++;
    c1 = ifile1.get();
    c2 = ifile2.get();
    }

  ifile1.close();
  ifile2.close();

  return EXIT_SUCCESS;
}
