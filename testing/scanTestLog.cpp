#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

void parserInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "scanTestLog - Run sbscanlog to view the test results.")(
        "testlogarea,t", po::value<std::string>(), "Logging folder");

    po::positional_options_description p;
    p.add("testlogarea", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        std::cout << "Usage: scanTestLog a_log_folder\n";
        return;
    }

    if (vm.count("testlogarea")) {
        Tools::run("sbscanlog", {"-testlogarea", vm["testlogarea"].as<std::string>()});
    } else {
        std::cout << desc;
        std::cout << "Usage: scanTestLog a_log_folder\n";
    }
}

int main(int ac, char *av[]) {
    parserInputParameters(ac, av);
    return EXIT_SUCCESS;
}
