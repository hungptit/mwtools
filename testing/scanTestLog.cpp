#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "utils/Basic.hpp"

bool parserInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "scanTestLog - Run sbscanlog to view the test results.")
            ("testlogarea,t", po::value<std::string>(), "Logging folder")
            ;

        po::positional_options_description p;
        p.add("testlogarea", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: scanTestLog [options]\n";
            std::cout << desc;
            return false;
        }

        std::string logDir;
        if (vm.count("testlogarea"))
        {
            logDir = vm["testlogarea"].as<std::string>();
        }
        else 
        {
            std::cout << "Usage: scanTestLog [options]\n";
            std::cout << desc;
            return false;
        }
        
        std::string cmdStr;
        if (!logDir.empty())
        {
            cmdStr = "sbscanlog -testlogarea " + logDir;
            Tools::run(cmdStr);
        }
        
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
    parserInputParameters(ac, av);
    return EXIT_SUCCESS;
}
