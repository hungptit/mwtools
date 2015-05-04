#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "utils/FileSystemUtilities.hpp"
#include "utils/Utils.hpp"
#include "sbtools/Utilities.hpp"

template <typename T>
void parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "restoreSandbox: A convenient utility which is built on top of sbrestore")
        ("backup_folder,f", po::value<std::string>(), "The backup folder's path.")
        ;

    po::positional_options_description p;
    p.add("backup_folder", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << desc;
        return;
    }

    std::string folderName;
    if (vm.count("backup_folder"))
    {
        folderName = vm["backup_folder"].as<std::string>();
    }

    if (Tools::isDirectory(folderName))
    {
        std::string cmdStr = "sbrestore -restore-from " + folderName + ";sbgentbxcache";
        Tools::run(cmdStr);
        std::cout << "Command: " << cmdStr << std::endl;            
    }
    else
    {
        std::string errorMessage = "\"" + folderName + "\"" + " is not a valid folder";
        throw(errorMessage);
    }
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
