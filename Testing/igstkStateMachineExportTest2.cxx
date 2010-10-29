/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkStateMachineExportTest2.cxx
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
#include "itkMacro.h"

#include "igstkLandmarkUltrasoundCalibration.h"
#include "igstkImageResliceObjectRepresentation.h"
#include "igstkSpatialObject.h"
#include "igstkObjectRepresentation.h"
#include "igstkSandboxConfigure.h"

#ifdef IGSTKSandbox_USE_vtkKWImage
#include "igstkGenericImageReader.h"
#endif

#ifdef IGSTKSandbox_USE_MicronTracker
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#endif

#ifdef IGSTKSandbox_USE_FLTK
#include "igstkFLTKWidget.h"
#endif

#ifdef IGSTKSandbox_USE_Qt
#include "igstkQTWidget.h"
#include <QApplication>
#include <QMainWindow>
#include <QtTest/QTest>
#endif

#include "igstkSocketCommunication.h"

namespace igstk 
{

typedef ImageSpatialObject<float,3>                 ImageSpatialObjectType;

typedef ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageResliceRepresentationType;
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

#ifdef IGSTKSandbox_USE_MicronTracker
  igstkTestExportStateMachine1( igstk::MicronTracker, 
                                outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::MicronTrackerTool, 
                                outputDirectory, skipLoops );
#endif

  // Exporting Abstract classes by creating derived surrogates for them.
  igstkTestExportStateMachine1( igstk::SpatialObjectSurrogate, outputDirectory, 
                                                                    skipLoops );
  igstkTestExportStateMachine1( igstk::ObjectRepresentationSurrogate, 
                                                   outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::CoordinateSystem, outputDirectory, skipLoops );

  igstkTestExportStateMachine1( igstk::ImageResliceRepresentationType, outputDirectory, skipLoops );

#ifdef IGSTKSandbox_USE_FLTK
  igstk::FLTKWidget fltkWidget(0,0, 100, 100, "Dummy FLTKWidget for testing");
  igstk::ExportStateMachineDescription( &fltkWidget, outputDirectory, skipLoops ); 
#endif /* IGSTKSandbox_USE_FLTK */

#ifdef IGSTKSandbox_USE_Qt
  QApplication app(argc, argv);
  QMainWindow  * qtMainWindow = new QMainWindow();
  qtMainWindow->setFixedSize(601,301);
  typedef igstk::QTWidget      QTWidgetType;
  QTWidgetType * qtWidget2D = new QTWidgetType();
  igstk::ExportStateMachineDescription( qtWidget2D, outputDirectory, skipLoops ); 
  delete qtWidget2D;
#endif /* IGSTKSandbox_USE_QT */

#ifdef IGSTKSandbox_USE_vtkKWImage
  igstkTestExportStateMachine1( igstk::GenericImageReader,
                                                 outputDirectory, skipLoops );
#endif

  return EXIT_SUCCESS;
}
