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
  REGISTER_TEST(igstkSerialCommunicationTest);
  REGISTER_TEST(igstkNDICommandInterpreterTest);
  REGISTER_TEST(igstkAuroraTrackerTest);
  REGISTER_TEST(igstkPolarisTrackerTest);

  REGISTER_TEST(igstkVTKLoggerOutputTest);
  REGISTER_TEST(igstkFLTKTextBufferLogOutputTest);
  REGISTER_TEST(igstkFLTKTextLogOutputTest);

  REGISTER_TEST(igstkSerialCommunicationSimulatorTest);
  REGISTER_TEST(igstkTubeObjectTest);
  REGISTER_TEST(igstkMeshObjectTest);
  REGISTER_TEST(igstkBinaryDataTest);

#ifdef IGSTK_USE_ATAMAI
  REGISTER_TEST(igstkAtamaiNDITrackerTest);
#endif /* IGSTK_USE_ATAMAI */

}
