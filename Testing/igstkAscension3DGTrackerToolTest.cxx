/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGTrackerToolTest.cxx
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

#include "igstkAscension3DGTrackerTool.h"

int igstkAscension3DGTrackerToolTest( int , char * []  )
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


  typedef igstk::Ascension3DGTrackerTool           TrackerToolType;
  typedef TrackerToolType::TransformType     TransformType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->SetLogger( logger );
  for (unsigned int i = 0; i< 8; i++)
    {
    trackerTool->RequestSetPortNumber( i );
    trackerTool->RequestConfigure();
    if ( trackerTool->GetPortNumber() != i )
      {
      std::cout << "Incorrect port number returned" << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << trackerTool << std::endl;

  return EXIT_SUCCESS;
}
