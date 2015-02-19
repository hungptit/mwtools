#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "base/Basic.hpp"
#include "sbtools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "This command will launch Matlab based on the given input arguments.")
            ("cluster,c", po::value<std::string>(), "Used cluster.")
            ("options,o", po::value<std::string>(), "Other options.")
            ;

        po::positional_options_description p;
        p.add("options", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: runMatlab  -c Bmdlref [options]\n";
            std::cout << desc;
            return false;
        }

        std::string optionString;
        if (vm.count("options"))
        {
            optionString = vm["options"].as<std::string>();
        }
        else 
        {
            optionString = "-softwareopengl ";
        }

        if (vm.count("cluster"))
        {
            optionString += "-s " + vm["cluster"].as<std::string>();
        }

        const std::string cmdStr = Sbtools::Resources<std::string>::SbCommand + optionString;
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
