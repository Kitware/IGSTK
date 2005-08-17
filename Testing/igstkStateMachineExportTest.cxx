/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineExportTest.cxx
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
 //Warning about: identifier was truncated to '255' characters in the debug
 //information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

/**
 *  This file generates the State Machine diagram of all the calsses by
 *  invoking their respective Export method.
 */

#include <fstream>
#include <iostream>

#include "igstkMacros.h"
#include "itkExceptionObject.h"

namespace igstk 
{
  
template<class ClassType>
void ExportStateMachineDescription( 
              const ClassType * instance, 
              const std::string & outputDirectory )
  {
  std::string filename = outputDirectory+"/";
  filename = filename + "igstk";
  filename = filename + instance->GetNameOfClass();
  filename = filename + ".dot";
  std::ofstream outputFile;
  outputFile.open( filename.c_str() );
  if( outputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescription( outputFile );
  outputFile.close();
  }

} // end namespace igstk


// This is for classes that use SmartPointers
#define igstkTestExportStateMachine1( type, outputDirectory ) \
  { \
  type::Pointer instance = type::New(); \
  igstk::ExportStateMachineDescription( instance.GetPointer(), outputDirectory ); \
  }

// This is for classes that do not use SmartPointers and have a default constructor
#define igstkTestExportStateMachine2( type, outputDirectory ) \
  { \
  type * instance = new type; \
  igstk::ExportStateMachineDescription( instance, outputDirectory ); \
  delete instance; \
  }




int main( int argc, char * argv [] )
{
  std::string outputDirectory;
  if( argc > 1 )
    {
    outputDirectory = argv[1];
    }

  std::cout << "Output directory = " << outputDirectory << std::endl;
  
  // This is for classes that use SmartPointers


  return EXIT_SUCCESS;
}

