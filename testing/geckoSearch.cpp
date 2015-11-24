#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Utils.hpp"
#include "utils/Process.hpp"
#include "tools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "geckoSearch - Search gecko for the given geck ID.")
            ("keyword,k", po::value<std::string>(), "search keyword")
            ;

        po::positional_options_description p;
        p.add("keyword", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: geckoSearch keyword\n";
            std::cout << desc;
            return false;
        }

        std::string keyword;
        if (vm.count("keyword"))
        {
            keyword = vm["keyword"].as<std::string>();
        }
        else 
        {
            keyword = "";
        }

        // Build options for web viewer
        std::string optionString;
        if (keyword.empty())
        {
            optionString = Tools::SandboxResources<std::string>::GeckoSearch;
        }
        else 
        {
            optionString = Tools::SandboxResources<std::string>::GeckoSearch + keyword;
        }

        const std::string cmdStr = Tools::SandboxResources<std::string>::WebViewer + " " + optionString;
        Utils::run(Tools::SandboxResources<std::string>::WebViewer, {optionString});
        std::cout << "Argument: " << optionString << std::endl;
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
