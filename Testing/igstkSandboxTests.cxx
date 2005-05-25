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
  REGISTER_TEST(igstkNDICyclicRedundancyTest);
  REGISTER_TEST(igstkAuroraTrackerTest);

  REGISTER_TEST(itkRealtimeClockTest);
  REGISTER_TEST(itkStdStreamLogOutputTest);
  REGISTER_TEST(itkMultipleLogOutputTest);
  REGISTER_TEST(itkLoggerTest);
  REGISTER_TEST(itkThreadLoggerTest);
  REGISTER_TEST(itkLoggerOutputTest);
  REGISTER_TEST(itkLoggerManagerTest);

  REGISTER_TEST(igstkVTKLoggerOutputTest);
  REGISTER_TEST(igstkFLTKTextBufferLogOutputTest);
  REGISTER_TEST(igstkFLTKTextLogOutputTest);

#ifdef IGSTK_USE_ATAMAI
  REGISTER_TEST(igstkAtamaiNDITrackerTest);
#endif /* IGSTK_USE_ATAMAI */

}
