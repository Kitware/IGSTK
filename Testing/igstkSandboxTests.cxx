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
  REGISTER_TEST(igstkGroupObjectTest);
  REGISTER_TEST(igstkLandmarkUltrasoundCalibrationTest);
  REGISTER_TEST(igstkSocketCommunicationTest);
  REGISTER_TEST(igstkGenericImageSpatialObjectTest);
  REGISTER_TEST(igstkDICOMGenericImageReaderTest);
  REGISTER_TEST(igstkPivotCalibrationAlgorithmTest);
  REGISTER_TEST(igstkVascularNetworkObjectTest);
#ifdef IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS
  REGISTER_TEST(igstkTrackerToolObserverToSocketRelayTest);
  REGISTER_TEST(igstkPolarisTrackerToolObserverToSocketRelayTest );
  REGISTER_TEST(igstkVicraTrackerToolObserverToSocketRelayTest );
  REGISTER_TEST(igstkTransformSocketListenerTest );
#endif

#ifdef IGSTKSandbox_USE_FLTK
  REGISTER_TEST(igstkObliqueImageSpatialObjectRepresentationTest);
#endif /* IGSTKSandbox_USE_FLTK */

#ifdef IGSTK_TEST_POLARIS_ATTACHED
#endif


}
