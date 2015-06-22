#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Utils.hpp"
#include "sbtools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "reviewBoard - This binary is built on top of sbreviewboard.")
            ("open-browser,o", "Open the web browser.")
            ("change_id,c", po::value<std::string>(), "Perforce change ID")
            ("reviewboard_id,i", po::value<std::string>(), "The current reviewboard ID")
            ("description,d", po::value<std::string>(), "Description about changes")
            ;

        po::positional_options_description p;
        p.add("change_id", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: reviewBoard  -c change_id [options]\n";
            std::cout << desc;
            return false;
        }

        std::string changeId;
        if (vm.count("change_id"))
        {
            changeId = vm["change_id"].as<std::string>();
        }
        else 
        {
            std::cout << "Usage: reviewBoard -c change_id [options]\n";
            std::cout << desc;
            return false;
        }

        std::string optionString;
        if (vm.count("reviewboard_id"))
        {
            optionString = "-r " + vm["reviewboard_id"].as<std::string>();
        }

        
        if (vm.count("description"))
        {
            optionString += " -description=\"" + vm["description"].as<std::string>() + "\" ";
        }

        if (vm.count("open-browser")) {
            optionString += " -open-browser";
        }
        else {
            optionString += " -no-open-browser";
        }
        
        // Construct the executed command.
        const std::string cmdStr = Tools::SandboxResources<std::string>::ReviewBoardCommand + changeId + " " + optionString;
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
