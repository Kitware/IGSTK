/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PivotCalibrationReader1.cxx
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
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
// 
// This example illustrates how to use IGSTK's pivot calibration 
// reader class to read the calibration matrix from an off-line
// calibration file. 
//
// EndLatex

#include <iostream>
#include <fstream>

// BeginLatex
// 
// To use the pivot calibration reader component, the header file for 
// \doxygen{PivotCalibrationReader} should be added:
//
// EndLatex

// BeginCodeSnippet
#include "igstkPivotCalibrationReader.h"
// EndCodeSnippet

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

// BeginLatex
// 
// At the very beginning of the program, two kinds of events and observers are 
// defined to track the information from the reader. The first event is 
// \code{igstk::CalibrationModifiedEvent}, which retrieves the calibration 
// class from the reader. The second one is \code{igstk::StringEvent}, which  
// retrieves some string-like information from the general calibration class.
// The commands are as follows:
//
// EndLatex

// BeginCodeSnippet
namespace ToolCalibrationTest
{
igstkObserverMacro(Calibration,::igstk::CalibrationModifiedEvent,
                                           ::igstk::PivotCalibration::Pointer)
igstkObserverMacro(String,::igstk::StringEvent,std::string)
}
// EndCodeSnippet

// BeginLatex
// 
// After defining the headers, the main function implementation 
// is started:
//
// EndLatex

// BeginCodeSnippet
int main( int argc, char * argv[] )
{
// EndCodeSnippet

  if(argc<3)
    {
    std::cout << "Usage = " << argv[0] << " calibrationFile" 
              << " calibrationFileCorrupted" << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::Object::LoggerType         LoggerType;
  typedef itk::StdStreamLogOutput           LogOutputType;

// BeginLatex
// 
// A pivot calibration reader is created and then a logger is attached:
//
// EndLatex

// BeginCodeSnippet
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the pivot calibration reader and attach the logger
  igstk::PivotCalibrationReader::Pointer reader = 
                              igstk::PivotCalibrationReader::New();

  reader->SetLogger( logger );
// EndCodeSnippet

// BeginLatex
// 
// The pivot calibration file's name is passed through the argument.
// After the filename is designated, a \code{RequestReadObject} function is 
// invoked to parse the data file. The information in the reader can be easily
// dumped by the the default Print function, as follows:
//
// EndLatex

// BeginCodeSnippet
  reader->RequestSetFileName(argv[1]);
  reader->RequestReadObject();

  reader->Print(std::cout);
// EndCodeSnippet

// BeginLatex
// 
// To retrieve all calibration data information, the previous defined observer
// is attached to the reader class.  After the \code{RequestGetCalibration}
// function is called, the calibration  information is passed by the observer's
// \code{GetCalibration} function. The sample code is as follows:
//
// EndLatex

// BeginCodeSnippet
  typedef ToolCalibrationTest::CalibrationObserver CalibrationObserverType;
  CalibrationObserverType::Pointer calibrationObserver 
                                              = CalibrationObserverType::New();

  reader->AddObserver(::igstk::CalibrationModifiedEvent(),calibrationObserver);
  reader->RequestGetCalibration();

  igstk::PivotCalibration::Pointer calibration = NULL;

  std::cout << "Testing Calibration: ";
  if( calibrationObserver->GotCalibration() )
    {
    calibration = calibrationObserver->GetCalibration();
    }
  else
    {
    std::cout << "No calibration!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;
// EndCodeSnippet

// BeginLatex
//
// To retrieve specific information from the trackers, like serial number and 
// manufacturer, another string event observer is attached 
// to the calibration class. For each request, the information
// content will be passed by the observer's \code{GetString} function. 
// Some sample codes are as follows:
//
// EndLatex

// BeginCodeSnippet
  typedef ToolCalibrationTest::StringObserver StringObserverType;
  StringObserverType::Pointer stringObserver = StringObserverType::New();
  calibration->AddObserver( ::igstk::StringEvent(), stringObserver );
    
  std::cout << "Testing Date: ";
  calibration->RequestGetDate();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No date!" << std::endl;
    return EXIT_FAILURE;
    }
// EndCodeSnippet
  
 
  std::cout << "Testing Time: ";
  calibration->RequestGetTime();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No time!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Testing Tooltype: ";
  calibration->RequestGetToolType();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No tool type!" << std::endl;
    return EXIT_FAILURE;
    }

// BeginCodeSnippet
  std::cout << "Testing Manufacturer: ";
  calibration->RequestGetToolManufacturer();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No tool manufacturer!" << std::endl;
    return EXIT_FAILURE;
    }
// EndCodeSnippet

  std::cout << "Testing Part Number: ";
  calibration->RequestGetToolPartNumber();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No tool part number!" << std::endl;
    return EXIT_FAILURE;
    }

// BeginCodeSnippet
  std::cout << "Testing Serial Number: ";
  calibration->RequestGetToolSerialNumber();
  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString().c_str() << std::endl;
    std::cout << "[PASSED]" << std::endl;
    }
  else
    {
    std::cout << "No tool serial number!" << std::endl;
    return EXIT_FAILURE;
    }
// EndCodeSnippet

  std::cout << "[Test DONE]" << std::endl;
  return EXIT_SUCCESS;
}
