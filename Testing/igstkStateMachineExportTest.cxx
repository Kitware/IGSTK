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

#include "igstkCylinderObjectRepresentation.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkObjectRepresentation.h"
#include "igstkScene.h"
#include "igstkSpatialObject.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkView.h"

namespace igstk 
{
  
template<class TType>
class StateMachineDescription
{
  typedef TType ClassType;
public:
  static void Export( const std::string & outputDirectory )
  {
  typename ClassType::Pointer instance = ClassType::New();
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
};

} // end namespace igstk

int main( int argc, char * argv [] )
{
  std::string outputDirectory;
  if( argc > 1 )
    {
    outputDirectory = argv[1];
    }

  std::cout << "Output directory = " << outputDirectory << std::endl;

  igstk::StateMachineDescription< igstk::SpatialObject >::Export( outputDirectory );


  return EXIT_SUCCESS;
}

