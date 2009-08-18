/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTrackerToolTest.cxx
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

#include "igstkVTKLoggerOutput.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkInfiniTrackTrackerTool.h"

int igstkInfiniTrackTrackerToolTest( int argc, char * argv[]  )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

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


  typedef igstk::InfiniTrackTrackerTool      TrackerToolType;
  typedef TrackerToolType::TransformType     TransformType;
    
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->SetLogger( logger );
  std::string markerNameTT = "ISTB0025";
 
  trackerTool->RequestSetMarkerName( markerNameTT );  
 
  trackerTool->RequestConfigure();

  std::cout << trackerTool << std::endl;

  return EXIT_SUCCESS;
}
