/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PivotCalibration1.cxx
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
// class to determine a calibration matrix for the tracker tools. 
//
// EndLatex

#include <iostream>
#include <fstream>

// BeginLatex
// 
// To use the pivot calibration component, the header file for 
// \doxygen{PivotCalibration} should be added, as follows:
//
// EndLatex

// BeginCodeSnippet
#include "igstkPivotCalibration.h"
// EndCodeSnippet

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

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

  if(argc<2)
    {
    std::cout << "Usage = " << argv[0] << " calibrationFile" 
              << std::endl;
    return EXIT_FAILURE;
    }

  igstk::RealTimeClock::Initialize();

// BeginLatex
//
// All the necessary data types in the pivot calibration are defined.
// VersorType and VectorType are used to represent the quaternion and
// translation inputs from the tracker; PointType is used to represent
// the position coordinate of the specific point; and ErrorType is used
// to represent the root mean square error. The commands are as follows:
// EndLatex

// BeginCodeSnippet
  typedef igstk::PivotCalibration           PivotCalibrationType;
  typedef PivotCalibrationType::VersorType  VersorType;
  typedef PivotCalibrationType::VectorType  VectorType;
  typedef PivotCalibrationType::PointType   PointType;
  typedef PivotCalibrationType::ErrorType   ErrorType;
  typedef igstk::Object::LoggerType         LoggerType;
  typedef itk::StdStreamLogOutput           LogOutputType;
// EndCodeSnippet

// BeginLatex
// 
// At the beginning, a pivot calibration class is initialized as follows: 
//
// EndLatex

// BeginCodeSnippet
  PivotCalibrationType::Pointer pivot = PivotCalibrationType::New();
// EndCodeSnippet

// BeginLatex
// 
// A logger is then created for logging the process of calibration 
// computation and attached to the pivot calibration class, as follows:
//
// EndLatex

// BeginCodeSnippet
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  pivot->SetLogger( logger );
// EndCodeSnippet

// BeginLatex
// 
// To use the pivot calibration class, some input samples from the 
// tracker should be provided. 
// Those samples come directly from tracker tools. In our example, 
// those samples are read from the record data file in the IGSTK 
// data directory, as follows:
//
// EndLatex
// Define the input file and the variables to extract the rotation 
// and translation information
  std::ifstream input;
  int frame;
  std::string temp;
  double time;

  VectorType pos;
  PointType pivotpos;
  VersorType versor;

// BeginCodeSnippet
  input.open( argv[1] );

  if (input.is_open() == 1)
    {
    std::cout << "PivotCalibration data open successfully!" << std::endl;
    }
  else
    {
    std::cout << "PivotCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }
// EndCodeSnippet


// BeginLatex
// 
// Before the computation, it is better to reset the calibration class 
// to remove any information which may come from previous operations:
//
// EndLatex

// BeginCodeSnippet
  pivot->RequestReset();
// EndCodeSnippet

// BeginLatex
// 
// Then, the sample frame is read from the data file and input 
// to the calibration class:
//
// EndLatex

// BeginCodeSnippet
  while ( !input.eof())
    {
    double vx;
    double vy;
    double vz;
    double vw;

    input >> frame >> temp >> time;
    input >> pos[0] >> pos[1] >> pos[2];
    input >> vw >> vx >> vy >> vz;

    versor.Set( vx, vy, vz, vw );
    pivot->RequestAddSample( versor, pos );
    }
// EndCodeSnippet

// BeginLatex
// 
// After this, a simple request will invoke the class to compute 
// the calibration transform:
//
// EndLatex

// BeginCodeSnippet
  pivot->RequestCalculateCalibration();
// EndCodeSnippet

// BeginLatex
// 
// Before the final calibration transform is retrieved, the user should
// check the tag to see whether a valid calibration has been computed.
// The final calibration result is stored in the translation factor in
// the transform matrix. The pivot position is also retrievable. 
// The sample code is as follows:
//
// EndLatex


// BeginCodeSnippet
  if ( !pivot->GetValidPivotCalibration())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
   
    // Get the calibration transformation
    VectorType translation = pivot->GetCalibrationTransform().GetTranslation();

    // Get the pivot focus position
    PointType position = pivot->GetPivotPosition();

    // Get the calibration RMS error
    ErrorType error = pivot->GetRootMeanSquareError();

    // Dump the calibration class information
    std::cout << "PivotCalibration: " << std::endl;
    std::cout << "NumberOfSamples: " << pivot->GetNumberOfSamples() 
                                     << std::endl;
    std::cout << "Translation: " << translation << std::endl;
    std::cout << "Pivot Position: " << position << std::endl;
    std::cout << "Calibration RMS: " << error << std::endl;

    }
// EndCodeSnippet

// BeginLatex
// 
// For computing only the calibration along the Z-axis, the following 
// function is used instead:
//
// EndLatex

// BeginCodeSnippet
  pivot->RequestCalculateCalibrationZ();
// EndCodeSnippet

  if ( !pivot->GetValidPivotCalibration())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
    // Get the calibration transformation
    VectorType translation = pivot->GetCalibrationTransform().GetTranslation();

    // Get the pivot focus position
    PointType position = pivot->GetPivotPosition();

    // Get the calibration RMS error
    ErrorType error = pivot->GetRootMeanSquareError();

    // Dump the calibration class information
    std::cout << "PivotCalibration: " << std::endl;
    std::cout << "NumberOfSamples: "
              << pivot->GetNumberOfSamples() << std::endl;
    std::cout << "Translation: " << translation << std::endl;
    std::cout << "Pivot Position: " << position << std::endl;
    std::cout << "Calibration RMS: " << error << std::endl;

    }

  return EXIT_SUCCESS;
}
