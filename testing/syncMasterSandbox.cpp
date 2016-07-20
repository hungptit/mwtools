#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

#include "fmt/format.h"

namespace {
    void parseInputParameters(int ac, char *av[]) {
        namespace po = boost::program_options;

        po::options_description desc("Allowed options");
        // clang-format off
        desc.add_options()(
            "help,h", "This command will sync a master sandbox on a local SSD drive.")
            ("root-dir,r", po::value<std::string>(), "A master sandbox.")
            ("sandbox,s", po::value<std::string>(), "A master sandbox name.")
            ("cluster,c", po::value<std::string>(), "A source cluster.")
            ("log-dir,l", po::value<std::string>(), "A new sandbox name.");
        // clang-format on
        po::positional_options_description p;
        p.add("sandbox", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: createSBSSandbox -c Bmdlref\n";
            std::cout << desc;
            return;
        }

        std::string rootDir;
        if (vm.count("root-dir")) {
            rootDir = vm["master"].as<std::string>();
        } else {
            auto currentPath = boost::filesystem::current_path();
            rootDir = currentPath.string();
        }

        std::string cluster;
        if (vm.count("cluster")) {
            cluster = vm["cluster"].as<std::string>();
        } else {
            cluster = "Bmdlref";
        }

        std::string sandbox;
        if (vm.count("sandbox")) {
            sandbox = vm["sandbox"].as<std::string>();
        } else {
            sandbox = "Bmdlref.latest_pass";
        }

        std::string logDir;
        if (vm.count("log-dir")) {
            logDir = vm["log-dir"].as<std::string>();
        } else {
            auto const logFolder = boost::filesystem::path("/local-ssd/logs/");
            if (!boost::filesystem::exists(logFolder)) {
                boost::filesystem::create_directory(logFolder);
            }
            logDir = logFolder.string() + cluster;
        }

        // Execute the sbsyncmaster command based on input information.
        const std::string syncCommand = "sbsyncmaster -C " + rootDir + " -log-dir " + logDir +
                                        " -src-root " + cluster + " -cfg default";
        fmt::print("sync command: {0}\n", syncCommand);
        std::system(syncCommand.c_str());
    }
}

int main(int ac, char *av[]) { parseInputParameters(ac, av); }
