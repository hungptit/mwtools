#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "tools/Utilities.hpp"
#include "base/Basic.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "checkoutCVSFile: A convenient utility which is built on top of sbm mget")
            ("file,f", po::value<std::string>(), "A file want to check out file")
            ("version,v", po::value<std::string>(), "The version of a checked out file")
            ;

        po::positional_options_description p;
        p.add("version", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: checkoutCVSFile  -v version_id -f checkout_file.txt\n";
            std::cout << desc;
            return false;
        }

        std::string fileName;
        if (vm.count("file"))
        {
            fileName = vm["file"].as<std::string>();
        }
        else 
        {
            std::cout << "Usage: reviewBoard -f submit_file.txt [options]\n";
            std::cout << desc;
            return false;
        }

        std::string version;
        if (vm.count("version"))
        {
            version = vm["version"].as<std::string>();
        }

        std::string cmdStr = "sbm mget -r " + version + " " + fileName;
        Tools::run(cmdStr);
        std::cout << "Command: " << cmdStr << std::endl;
    }

    catch(std::exception & e)
    {
        std::cout << e.what() << "\n";
        return false;
    }
    
    return true;
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
