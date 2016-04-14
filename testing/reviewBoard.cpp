#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "tools/Resources.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "reviewBoard - This binary is built on top of sbreviewboard.")
        ("open-browser,o", "Open the web browser.")
        ("change_id,c", po::value<std::string>(), "Perforce change ID. Users must provide the change set ID.")
        ("reviewboard_id,i", po::value<std::string>(), "The current reviewboard ID")
        ("description,d", po::value<std::string>(), "Description about changes");
    // clang-format on
    po::positional_options_description p;
    p.add("change_id", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    // Parse input arguments
    if (vm.count("help")) {
        std::cout << desc;
        return;
    }

    std::string changeId;
    if (vm.count("change_id")) {
        changeId = vm["change_id"].as<std::string>();
    } else {
        std::cout << "Usage: reviewBoard -c change_id [options]\n";
        std::cout << desc;
        return;
    }

    std::string optionString;
    if (vm.count("reviewboard_id")) {
        optionString = "-r " + vm["reviewboard_id"].as<std::string>();
    }

    if (vm.count("description")) {
        optionString += " -description=\"" + vm["description"].as<std::string>() + "\" ";
    }

    if (vm.count("open-browser")) {
        optionString += " -open-browser";
    } else {
        optionString += " -no-open-browser";
    }
    
    optionString += " -create-images";
    
    // Construct the executed command.
    auto command = Tools::SandboxResources<std::string>::ReviewBoardCommand + " " + changeId +
                   " " + optionString;
    std::system(command.c_str());
    std::cout << "Executed command: " << command << std::endl;
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
