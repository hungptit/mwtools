#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "base/Basic.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "pcodeFiles: A convenient utility which is built on top of sbpcode")
            ("submit_file,f", po::value<std::string>(), "A submitted file.")
            ;

        po::positional_options_description p;
        p.add("submit_file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: pcodeFiles  -f submit_file\n";
            std::cout << desc;
            return false;
        }

        std::string fileName;
        if (vm.count("submit_file"))
        {
            fileName = vm["submit_file"].as<std::string>();
        }

        std::string cmdStr = "sbpcode -F " + fileName;
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
