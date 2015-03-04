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
            ("help,h", "This binary is built on top of sbcheck.")
            ("submit_file,f", po::value<std::string>(), "A given submit file. Default values is submit.txt.")
            ("options,i", po::value<std::string>(), "Check options. Default value is \"-check sbedits\"")
            ;

        po::positional_options_description p;
        p.add("submit_file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: checkSubmitFile -c change_id [options]\n";
            std::cout << desc;
            return false;
        }

        std::string submitFile;
        if (vm.count("submit_file"))
        {
            submitFile = vm["submit_file"].as<std::string>();
        }
        else 
        {
            submitFile = "submit.txt";
        }

        std::string optionString;
        if (vm.count("options"))
        {
            optionString = vm["options"].as<std::string>();
        }
        else
        {
            optionString = " -check sbedits ";
        }

        const std::string cmdStr = Tools::SandboxResources<std::string>::SbCheckCommand + optionString + "-F " + submitFile;
        std::cout << "Command: " << cmdStr << std::endl;
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
