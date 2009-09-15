/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
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
// Warning about: identifier was truncated to '255' characters
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "VideoFrameGrabberAndViewerMicron.h"

int main(int argc, char** argv)
{

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
  {
    std::cerr << " Usage: " << argv[0] << "\t"
    << "MicronTracker_Camera_Calibration_file" << "\t"
    << "MicronTracker_initialization_file"  << "\t"
    << "Marker_template_directory " << std::endl;
    return EXIT_FAILURE;
  }

  std::string  CameraCalibrationFileDirectory = argv[1];
  std::string InitializationFile = argv[2];
  std::string markerTemplateDirectory = argv[3];

  igstk::VideoFrameGrabberAndViewerMicron * application = new igstk::VideoFrameGrabberAndViewerMicron;

  application->RequestInitialize(
      InitializationFile,
      CameraCalibrationFileDirectory,
      markerTemplateDirectory );

  while( !application->HasQuitted() )
  {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
  }

  application->mainWindow->hide();

  delete application;

  return EXIT_SUCCESS;
}
