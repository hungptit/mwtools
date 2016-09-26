#include <cstdlib>
#include <iostream>

#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"

int main(int ac, char *av[]) {
    namespace po = boost::program_options;
    using path = boost::filesystem::path;
    boost::system::error_code errcode;
    path currentFolder = boost::filesystem::current_path(errcode);
    std::string defaultDataFolder = (currentFolder / path("data")).string();
    std::string defaultViewFolder = (currentFolder / path("views")).string();;
    std::string defaultCommand("/local/projects/testloganalyzer/bin/logdb.pl");
    std::string logFolder, dataFolder, viewFolder, command;

    po::options_description desc("Allowed options");
    
    // clang-format off
    desc.add_options()
        ("help,h", "parseTestLog - Parse a given test log using testlogparser.")
        ("command,c", po::value<std::string>(&command)->default_value(defaultCommand), "Run command")
        ("log-folder,l", po::value<std::string>(&logFolder), "A sbruntest log folder")
        ("data-folder,d", po::value<std::string>(&dataFolder)->default_value(defaultDataFolder), "A data folder")
        ("view-folder,v", po::value<std::string>(&viewFolder)->default_value(defaultViewFolder), "A view folder");
    // clang-format on
        
    po::positional_options_description p;
    p.add("log-folder", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    // Execute the test log parser command.
    std::string excCmd = command + " -t " + logFolder + " -d " + dataFolder + " -v " + viewFolder;
    std::cout << excCmd << "\n";
    return std::system(excCmd.c_str());
}
