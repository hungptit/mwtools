#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

int main(int argc, char *argv[]) {
    using namespace boost;
    namespace po = boost::program_options;

    std::vector<std::string> sandboxes;
    std::string cluster;

    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help,h", "This command will delete given SBS sanboxes.")
        ("cluster,c", po::value<std::string>(&cluster)->default_value("Bmdlref"), "Synced cluster.")
        ("sandbox,s", po::value<std::vector<std::string>>(&sandboxes), "Sandboxes want to discard.");
    // clang-format on

    po::positional_options_description p;
    p.add("sandbox", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    // Parse input arguments
    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    std::for_each(sandboxes.begin(), sandboxes.end(), [&cluster](const auto &aSandbox) {
        std::string userName = std::getenv("USER");
        const std::string cmdStr = "mw -using " + cluster + " sbs clone discard " + userName +
                                   "." + utils::normalize_path(aSandbox);
        std::system(cmdStr.c_str());
        std::cout << "Command: " << cmdStr << std::endl;
    });

    return EXIT_SUCCESS;
}
