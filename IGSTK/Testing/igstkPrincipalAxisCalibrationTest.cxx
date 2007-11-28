/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPrincipalAxisCalibrationTest.cxx
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

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkSystemInformation.h"
#include "igstkPrincipalAxisCalibration.h"

int igstkPrincipalAxisCalibrationTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  // Define type used in the principal axis calibration class
  typedef igstk::PrincipalAxisCalibration   PrincipalAxisCalibrationType;

  typedef PrincipalAxisCalibrationType::VectorType          VectorType;
  typedef PrincipalAxisCalibrationType::CovariantVectorType CovariantVectorType;
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput                           LogOutputType;

  // Logger object created for logging calibration computation
  LoggerType::Pointer                            logger = LoggerType::New();
  LogOutputType::Pointer                   logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create the principal axis calibration object and attach the logger
  PrincipalAxisCalibrationType::Pointer principal
                                        = PrincipalAxisCalibrationType::New();
  principal->SetLogger( logger );

  // Reset the principal axis calibration class
  principal->RequestReset();

  // Define the input paramters
  VectorType          axis;
  CovariantVectorType normal;

  // Set the initial orientation of the tool
  axis[0] = 0.0;
  axis[1] = 1.0;
  axis[2] = 0.0;
  normal[0] = 0.0;
  normal[1] = 0.0;
  normal[2] = 1.0;
  principal->RequestSetInitialOrientation( axis, normal );

  // Set the desired orientation of the tool, normal is a bit 
  // shifted for adjust test
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;
  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.3;
  principal->RequestSetDesiredOrientation( axis, normal );

  // Calculate the rotation matrix
  principal->RequestCalculateRotation();

  if ( !principal->GetValidRotation())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
    // Dump the self class information
    std::cout << "Initial Principal Axis:"
              << principal->GetInitialPrincipalAxis() << std::endl;
    std::cout << "Initial Plane Normal:"
              << principal->GetInitialPlaneNormal() << std::endl;
    std::cout << "Desired Principal Axis:"
              << principal->GetDesiredPrincipalAxis() << std::endl;
    std::cout << "Desired Plane Normal:"
              << principal->GetDesiredPlaneNormal() << std::endl;
    std::cout << "Calibration Transform:"
              << principal->GetCalibrationTransform() << std::endl;

    principal->Print( std::cout);

    }

  // Simulate NoProcessing test
  principal->RequestCalculateRotation();

  return EXIT_SUCCESS;

}
