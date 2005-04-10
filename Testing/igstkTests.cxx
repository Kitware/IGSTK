// this file defines the igstkTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
    REGISTER_TEST(igstkEllipsoidObjectTest);
    REGISTER_TEST(igstkCylinderObjectTest);
    REGISTER_TEST(igstkCommunicationTest);
    REGISTER_TEST(igstkMultipleOutputTest);
    REGISTER_TEST(igstkLoggerTest);
    REGISTER_TEST(igstkPulseGeneratorTest);
    REGISTER_TEST(igstkSceneTest);
    REGISTER_TEST(igstkStateMachineTest);
    REGISTER_TEST(igstkStateMachineErrorsTest);
    REGISTER_TEST(igstkTokenTest);
    REGISTER_TEST(igstkBasicTrackerTest);
    REGISTER_TEST(igstkTimeStampTest);
    REGISTER_TEST(igstkTrackerToolTest);
    REGISTER_TEST(igstkTrackerPortTest);
    REGISTER_TEST(igstkTransformTest);
    REGISTER_TEST(igstkViewTest);
}
