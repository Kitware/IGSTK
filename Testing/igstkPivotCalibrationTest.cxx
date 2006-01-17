/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkPivotCalibration.h"

int igstkPivotCalibrationTest( int, char * [] )
{
  // Define type used in the pivot calibration class
  typedef igstk::PivotCalibration           PivotCalibrationType;
  typedef PivotCalibrationType::VersorType  VersorType;
  typedef PivotCalibrationType::VectorType  VectorType;
  typedef PivotCalibrationType::PointType   PointType;
  typedef PivotCalibrationType::MatrixType  MatrixType;
  typedef PivotCalibrationType::ErrorType   ErrorType;
  typedef itk::Logger                       LoggerType; 
  typedef itk::StdStreamLogOutput           LogOutputType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the pivot calibration object and attach the logger
  PivotCalibrationType::Pointer pivot = PivotCalibrationType::New();
  pivot->SetLogger( logger );

  // Define the input file and the variables to extract the rotation and translation information
  std::ifstream input;
  unsigned int i;
  int frame, index;
  std::string temp;
  double time;

  MatrixType matrix;
  VectorType pos;
  PointType pivotpos;
  VersorType versor;

  // Open the calibration data file, which recorded the traker information
  std::string igstkDataDirectory = IGSTKSandbox_DATA_ROOT;
  std::string simulationFile = ( igstkDataDirectory + "/" + "PivotCalibration.txt");
  input.open( simulationFile.c_str() );

  if (input.is_open() == 1)
    {
    std::cout << "PivotCalibration data open sucessully!" << std::endl;
    }
  else
    {
    std::cout << "PivotCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }

  // Reset the pivot calibration class
  pivot->RequestReset();

  // Input the frame data into the calibration class
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

  // Calculate the calibration matrix along three axis
  pivot->RequestCalculateCalibration();

  if ( !pivot->GetValidPivotCalibration())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
    // Test the simulated pivot position
    for ( i = 0; i < pivot->GetNumberOfSamples(); i++)
      {
      if (pivot->RequestGetInputSample( i, versor, pos))
        {
        std::cout << "Input Sample: " << i << " " << versor << pos << std::endl;

        pivotpos = pivot->RequestSimulatePivotPosition( versor, pos);
        std::cout << "SimulatedPivotPosition: " << pivotpos << std::endl;
        }
      else
        {
        std::cout << "Invalid input Sample: " << i << std::endl;
        }
      }

    // Simulate for bad index
    index = -1;
    if (pivot->RequestGetInputSample( index, versor, pos))
      {
      std::cout << "Input Sample: " << index << " " << versor << pos << std::endl;
      }
    else
      {
      std::cout << "Invalid input Sample: " << index << std::endl;
      }

    index = pivot->GetNumberOfSamples();
    if (pivot->RequestGetInputSample( index, versor, pos))
      {
      std::cout << "Input Sample: " << index << " " << versor << pos << std::endl;
      }
    else
      {
      std::cout << "Invalid input Sample: " << index << std::endl;
      }

    // Get the calibration transformation
    VectorType translation = pivot->GetCalibrationTransform().GetTranslation();

    // Get the pivot focus position
    VectorType position = pivot->GetPivotPosition();

    // Get the calibration RMS error
    ErrorType error = pivot->GetRMS();

    // Dump the calibration class information
    std::cout << "PivotCalibration: " << std::endl;
    std::cout << "NumberOfSamples: " << pivot->GetNumberOfSamples() << std::endl;
    std::cout << "Translation: " << translation << std::endl;
    std::cout << "Pivot Position: " << position << std::endl;
    std::cout << "Calibration RMS: " << error << std::endl;

    // Dump the self class information
    pivot->Print( std::cout);

    }

  // Calculate the calibration matrix along z-axis
  pivot->RequestCalculateCalibrationZ();

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
    VectorType position = pivot->GetPivotPosition();

    // Get the calibration RMS error
    ErrorType error = pivot->GetRMS();

    // Dump the calibration class information
    std::cout << "PivotCalibration: " << std::endl;
    std::cout << "NumberOfSamples: " << pivot->GetNumberOfSamples() << std::endl;
    std::cout << "Translation: " << translation << std::endl;
    std::cout << "Pivot Position: " << position << std::endl;
    std::cout << "Calibration RMS: " << error << std::endl;

    // Dump the self class information
    pivot->Print( std::cout);

    }

  // Test the NoProcessing function
  pivot->RequestCalculateCalibrationZ();

  return EXIT_SUCCESS;

}


