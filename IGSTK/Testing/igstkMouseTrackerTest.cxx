/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTest.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkMouseTracker.h"

int igstkMouseTrackerTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Transform         TransformType;
  typedef igstk::MouseTracker      MouseTrackerType;
  typedef itk::Logger              LoggerType; 
  typedef itk::StdStreamLogOutput  LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  MouseTrackerType::Pointer tracker = MouseTrackerType::New();
  tracker->SetLogger( logger );

  // Test SetScaleFactor()/GetScaleFactor() methods
  const double scale = 1.0;
  tracker->SetScaleFactor( scale );
  const double scaleReturned = tracker->GetScaleFactor();
  if( fabs( scale - scaleReturned ) > 1e-5 )
    {
    std::cerr << "Error in SetScaleFactor()/GetScaleFactor() " << std::endl;
    return EXIT_FAILURE;
    }

  tracker->RequestOpen();

  tracker->RequestInitialize();

  tracker->RequestStartTracking();

  TransformType transform;

  tracker->RequestUpdateStatus();

  tracker->GetTransform( transform );

  TransformType::VectorType position = transform.GetTranslation();
  
  std::cout << "Mouse Position -> ( " << position[0] << "," 
            << position[1] << "," << position[2] << ")" << std::endl;

  tracker->RequestReset();

  tracker->RequestStopTracking();

  std::cout << tracker << std::endl;
  std::cout << transform << std::endl;

  tracker->RequestClose();

  return EXIT_SUCCESS;
}
