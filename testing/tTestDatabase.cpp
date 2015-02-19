#include "sbtools/TestDatabase.hpp"

bool runUnitTests()
{
    Sbtools::TestDatabase database;
    return database.unitTest();
}


int main(int ac, char* av[])
{
    const bool flag = runUnitTests();
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
