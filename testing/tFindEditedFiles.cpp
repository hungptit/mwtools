#include "sbtools/FindEditedFiles.hpp"

bool runUnitTests()
{
    Sbtools::FindEditedFiles finder;
    return finder.unitTest();
}


int main(int ac, char* av[])
{
    const bool flag = runUnitTests();
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
