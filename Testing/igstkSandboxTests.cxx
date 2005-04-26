// this file defines the igstkSandBoxTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
  REGISTER_TEST(igstkMouseTrackerTest);
  REGISTER_TEST(igstkSerialCommunicationTest);
  REGISTER_TEST(igstkAtamaiNDITrackerTest);
  REGISTER_TEST(itkRealtimeClockTest);
  REGISTER_TEST(itkStdStreamLogOutputTest);
  REGISTER_TEST(itkMultipleLogOutputTest);
  REGISTER_TEST(itkLoggerTest);
  REGISTER_TEST(itkThreadLoggerTest);
  REGISTER_TEST(itkLoggerOutputTest);
  REGISTER_TEST(itkLoggerManagerTest);
}
