// this file defines the igstkTests for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
    REGISTER_TEST(igstkMultipleOutputTest);
    REGISTER_TEST(igstkStateMachineTest);
    REGISTER_TEST(igstkStateMachineErrorsTest);
    REGISTER_TEST(igstkStateMachineTokenTest);
    REGISTER_TEST(igstkBasicTrackerTest);
    REGISTER_TEST(igstkTrackerToolTest);
    REGISTER_TEST(igstkTrackerPortTest);
}
