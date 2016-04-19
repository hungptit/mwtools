#include <algorithm>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "utils/FileUtils.hpp"

int copy_directory(int ac, char *av[]) {
    using path = boost::filesystem::path;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    
    // clang-format off
    desc.add_options()
        ("help,h", "getGeckFiles - Download all files associated with given geck.")
        ("geck-id,g", po::value<size_t>(), "Geck ID")
        ("source,s", po::value<std::string>(), "Source folder")
        ("destination,d", po::value<std::string>(), "Destination folder");
    // clang-format on

    po::positional_options_description p;
    p.add("geck-id", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: getGeckFiles [options]\n";
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    size_t geckId;
    if (vm.count("geck-id")) {
        geckId = vm["geck-id"].as<size_t>();
    } else {
        std::cout << desc;
        return EXIT_FAILURE;
    }

    // Default source and destination folders
    auto srcFolder = path("/mathworks/home/tester/bugs/g" + std::to_string(geckId));
    path desFolder;
    if (vm.count("destination")) {
        desFolder = path(vm["destination"].as<std::string>());
    } else {
        desFolder = path("/local/sandbox/gecks/g" + std::to_string(geckId));
    }

    // Display input parameters
    std::cout << "Geck ID is: " << geckId << "\n";
    std::cout << "Source folder: " << srcFolder << "\n";
    std::cout << "Destination folder: " << desFolder << "\n";

    // Copy geck files to a folder.
    auto parentFolder = desFolder.parent_path();
    if (!boost::filesystem::exists(parentFolder)) {
        boost::filesystem::create_directories(parentFolder);
    }

    utils::copyDir(path(srcFolder), path(desFolder));
    return EXIT_SUCCESS;
}

int main(int ac, char *av[]) { return copy_directory(ac, av); }
