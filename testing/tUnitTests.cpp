#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"
#include "base/FileSystemUtilities.hpp"
#include "base/Basic.hpp"

bool runUnitTests()
{
    bool flag = true;
   
    {
        std::string currentPath = Tools::getCurrentFolder();
        flag &= currentPath == Tools::FileSeparator<std::string>::value + "local" + 
            Tools::FileSeparator<std::string>::value + "sbtools" + 
            Tools::FileSeparator<std::string>::value + "src" + 
            Tools::FileSeparator<std::string>::value + "testing";
        std::cout << currentPath << std::endl;
    }

    {
        std::string folderName = 
            Tools::TemporaryFolder<std::string>::value + 
            Tools::FileSeparator<std::string>::value +
            "a" + 
            Tools::FileSeparator<std::string>::value + "b" + 
            Tools::FileSeparator<std::string>::value + "c" + 
            Tools::FileSeparator<std::string>::value + "d";
        flag &= Tools::createDirectory(folderName);
    }

    return flag;
}


int main(int ac, char* av[])
{
    const bool flag = runUnitTests();
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
