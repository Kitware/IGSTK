/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSandboxTests.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// this file defines the igstkSandBoxTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "igstkTestMain.h" 
#include "igstkSandboxConfigure.h"
#include "igstkSystemInformation.h"

void RegisterTests()
{
  // Register Sandbox tests
  REGISTER_TEST(igstkObliqueImageSpatialObjectRepresentationTest);
  REGISTER_TEST(igstkLandmarkUltrasoundCalibrationTest);
  REGISTER_TEST(igstkFlockOfBirdsTrackerTest);
  REGISTER_TEST(igstkSocketCommunicationTest);
  REGISTER_TEST(igstkPivotCalibrationReaderTest);
  REGISTER_TEST(igstkMR3DImageToUS3DImageRegistrationTest);
  REGISTER_TEST(igstkImageSpatialObjectRepresentationTest2);
  REGISTER_TEST(igstkImageSpatialObjectRepresentationTest3);
  REGISTER_TEST(igstkVesselObjectTest);
  REGISTER_TEST(igstkVascularNetworkReaderTest);
  REGISTER_TEST(igstkUltrasoundImageSimulatorTest);
  REGISTER_TEST(igstkUSImageObjectTest);
  REGISTER_TEST(igstkUSImageObjectRepresentationTest);
  REGISTER_TEST(igstkUSImageReaderTest);
}
