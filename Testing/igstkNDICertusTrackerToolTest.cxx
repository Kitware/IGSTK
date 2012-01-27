/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusToolTest.cxx
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
#include <sstream>

#include "igstkVTKLoggerOutput.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkNDICertusTrackerTool.h"


int igstkNDICertusTrackerToolTest( int argc, char * argv[] )
{
  
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  if( argc < 2 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "[RIG filename]" << std::endl;
    return EXIT_FAILURE;
    }

  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                                = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger

  typedef igstk::NDICertusTrackerTool      TrackerToolType;
  typedef TrackerToolType::TransformType TransformType;
    
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->SetLogger( logger );
 

    // Set the RIG filename if specified 
    std::string rigidBodyFilename = argv[1];
    trackerTool->RequestSetRigidBodyName( rigidBodyFilename );

	// set invalid SROM filename for testing
    std::string invalidRigidBodyFilename = "";
    trackerTool->RequestSetRigidBodyName( invalidRigidBodyFilename );


  

  trackerTool->RequestConfigure();

  trackerTool->Print( std::cout );
  return EXIT_SUCCESS;
}
