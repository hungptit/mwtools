#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "sbtools/Utilities.hpp"
#include "sbtools/SyncSandbox.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "A convenient utility which is built on top of sbsync")
        ("sandbox,s", po::value<std::string>(), "A source sandbox. If this is not specified a default value for your current sandbox is used.")
        ("perfect,p", po::value<std::string>(), "Perfect area.")
        ("platforms,o", po::value< std::vector<std::string> >(), "Platforms want to sync, i.e glnxa64")
        ;

    po::positional_options_description p;
    p.add("sandbox", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: syncSandbox  -s sandbox_area\n";
        std::cout << desc;
        return false;
    }

    std::string sandbox;
    if (vm.count("sandbox"))
    {
        sandbox = vm["sandbox"].as<std::string>();
    }

    std::string perfectArea;
    if (vm.count("perfect"))
    {
        perfectArea = vm["perfect"].as<std::string>();
    }
    else
    {
        perfectArea = "Bmdlref";
    }

    std::vector < std::string > platforms;
    if (vm.count("platforms"))
    {
        platforms = vm["platforms"].as< std::vector < std::string > >();
    }
    else
    {
        platforms.push_back("glnxa64");
    }    

    Sbtools::SyncSandbox sync(sandbox, platforms, perfectArea);
    sync.sync();

    return true;
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
