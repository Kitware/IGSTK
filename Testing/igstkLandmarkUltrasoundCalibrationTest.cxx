/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkUltrasoundCalibrationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters in the debug 
//  information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkPivotCalibration.h"
#include "igstkLandmarkUltrasoundCalibration.h"

int igstkLandmarkUltrasoundCalibrationTest( int, char * [] )
{
  // Define type used in the pivot calibration class
  typedef igstk::LandmarkUltrasoundCalibration             LandmarkUltrasoundCalibrationType;
  typedef LandmarkUltrasoundCalibrationType::VersorType    VersorType;
  typedef LandmarkUltrasoundCalibrationType::VectorType    VectorType;
  typedef LandmarkUltrasoundCalibrationType::PointType     PointType;
  typedef LandmarkUltrasoundCalibrationType::IndexType     IndexType;
  typedef LandmarkUltrasoundCalibrationType::TransformType TransformType;
  typedef LandmarkUltrasoundCalibrationType::SpacingType   SpacingType;
  typedef LandmarkUltrasoundCalibrationType::ErrorType     ErrorType;
  typedef itk::Logger                                     LoggerType; 
  typedef itk::StdStreamLogOutput                         LogOutputType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the pivot calibration object and attach the logger
  LandmarkUltrasoundCalibrationType::Pointer ultrasound = LandmarkUltrasoundCalibrationType::New();
  ultrasound->SetLogger( logger );

  // Define the input file and the variables to extract the rotation and translation information
  unsigned int i, j;
  std::ifstream input;
  
  // Input
  IndexType indexposition;
  PointType imageposition;
  VersorType pointerversor, probeversor;
  VectorType pointertranslation, probetranslation, tooltranslation;
  TransformType toolcalibration;

  std::vector< IndexType > indexpositioncontainer;
  std::vector< VersorType > pointerversorcontainer;
  std::vector< VectorType > pointertranslationcontainer;
  std::vector< VersorType > probeversorcontainer;
  std::vector< VectorType > probetranslationcontainer;

  // Output
  TransformType transform;
  SpacingType   scale;
  ErrorType     error;
  int           num;
  
  // Open the calibration data file, which recorded the traker information
  std::string igstkDataDirectory = IGSTKSandbox_DATA_ROOT;
  std::string simulationFile = ( igstkDataDirectory + "/Input/USCalibration.txt");
  input.open( simulationFile.c_str() );

  if (input.is_open() == 1)
    {
    std::cout << "UltrasoundCalibration data open sucessully!" << std::endl;
    }
  else
    {
    std::cout << "UltrasoundCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }

  // Input the frame data into the calibration class
  while ( !input.eof())
    {
    double vx;
    double vy;
    double vz;
    double vw;

    input >> indexposition[0] >> indexposition[1];
    indexposition[2] = 0.0;
    indexpositioncontainer.push_back( indexposition);

    input >> pointertranslation[0] >> pointertranslation[1] >> pointertranslation[2];
    pointertranslationcontainer.push_back( pointertranslation);
    
    input >> vw >> vx >> vy >> vz;
    pointerversor.Set( vx, vy, vz, vw);
    pointerversorcontainer.push_back( pointerversor);

    input >> probetranslation[0] >> probetranslation[1] >> probetranslation[2];
    probetranslationcontainer.push_back( probetranslation);

    input >> vw >> vx >> vy >> vz;
    probeversor.Set( vx, vy, vz, vw);
    probeversorcontainer.push_back( probeversor);
    }

  // Set the pointer tool's calibration matrix
  tooltranslation[0] = 0.0;
  tooltranslation[1] = 0.0;
  tooltranslation[2] = -9.5;
  toolcalibration.SetTranslation( tooltranslation, 0.1, 1000 );

  // Test for routine 1: index input && pointer tool input
  ultrasound->RequestReset();
  ultrasound->RequestSetPointerToolCalibrationTransform( toolcalibration);
  for (i = 0; i < indexpositioncontainer.size(); i++)
    {
    ultrasound->RequestAddPointerToolIndexPositionSample( indexpositioncontainer[i], 
      pointerversorcontainer[i], pointertranslationcontainer[i], 
      probeversorcontainer[i], probetranslationcontainer[i] );
    }
  ultrasound->RequestCalculateCalibration();

  if ( !ultrasound->GetValidLandmarkUltrasoundCalibration())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
    // Dump the self class information
    transform = ultrasound->GetCalibrationTransform();
    error = ultrasound->GetRootMeanSquareError();
    num = ultrasound->GetNumberOfSamples();
    scale = ultrasound->GetScaleTransform();

    ultrasound->Print( std::cout);
    }

  // Test for routine 2: image input && pointer tool input
  ultrasound->RequestReset();
  ultrasound->RequestSetPointerToolCalibrationTransform( toolcalibration);
  for (i = 0; i < indexpositioncontainer.size(); i++)
    {
    indexposition = indexpositioncontainer[i];
    for (j = 0; j < 3; j++)
      {
      imageposition[j] = indexposition[j] * scale[j];
      }
    ultrasound->RequestAddPointerToolImagePositionSample( imageposition, 
      pointerversorcontainer[i], pointertranslationcontainer[i], 
      probeversorcontainer[i], probetranslationcontainer[i] );
    }
  ultrasound->RequestCalculateCalibration();

  if ( !ultrasound->GetValidLandmarkUltrasoundCalibration())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
    // Dump the self class information
    transform = ultrasound->GetCalibrationTransform();
    error = ultrasound->GetRootMeanSquareError();
    num = ultrasound->GetNumberOfSamples();

    ultrasound->Print( std::cout);
    }

  return EXIT_SUCCESS;

}

