#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "sbtools/Utilities.hpp"
#include "sbtools/ZFSUtils.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "This command will build the current sandbox using the given submit file. "
         "If there is not any input then it will build the matlab/src/simulink folder.")
        ("debug,d", "Build the curent sandbox is using DEBUG build.")
        ("sandbox,s", po::value<std::string>(), "Sandbox want to build.")
        ("submit_file,f", po::value<std::string>(), "Submit file.")
        ;

    po::positional_options_description p;
    p.add("submit_file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);
         
    if (vm.count("help")) 
    {
        std::cout << desc;
        return false;
    }

    std::string sandbox;
    if (vm.count("sandbox"))
    {
        sandbox = vm["sandbox"].as<std::string>();
    }
    else
    {
        sandbox = Tools::getCurrentFolder();
    }

   
    // Get all clones
    std::string submitFile;
    if (vm.count("submit_file"))
    {
        submitFile = vm["submit_file"].as<std::string>();
    }
    
    // Build the sandbox

    return true;
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
