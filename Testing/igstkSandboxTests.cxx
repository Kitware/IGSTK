// this file defines the igstkSandBoxTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 
#include "igstkSandboxConfigure.h"

void RegisterTests()
{
  REGISTER_TEST(igstkMouseTrackerTest);
  REGISTER_TEST(igstkPolarisTrackerTest);
  REGISTER_TEST(igstkLandmarkRegistrationTest);
  REGISTER_TEST(igstkDICOMImageReaderTest);
  REGISTER_TEST(igstkCTImageReaderTest);

#ifdef IGSTK_USE_ATAMAI
  REGISTER_TEST(igstkAtamaiNDITrackerTest);
#endif /* IGSTK_USE_ATAMAI */

}
