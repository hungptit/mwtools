#include "sbtools/TestSuite.hpp"
#include "sbtools/Utilities.hpp"

bool runUnitTests()
{
    Sbtools::TestSuite testSuite;
    return testSuite.unitTest();
}


int main(int ac, char* av[])
{
    const bool flag = runUnitTests();
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
