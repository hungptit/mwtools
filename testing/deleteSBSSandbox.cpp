#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

bool parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()(
            "help,h", "This command will launch Matlab based on the given input arguments.")(
            "cluster,c", po::value<std::string>(), "Used cluster.")(
            "sandbox,s", po::value<std::string>(), "Sandboxes want to discard.");

        po::positional_options_description p;
        p.add("sandbox", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: createSBSSandbox -c Bmdlref\n";
            std::cout << desc;
            return false;
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
            std::cerr << "You will need to provide a sandbox name.\n";
            return 1;
        }

        // Remove the trailing slash
        auto it = --sandbox.end();
        for (; it != sandbox.begin(); --it) {
            if (*it != '/') {
                break;
            }
        }
        std::string sandboxName(sandbox.begin(), ++it);
        std::string userName = std::getenv("USER");
        const std::string cmdStr =
            "mw -using " + cluster + " sbs clone discard " + userName + "." + sandboxName;
        std::system(cmdStr.c_str());
        std::cout << "Command: " << cmdStr << std::endl;
    }

    catch (std::exception &e) {
        std::cout << e.what() << "\n";
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    parseInputParameters(argc, argv);
    return EXIT_SUCCESS;
}
