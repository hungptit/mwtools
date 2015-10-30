#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/Utils.hpp"

void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("database,d", po::value<std::string>(), "Run folder")
        ("folder,f", po::value<std::string>(), "Run folder")
        ("sanbox,s", po::value<std::string>(), "Sanbox want to run. The default value is the current folder");
    
    po::positional_options_description p;
    p.add("folder", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help") || (ac < 2)) {
        std::cout << "Usage: runTests [options]\n";
        std::cout << desc;
        std::cout << "Examples:" << std::endl;
        std::cout << "\t runTests test_folder_path - Run all tests "
                     "recursively in a given folder"
                  << std::endl;
        std::cout << "\t runTests submit.txt - Run Aslrtw test suite" << std::endl;
        std::cout << "\t runTests failed_tests.txt - Run all failed tests "
                     "in a text file"
                  << std::endl;
        return;
    }

    if (vm.count("folder")) {
        Tools::runTests(vm["folder"].as<std::string>());
    } else {
        std::cerr << "You need to provide a test folder or a submit file or a file which has a list of tests!";
    }
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
