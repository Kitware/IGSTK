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
  int i, j;
  int frame;
  std::string temp;
  double time;
  double quat[4];
  double matrix[3][3];
  VectorType pos, pivotpos;
  VersorType quaternion;

  // Open the calibration data file, which recorded the traker information
  std::string igstkDataDirectory = IGSTKSandbox_DATA_ROOT;
  std::string simulationFile = ( igstkDataDirectory + "/" + "PivotCalibration.txt");
  input.open( simulationFile.c_str() );

  if (input.is_open() == 1)
    {
    std::cout << "PivtoCalibration data open sucessully!" << std::endl;
    }
  else
    {
    std::cout << "PivtoCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }

  // Reset the pivot calibration class
  pivot->RequestReset();

  // Input the frame data into the calibration class
  while ( !input.eof())
    {
    input >> frame >> temp >> time;
    input >> pos[0] >> pos[1] >> pos[2];
    input >> quat[0] >> quat[1] >> quat[2] >> quat[3];

    quaternion.Set( quat[1], quat[2], quat[3], quat[0]);
    pivot->RequestAddRotationTranslation( quaternion, pos );
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
      if (pivot->RequestGetInputRotationTranslation( i, quaternion, pos))
        {
        std::cout << "Input Sample: " << i << " " << quaternion << pos << std::endl;

        pivotpos = pivot->RequestSimulatePivotPosition( quaternion, pos);
        std::cout << "SimulatedPivotPosition: " << pivotpos << std::endl;
        }
      else
        {
        std::cout << "Invalid input Sample: " << i << std::endl;
        }
      }

    // Simulate for bad index
    i = -1;
    if (pivot->RequestGetInputRotationTranslation( i, quaternion, pos))
      {
      std::cout << "Input Sample: " << i << " " << quaternion << pos << std::endl;
      }
    else
      {
      std::cout << "Invalid input Sample: " << i << std::endl;
      }

    i = pivot->GetNumberOfSamples();
    if (pivot->RequestGetInputRotationTranslation( i, quaternion, pos))
      {
      std::cout << "Input Sample: " << i << " " << quaternion << pos << std::endl;
      }
    else
      {
      std::cout << "Invalid input Sample: " << i << std::endl;
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

  // Simulate the manually setting calibration matrix
  pivot->RequestSetTranslation( 1.0, 2.0, 10.0);
  pivot->RequestSetQuaternion( 1.0, 0.0, 0.0, 1.0);
  pivot->Print( std::cout);

  // Simulate the manually setting rotation matrix
  for (j = 0; j < 3; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      matrix[j][i] = (rand() % 1000) / 1000.0;
      }
    }
  pivot->RequestSetRotationMatrix( matrix);
  pivot->Print( std::cout);

  // Simulate the manually setting principal axis and normal
  pivot->RequestSetToolPrincipalAxis( 0.0, 0.0, 1.0);
  pivot->RequestSetToolPlaneNormal( 0.0, 1.0, 1.0);
  pivot->Print( std::cout);

  return EXIT_SUCCESS;

}


