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
#include "itkTestMain.h" 
#include "igstkSandboxConfigure.h"
#include "igstkSystemInformation.h"

void RegisterTests()
{
  REGISTER_TEST(igstkMouseTrackerTest);
#ifdef IGSTK_TEST_POLARIS_ATTACHED
  REGISTER_TEST(igstkPolarisTrackerTest);
#endif
  REGISTER_TEST(igstkPolarisTrackerSimulatedTest);
  REGISTER_TEST(igstkLandmarkRegistrationTest);
  REGISTER_TEST(igstkLandmark3DRegistrationTest);
  REGISTER_TEST(igstkDICOMImageReaderTest);
  REGISTER_TEST(igstkDICOMImageReaderErrorsTest);
  REGISTER_TEST(igstkImageReaderTest);
  REGISTER_TEST(igstkCTImageReaderTest);
  REGISTER_TEST(igstkCTImageSpatialObjectTest);
  REGISTER_TEST(igstkCTImageSpatialObjectReadingAndRepresentationTest);
  REGISTER_TEST(igstkMRImageReaderTest);
  REGISTER_TEST(igstkMRImageSpatialObjectTest);
  REGISTER_TEST(igstkImageSpatialObjectTest);
  REGISTER_TEST(igstkImageSpatialObjectRepresentationTest);
  REGISTER_TEST(igstkCTImageSpatialObjectRepresentationTest);
  REGISTER_TEST(igstkMRImageSpatialObjectRepresentationTest);
  REGISTER_TEST(igstkStringEventTest);
  REGISTER_TEST(igstkSpatialObjectReaderTest);

#ifdef IGSTK_USE_ATAMAI
  REGISTER_TEST(igstkAtamaiNDITrackerTest);
#endif /* IGSTK_USE_ATAMAI */

}
