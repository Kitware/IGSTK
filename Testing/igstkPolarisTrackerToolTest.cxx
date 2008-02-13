/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerToolTest.cxx
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

#include "igstkPolarisTrackerTool.h"


int igstkPolarisTrackerToolTest( int argc, char * argv[] )
{
  
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  if( argc < 2 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Type( 0 for Wireless, 1 for Wired ) \t"
                            << "Port Number\t"
                            << "[SROM file]\t"
                            << "[tool Id]\t" << std::endl;
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

  typedef igstk::PolarisTrackerTool      TrackerToolType;
  typedef TrackerToolType::TransformType TransformType;
    
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->SetLogger( logger );
 
  // Select tracker tool to be wired or wireless
  if ( argv[0] ) 
    {
    trackerTool->RequestSelectWirelessTrackerTool();
    }
  else
    {
    trackerTool->RequestSelectWiredTrackerTool();
    }

  //try setting invalid port number ( the maximum is 11 )
  unsigned int invalidPortNumber = 255;
  trackerTool->RequestSetPortNumber( invalidPortNumber );

  // Set the port number 
  std::istringstream inputstream( argv[1] );
  
  unsigned int portNumber; 

  inputstream >> portNumber;

  trackerTool->RequestSetPortNumber( portNumber );


  // Set the SROM filename if specified 
  if ( argc >= 3 )
    {
    std::string sromFile = argv[2];
    trackerTool->RequestSetSROMFileName( sromFile );
    }
  else
    {
    // set invalid SROM filename for testing
    std::string invalidSROMFile = "";
    trackerTool->RequestSetSROMFileName( invalidSROMFile );
    }

  // set a tool part number if specified
  if ( argc >= 4 )
    {
    std::string  partNumber = argv[3];
    trackerTool->RequestSetPartNumber( partNumber );
    }
  else
    {
    std::string invalidPartNumber = "";
    trackerTool->RequestSetPartNumber( invalidPartNumber );

    std::string validPartNumber = "NDI-112-11";
    trackerTool->RequestSetPartNumber( validPartNumber );
    }

  trackerTool->RequestConfigure();

  trackerTool->Print( std::cout );
  return EXIT_SUCCESS;
}
