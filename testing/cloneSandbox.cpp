#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

#include "fmt/format.h"

namespace {
    void createSandbox(const std::string &aMasterSandbox, const std::string &aNewSandbox) {
        // Create a sandbox using sbclone
        std::string cmdStr = "sbclone " + aMasterSandbox + " " + aNewSandbox;
        std::system(cmdStr.c_str());

        fmt::print("Clone sandbox command: {0}\n", cmdStr);

        // Update the database using mupdatedb
        cmdStr = std::string("cd ") + aNewSandbox + ";" +
                 "mupdatedb -d .sbtools/.database matlab;" +
                 "ln -sf .sbtools/.database .database;";
        std::system(cmdStr.c_str());
        fmt::print("Update file database command: {0}\n", cmdStr);
    }

    void parseInputParameters(int ac, char *av[]) {
        using namespace boost;
        namespace po = boost::program_options;

        po::options_description desc("Allowed options");
        desc.add_options()(
            "help,h", "This command will launch Matlab based on the given input arguments.")(
            "master,s", po::value<std::string>(), "A master sandbox.")(
            "sandbox,d", po::value<std::vector<std::string>>(), "A new sandbox name.");

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

        std::string master;
        if (vm.count("master")) {
            master = vm["master"].as<std::string>();
        } else {
            master = "Bmdlref.latest_pass";
        }

        if (vm.count("sandbox")) {
            std::vector<std::string> newSandboxes =
                vm["sandbox"].as<std::vector<std::string>>();
            for (auto const &aNewSandbox : newSandboxes) {
                createSandbox(master, aNewSandbox);
            }
        }
    }
}

int main(int ac, char *av[]) { parseInputParameters(ac, av); }
