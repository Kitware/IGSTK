/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibrationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters in the debug 
//  information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkToolCalibration.h"

int igstkToolCalibrationTest( int, char * [] )
{
  // Define type used in the calibration class
  typedef igstk::ToolCalibration         CalibrationType;

  typedef igstk::Object::LoggerType      LoggerType;
  typedef itk::StdStreamLogOutput        LogOutputType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                    logger = LoggerType::New();
  LogOutputType::Pointer                 logOutput = LogOutputType::New();

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the calibration object and attach the logger
  CalibrationType::Pointer calibration = CalibrationType::New();

  calibration->SetLogger( logger );


  // Test the normal use
  calibration->RequestSetTime("10:30");
  calibration->RequestSetDate("May 31 2006");
  calibration->RequestSetToolType("Passive");
  calibration->RequestSetToolManufacturer("ACME Inc.");
  calibration->RequestSetToolPartNumber("3141516");
  calibration->RequestSetToolSerialNumber("157");

  // Test error cases with empty strings
  calibration->RequestSetTime("");
  calibration->RequestSetDate("");
  calibration->RequestSetToolType("");
  calibration->RequestSetToolManufacturer("");
  calibration->RequestSetToolPartNumber("");
  calibration->RequestSetToolSerialNumber("");

  // Test the returns
  calibration->RequestGetTime();
  calibration->RequestGetDate();
  calibration->RequestGetToolType();
  calibration->RequestGetToolManufacturer();
  calibration->RequestGetToolPartNumber();
  calibration->RequestGetToolSerialNumber();
 

  return EXIT_SUCCESS;

}
