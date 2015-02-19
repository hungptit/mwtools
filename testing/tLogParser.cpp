#include "sbtools/LogParser.hpp"

bool runUnitTests(const std::string & logDir)
{
    Sbtools::LogParser parser(logDir);
    return parser.unitTest();
}


int main(int ac, char* av[])
{
    std::string logDir("data" + Tools::FileSeparator<std::string>::value + "logs" + Tools::FileSeparator<std::string>::value + "20130521T122959");
    const bool flag = runUnitTests(logDir);
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
