/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTests.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// this file defines the igstkTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 
#include "igstkSystemInformation.h"


void RegisterTests()
{
#ifdef IGSTK_TEST_AURORA_ATTACHED
  REGISTER_TEST(igstkAuroraTrackerTest);
#endif
  REGISTER_TEST(igstkAuroraTrackerSimulatedTest);
  REGISTER_TEST(igstkBasicTrackerTest);
  REGISTER_TEST(igstkBinaryDataTest);
  REGISTER_TEST(igstkCommunicationTest);
  REGISTER_TEST(igstkCylinderObjectTest);
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
  REGISTER_TEST(igstkEllipsoidObjectTest);
  REGISTER_TEST(igstkFLTKTextBufferLogOutputTest);
  REGISTER_TEST(igstkFLTKTextLogOutputTest);
  REGISTER_TEST(igstkMeshObjectTest);
  REGISTER_TEST(igstkMultipleOutputTest);
#if defined( IGSTK_TEST_AURORA_ATTACHED )
  REGISTER_TEST(igstkNDICommandInterpreterTest);
#elif defined( IGSTK_TEST_POLARIS_ATTACHED )
  REGISTER_TEST(igstkNDICommandInterpreterTest);
#endif
  REGISTER_TEST(igstkNDICommandInterpreterSimulatedTest);
  REGISTER_TEST(igstkNDICommandInterpreterStressTest);
#ifdef IGSTK_TEST_POLARIS_ATTACHED
  REGISTER_TEST(igstkPolarisTrackerTest);
#endif
  REGISTER_TEST(igstkPolarisTrackerSimulatedTest);
  REGISTER_TEST(igstkPulseGeneratorTest);
  REGISTER_TEST(igstkSerialCommunicationTest);
  REGISTER_TEST(igstkSerialCommunicationSimulatorTest);
  REGISTER_TEST(igstkStateMachineTest);
  REGISTER_TEST(igstkStateMachineErrorsTest);
  REGISTER_TEST(igstkTimeStampTest);
  REGISTER_TEST(igstkTokenTest);
  REGISTER_TEST(igstkTubeObjectTest);
  REGISTER_TEST(igstkTrackerPortTest);
  REGISTER_TEST(igstkTrackerToolTest);
  REGISTER_TEST(igstkTransformTest);
  REGISTER_TEST(igstkViewTest);
  REGISTER_TEST(igstkViewRefreshRateTest);
  REGISTER_TEST(igstkVTKLoggerOutputTest);
}
