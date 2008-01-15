/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSandboxTests.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  REGISTER_TEST(igstkSpatialObjectCoordinateSystemTest);
  REGISTER_TEST(igstkCoordinateReferenceSystemTest);
  REGISTER_TEST(igstkCoordinateReferenceSystemTest2);
  REGISTER_TEST(igstkCoordinateReferenceSystemTest3);
  REGISTER_TEST(igstkCoordinateReferenceSystemDelegatorTest);
  REGISTER_TEST(igstkSpatialObjectCoordinateSystemTest3);
  REGISTER_TEST(igstkGroupObjectTest);
  REGISTER_TEST(igstkLandmarkUltrasoundCalibrationTest);
  REGISTER_TEST(igstkSocketCommunicationTest);
  REGISTER_TEST(igstkTransductionMacroTest);
  REGISTER_TEST(igstkGenericImageSpatialObjectTest);
  REGISTER_TEST(igstkDICOMGenericImageReaderTest);
  REGISTER_TEST(igstkPivotCalibrationAlgorithmTest);
  REGISTER_TEST(igstkVascularNetworkObjectTest);
  REGISTER_TEST(igstkObjectRepresentationRemovalTest);

#ifdef IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS
  REGISTER_TEST(igstkTrackerToolObserverToSocketRelayTest);
  REGISTER_TEST(igstkPolarisTrackerToolObserverToSocketRelayTest );
  REGISTER_TEST(igstkVicraTrackerToolObserverToSocketRelayTest );
  REGISTER_TEST(igstkTransformSocketListenerTest );
#endif

#ifdef IGSTKSandbox_USE_FLTK
  REGISTER_TEST(igstkCoordinateReferenceSystemObjectWithViewTest);
  REGISTER_TEST(igstkObliqueImageSpatialObjectRepresentationTest);
  REGISTER_TEST(igstkCTImageSpatialObjectReadingAndRepresentationTest2);
  REGISTER_TEST(igstkCircularSimulatedTrackerTest);
  REGISTER_TEST(igstkFLTKWidgetTest);
  REGISTER_TEST(igstkFLTKWidgetTest2);
  REGISTER_TEST(igstkAnnotation2DTest2);
  REGISTER_TEST(igstkMouseTrackerToolTest);
#endif /* IGSTKSandbox_USE_FLTK */

#ifdef IGSTKSandbox_USE_Qt
  REGISTER_TEST(igstkQTWidgetTest);
  REGISTER_TEST(igstkQTWidgetTest2);
  REGISTER_TEST(igstkCTImageSpatialObjectReadingAndRepresentationTest3);
#endif /* IGSTKSandbox_USE_Qt */

#ifdef IGSTKSandbox_USE_MicronTracker
  REGISTER_TEST(igstkMicronTrackerTest);
  REGISTER_TEST(igstkMicronTrackerToolTest);
#endif /* IGSTKSandbox_USE_MicronTracker */

#ifdef IGSTK_TEST_POLARIS_ATTACHED
  REGISTER_TEST(igstkPolarisTrackerTest2);
#endif



}
