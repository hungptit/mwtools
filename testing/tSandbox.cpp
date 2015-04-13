#include "sbtools/Sandbox.hpp"

bool runUnitTests(const std::string & folderPath)
{
    std::string ts("2002-01-20 00:00:00");
    boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
    std::cout << "Time: " << t<< std::endl;
    Tools::Sandbox sandbox(folderPath);
    return sandbox.unitTest();
}


int main()
{
    const bool flag = runUnitTests("/sandbox/hungdang/Aslrtw/matlab/toolbox");
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
