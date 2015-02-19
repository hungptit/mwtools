#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"

int main(int ac, char* av[])
{
    std::string strBuf = "fdsfsd  fdsg dsgdsgd sgds";
    std::string strBuf_results = "fdsfsd..fdsg.dsgdsgd.sgds";
    std::cout << strBuf << std::endl;
    Tools::replaceSubstring(strBuf, " ", ".");
    std::cout << strBuf << std::endl;
    return (strBuf == strBuf_results) ? EXIT_SUCCESS : EXIT_FAILURE;
}
