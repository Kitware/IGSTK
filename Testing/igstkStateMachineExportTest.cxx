/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineExportTest.cxx
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

#include "igstkLandmarkUltrasoundCalibration.h"
#include "igstkSocketCommunication.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkObliqueImageSpatialObjectRepresentation.h"
#include "igstkSpatialObject.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkObjectRepresentation.h"
#include "igstkViewNew.h"
#include "igstkViewNew2D.h"
#include "igstkViewNew3D.h"
#include "igstkTrackerNew.h"
#include "igstkTrackerToolNew.h"
#include "igstkPolarisTrackerNew.h"
#include "igstkPolarisTrackerToolNew.h"
#include "igstkAuroraTrackerNew.h"
#include "igstkAuroraTrackerToolNew.h"

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
  igstk::ExportStateMachineDescription( instance.GetPointer(), \
  outputDirectory, skipLoops ); \
}

// This is for classes that do not use SmartPointers and 
// have a default constructor
#define igstkTestExportStateMachine2( type, outputDirectory, skipLoops ) \
{ \
  type * instance = new type; \
  igstk::ExportStateMachineDescription( instance, \
  outputDirectory, skipLoops ); \
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

namespace igstk
{
typedef ImageSpatialObject<float,3>                    ImageSpatialObjectType;
typedef ObliqueImageSpatialObjectRepresentation< ImageSpatialObjectType >  
                                  ObliqueImageSpatialObjectRepresentationType;

class ObjectRepresentationSurrogate : public ObjectRepresentation
{
public:
  typedef ObjectRepresentationSurrogate     Self;
  typedef ObjectRepresentation              Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  igstkTypeMacro( ObjectRepresentationSurrogate, ObjectRepresentation );
  igstkNewMacro( Self );
  virtual void CreateActors() {};
  virtual void UpdateRepresentationProcessing() {};

};
  
igstkDeclareSurrogateClass( SpatialObjectSurrogate, SpatialObject );
}

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

  std::cout << "Skip Loops option = " << skipLoops << std::endl;

  igstkTestExportStateMachine1( igstk::LandmarkUltrasoundCalibration, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::SocketCommunication, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( 
                          igstk::ObliqueImageSpatialObjectRepresentationType, 
                          outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::Tracker, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::TrackerTool, outputDirectory,skipLoops );

  igstkTestExportStateMachine1( igstk::TrackerNew, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::TrackerToolNew, outputDirectory,skipLoops );

  igstkTestExportStateMachine1( igstk::PolarisTrackerNew, outputDirectory,skipLoops );
  igstkTestExportStateMachine1( igstk::PolarisTrackerToolNew, outputDirectory,skipLoops );

  igstkTestExportStateMachine1( igstk::AuroraTrackerNew, outputDirectory,skipLoops );
  igstkTestExportStateMachine1( igstk::AuroraTrackerToolNew, outputDirectory,skipLoops );

  // Exporting Abstract classes by creating derived surrogates for them.
  igstkTestExportStateMachine1( igstk::SpatialObjectSurrogate, outputDirectory, 
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::ObjectRepresentationSurrogate, 
                                                   outputDirectory, skipLoops );
  // The View New classes
  igstkTestExportStateMachine1( igstk::ViewNew, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ViewNew2D, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ViewNew3D, outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::CoordinateReferenceSystem, outputDirectory, skipLoops );

  return EXIT_SUCCESS;
}
