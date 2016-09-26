#include <cstdlib>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

int main(int ac, char *av[]) {
    namespace po = boost::program_options;
    boost::system::error_code errcode;
    std::string computer;
    std::string resolution;
    std::string userName;
    const std::string defaultResolution("1680x1020"), defaultUserName("hungdang"),
        defaultComputer("ah-hungdang");

    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help,h", "parseTestLog - Parse a given test log using testlogparser.")
        ("computer,c", po::value<std::string>(&computer)->default_value(defaultComputer), "A remote computer")
        ("resolution,g", po::value<std::string>(&resolution)->default_value(defaultResolution), "Screen resolution")
        ("user-name,u", po::value<std::string>(&userName)->default_value(defaultUserName), "A user name");
    // clang-format on

    po::positional_options_description p;
    p.add("computer", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    // Execute the test log parser command.
    std::string excCmd = "sbrdesktop " + computer + " -g " + resolution;
    std::cout << excCmd << "\n";
    return std::system(excCmd.c_str());
}
