#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Utils.hpp"
#include "utils/Process.hpp"
#include "sbtools/Utilities.hpp"

bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "regenerateGlobalDatabase - Update global ID database.")
            ("sandbox,s", po::value<std::string>(), "Sandbox want to update database.")
            ;

        po::positional_options_description p;
        p.add("sandbox", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) 
        {
            std::cout << desc;
            return false;
        }


        std::string sandboxPath;
        if (vm.count("sandbox"))
        {
            sandboxPath = vm["sandbox"].as<std::string>();
        }

        const std::string cmdStr = Tools::generateGlobalDatabaseCommand(sandboxPath);
        Tools::run(cmdStr, {});
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
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
