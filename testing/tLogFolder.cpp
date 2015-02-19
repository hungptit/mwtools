#include "sbtools/LogFolder.hpp"

bool runUnitTests(const std::string & rootPath)
{
    Sbtools::LogFolder logDir(rootPath);
    return logDir.unitTest();
}


int main(int ac, char* av[])
{
    const bool flag = runUnitTests("tmp");
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
