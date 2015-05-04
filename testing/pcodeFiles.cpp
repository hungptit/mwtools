#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "utils/Utils.hpp"
#include "utils/Process.hpp"

void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()(
        "help,h",
        "pcodeFiles: A convenient utility which is built on top of sbpcode")(
        "submit_file,f", po::value<std::string>(), "A submitted file.");

    po::positional_options_description p;
    p.add("submit_file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(),
              vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: pcodeFiles  -f submit_file\n";
        std::cout << desc;
        return;
    }

    std::string fileName;
    if (vm.count("submit_file")) {
        fileName = vm["submit_file"].as<std::string>();
    } else {
        fileName = "submit.txt";
    }

    std::string command = "sbpcode";
    std::vector<std::string> args = {"-F", fileName};
    std::cout << Tools::run(command, args) << std::endl;
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
