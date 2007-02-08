/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PrincipalAxisCalibration1.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
// This example illustrates how to use IGSTK's principal axis calibration 
// class to determine the rotation matrix for the tracker tools. 
//
// EndLatex

#include <iostream>
#include <fstream>

// BeginLatex
// 
// To use the principal axis calibration component, the header file for 
// \doxygen{PrincipalAxisCalibration} should be added:
//
// EndLatex

// BeginCodeSnippet
#include "igstkPrincipalAxisCalibration.h"
// EndCodeSnippet

#include "itkLogger.h"
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

  igstk::RealTimeClock::Initialize();

// BeginLatex
//
// All the necessary data types in the principal axis calibration are defined.
// VectorType and CovariantVectorType are used to represent the vectors 
// along the principal axis and the plane normal, as follow:
//
// EndLatex

// BeginCodeSnippet
  typedef igstk::PrincipalAxisCalibration   PrincipalAxisCalibrationType;

  typedef PrincipalAxisCalibrationType::VectorType          VectorType;
  typedef PrincipalAxisCalibrationType::CovariantVectorType CovariantVectorType;
  typedef itk::Logger                                       LoggerType; 
  typedef itk::StdStreamLogOutput                           LogOutputType;
// EndCodeSnippet

// BeginLatex
// 
// At the beginning, a principal axis calibration class is 
// initialized as follows: 
//
// EndLatex

// BeginCodeSnippet
  PrincipalAxisCalibrationType::Pointer principal
                                        = PrincipalAxisCalibrationType::New();
// EndCodeSnippet

// BeginLatex
// 
// A logger is then created for logging the process of calibration 
// computation and attached to the principal axis calibration class, as follows:
//
// EndLatex

// BeginCodeSnippet
  LoggerType::Pointer                            logger = LoggerType::New();
  LogOutputType::Pointer                   logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  principal->SetLogger( logger );
// EndCodeSnippet

// BeginLatex
// 
// Before the computation, the calibration class must be reset 
// to remove any information that may come from previous
// computation:
//
// EndLatex

// BeginCodeSnippet
  principal->RequestReset();
// EndCodeSnippet

// BeginLatex
// 
// Some parameters, such as axis and normal, are defined to store 
// the input information to determine the initial and desired orientations:
//
// EndLatex

// BeginCodeSnippet
  VectorType          axis;
  CovariantVectorType normal;
// EndCodeSnippet

// BeginLatex
// 
// An initial orientation is given as the default for the tracker tools:
//
// EndLatex

// BeginCodeSnippet
  axis[0] = 0.0;
  axis[1] = 1.0;
  axis[2] = 0.0;
  normal[0] = 0.0;
  normal[1] = 0.0;
  normal[2] = 1.0;
  principal->RequestSetInitialOrientation( axis, normal );
// EndCodeSnippet

// BeginLatex
// 
// The desired orientation of the tracker tools is also specified:
//
// EndLatex

// BeginCodeSnippet
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;
  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.3;
  principal->RequestSetDesiredOrientation( axis, normal );
// EndCodeSnippet

// BeginLatex
// 
// Then, a \code{RequestCalculateRotation} function is invoked to compute 
// the final results:
//
// EndLatex

// BeginCodeSnippet
  principal->RequestCalculateRotation();
// EndCodeSnippet

// BeginLatex
// 
// Before the final calibration transform is retrieved, the user should
// check the tag to see whether a valid calibration has been computed.
// The sample code is as follows:
//
// EndLatex


// BeginCodeSnippet
  if ( !principal->GetValidRotation())
    {
    std::cout << "No valid calibration!" << std::endl;

    return EXIT_FAILURE;
    }
  else
    {
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
// EndCodeSnippet

  return EXIT_SUCCESS;

}
