#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Basic.hpp"
#include "sbtools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "openCluster - Search gecko for the given geck ID.")
            ("cluster,k", po::value<std::string>(), "search cluster")
            ;

        po::positional_options_description p;
        p.add("cluster", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: openCluster  -s cluster\n";
            std::cout << desc;
            return false;
        }

        std::string cluster;
        if (vm.count("cluster"))
        {
            cluster = vm["cluster"].as<std::string>();
        }
        else 
        {
            cluster = "Bmdlref";
        }

        const std::string cmdStr = Tools::SandboxResources<std::string>::WebViewer + " " + 
            Tools::SandboxResources<std::string>::Batcave + cluster;
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
