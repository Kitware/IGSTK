/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibrationReaderTest.cxx
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

#include <itksys/SystemTools.hxx>
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkPivotCalibration.h"
#include "igstkToolCalibrationReader.h"

int igstkToolCalibrationReaderTest( int argc, char * argv[] )
{
  if(argc<3)
    {
    std::cout << "Usage = " << argv[0] << " calibrationFile"
              << " calibrationFileCorrupted" << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::Object::LoggerType         LoggerType;
  typedef itk::StdStreamLogOutput           LogOutputType;
  typedef igstk::ToolCalibrationReader< igstk::PivotCalibration >
                                            ToolCalibrationReaderType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the specific tool calibration reader and attach the logger

  ToolCalibrationReaderType::Pointer reader =
                              ToolCalibrationReaderType::New();

  reader->SetLogger( logger );

  // Set an empty filename
  reader->RequestSetFileName("");
  reader->RequestSetFileName("This.file.does.not.exists");

  std::string dir = itksys::SystemTools::GetFilenamePath(argv[1]);
  reader->RequestSetFileName(dir.c_str());

  reader->RequestSetFileName(argv[2]);
  reader->RequestReadObject();

  reader->RequestSetFileName(argv[1]);
  reader->RequestReadObject();

  reader->Print(std::cout);

  std::cout << "[Test DONE]" << std::endl;
  return EXIT_SUCCESS;
}
