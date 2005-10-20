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
#include "itkExceptionObject.h"

#include "igstkCTImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkLandmarkRegistration.h"

namespace igstk 
{
  
template<class ClassType>
void ExportStateMachineDescription( 
              const ClassType * instance, 
              const std::string & outputDirectory, bool skipLoops )
  {
  std::string filename = outputDirectory+"/";
  filename = filename + "igstk";
  filename = filename + instance->GetNameOfClass();


  std::string dotfilename = filename + ".dot";
  std::ofstream dotOutputFile;
  dotOutputFile.open( dotfilename.c_str() );
  if( dotOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescription( dotOutputFile, skipLoops );
  dotOutputFile.close();


  std::string ltsfilename = filename + ".lts";
  std::ofstream ltsOutputFile;
  ltsOutputFile.open( ltsfilename.c_str() );
  if( ltsOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescription( ltsOutputFile, skipLoops );
  ltsOutputFile.close();

  }


} // end namespace igstk


// This is for classes that use SmartPointers
#define igstkTestExportStateMachine1( type, outputDirectory, skipLoops ) \
  { \
  type::Pointer instance = type::New(); \
  igstk::ExportStateMachineDescription( instance.GetPointer(), outputDirectory, skipLoops ); \
  }

// This is for classes that do not use SmartPointers and have a default constructor
#define igstkTestExportStateMachine2( type, outputDirectory, skipLoops ) \
  { \
  type * instance = new type; \
  igstk::ExportStateMachineDescription( instance, outputDirectory, skipLoops ); \
  delete instance; \
  }


#define igstkDeclareSurrogateClass( surrogate, type ) \
class surrogate : public type \
  {  \
public:      \
    typedef surrogate                      Self;    \
    typedef itk::Object                    Superclass;    \
    typedef itk::SmartPointer<Self>        Pointer;       \
    igstkTypeMacro( surrogate, type );   \
    igstkNewMacro( Self );      \
  };    \




int main( int argc, char * argv [] )
{

  if( argc < 2 )
    {
    std::cerr << "ERROR: Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  outputDirectory  skipLoopsBoolean" << std::endl;
    return 1;
    }

  std::string outputDirectory;
  outputDirectory = argv[1];

  std::cout << "Output directory = " << outputDirectory << std::endl;
  
  bool skipLoops = true;

  if( argc > 2 )
    {
    skipLoops = atoi( argv[2] );
    }

  // This is for classes that use SmartPointers
  igstkTestExportStateMachine1( igstk::LandmarkRegistration<3>, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageReader, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageReader, outputDirectory, skipLoops );


  return EXIT_SUCCESS;
}

