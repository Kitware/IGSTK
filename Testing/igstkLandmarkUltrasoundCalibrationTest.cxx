/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkUltrasoundCalibrationTest.cxx
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
#include "igstkLandmarkUltrasoundCalibration.h"

int igstkLandmarkUltrasoundCalibrationTest( int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputDirectory" << std::endl;
    return EXIT_FAILURE;
    }

  // Define type used in the landmark ultrasound calibration class
  typedef igstk::LandmarkUltrasoundCalibration
                                            LandmarkUltrasoundCalibrationType;


  typedef LandmarkUltrasoundCalibrationType::VersorType    VersorType;
  typedef LandmarkUltrasoundCalibrationType::VectorType    VectorType;
  typedef LandmarkUltrasoundCalibrationType::PointType     PointType;
  typedef LandmarkUltrasoundCalibrationType::IndexType     IndexType;
  typedef LandmarkUltrasoundCalibrationType::TransformType TransformType;
  typedef LandmarkUltrasoundCalibrationType::SpacingType   SpacingType;
  typedef LandmarkUltrasoundCalibrationType::ErrorType     ErrorType;
  typedef igstk::Object::LoggerType                        LoggerType;
  typedef itk::StdStreamLogOutput                          LogOutputType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the landmark ultrasound calibration object and attach the logger
  LandmarkUltrasoundCalibrationType::Pointer ultrasound 
                                   = LandmarkUltrasoundCalibrationType::New();
  ultrasound->SetLogger( logger );

  // Define the input file and the variables to extract 
  // the rotation and translation information
  unsigned int i, j;
  std::ifstream input1;
  std::ifstream input2;
  
  // Input
  IndexType indexposition;
  PointType imageposition, pointerposition;
  VersorType pointerversor, probeversor;
  VectorType pointertranslation, probetranslation, tooltranslation;
  TransformType toolcalibration;

  std::vector< IndexType > indexpositioncontainer1;
  std::vector< VersorType > pointerversorcontainer1;
  std::vector< VectorType > pointertranslationcontainer1;
  std::vector< VersorType > probeversorcontainer1;
  std::vector< VectorType > probetranslationcontainer1;

  std::vector< IndexType > indexpositioncontainer2;
  std::vector< PointType > pointerpositioncontainer2;
  std::vector< VersorType > probeversorcontainer2;
  std::vector< VectorType > probetranslationcontainer2;

  // Output
  TransformType transform;
  SpacingType   scale;
  ErrorType     error;
  int           num;
  
  // Open the calibration data file, which recorded the traker information
  std::string igstkDataDirectory = argv[1];
  std::string simulationFile1 = igstkDataDirectory+"/USCalibration1.txt";
  std::string simulationFile2 = igstkDataDirectory+"/USCalibration2.txt";
  
  input1.open( simulationFile1.c_str() );

  if (input1.is_open() == 1)
    {
    std::cout << "UltrasoundCalibration data open sucessully!" << std::endl;
    }
  else
    {
    std::cout << "UltrasoundCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }

  // Input the frame data into the calibration class
  while ( !input1.eof())
    {
    double vx;
    double vy;
    double vz;
    double vw;

    input1 >> indexposition[0] >> indexposition[1];
    indexposition[2] = 0;
    indexpositioncontainer1.push_back( indexposition);

    input1 >> pointertranslation[0] >> pointertranslation[1];
    input1 >> pointertranslation[2];
    pointertranslationcontainer1.push_back( pointertranslation);
    
    input1 >> vw >> vx >> vy >> vz;
    pointerversor.Set( vx, vy, vz, vw);
    pointerversorcontainer1.push_back( pointerversor);

    input1 >> probetranslation[0] >> probetranslation[1] >> probetranslation[2];
    probetranslationcontainer1.push_back( probetranslation);

    input1 >> vw >> vx >> vy >> vz;
    probeversor.Set( vx, vy, vz, vw);
    probeversorcontainer1.push_back( probeversor);
    }

  input1.close();

  input2.open( simulationFile2.c_str() );

  if (input2.is_open() == 1)
    {
    std::cout << "UltrasoundCalibration data open sucessully!" << std::endl;
    }
  else
    {
    std::cout << "UltrasoundCalibration data open error!" << std::endl;

    return EXIT_FAILURE;
    }

  // Input the frame data into the calibration class
  while ( !input2.eof())
    {
    double vx;
    double vy;
    double vz;
    double vw;

    input2 >> indexposition[0] >> indexposition[1];
    indexposition[2] = 0;
    indexpositioncontainer2.push_back( indexposition);

    input2 >> pointerposition[0] >> pointerposition[1];
    input2 >> pointerposition[2];
    pointerpositioncontainer2.push_back( pointerposition);
    
    input2 >> probetranslation[0] >> probetranslation[1] >> probetranslation[2];
    probetranslationcontainer2.push_back( probetranslation);

    input2 >> vw >> vx >> vy >> vz;
    probeversor.Set( vx, vy, vz, vw);
    probeversorcontainer2.push_back( probeversor);
    }

  input2.close();

  // Set the pointer tool's calibration matrix
  tooltranslation[0] = 0.0;
  tooltranslation[1] = 0.0;
  tooltranslation[2] = -9.5;
  toolcalibration.SetTranslation( tooltranslation, 0.1, 1e300 );

  // Test for routine 1: index input && pointer tool input
  ultrasound->RequestReset();
  ultrasound->RequestSetPointerToolCalibrationTransform( toolcalibration);
  for (i = 0; i < indexpositioncontainer1.size(); i++)
    {
    ultrasound->RequestAddPointerToolIndexPositionSample(
                                               indexpositioncontainer1[i],
                                               pointerversorcontainer1[i],
                                               pointertranslationcontainer1[i],
                                               probeversorcontainer1[i],
                                               probetranslationcontainer1[i] );
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
  for (i = 0; i < indexpositioncontainer1.size(); i++)
    {
    indexposition = indexpositioncontainer1[i];
    for (j = 0; j < 3; j++)
      {
      imageposition[j] = indexposition[j] * scale[j];
      }
    ultrasound->RequestAddPointerToolImagePositionSample( imageposition, 
      pointerversorcontainer1[i], pointertranslationcontainer1[i], 
      probeversorcontainer1[i], probetranslationcontainer1[i] );
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

  // Test for routine 3: index input && landmark position input
  ultrasound->RequestReset();
  for (i = 0; i < indexpositioncontainer2.size(); i++)
    {
    ultrasound->RequestAddIndexPositionSample(
                                               indexpositioncontainer2[i],
                                               pointerpositioncontainer2[i],
                                               probeversorcontainer2[i],
                                               probetranslationcontainer2[i] );
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

  // Test for routine 4: image input && landmark position input
  ultrasound->RequestReset();
  for (i = 0; i < indexpositioncontainer2.size(); i++)
    {
    indexposition = indexpositioncontainer2[i];
    for (j = 0; j < 3; j++)
      {
      imageposition[j] = indexposition[j] * scale[j];
      }
    ultrasound->RequestAddImagePositionSample( imageposition, 
      pointerpositioncontainer2[i],  
      probeversorcontainer2[i], probetranslationcontainer2[i] );
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

  // Test for NoProcessing
  ultrasound->RequestCalculateCalibration();

  return EXIT_SUCCESS;

}
