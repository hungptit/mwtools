#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

template <typename T> bool parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()("help,h", "Open my favorist web browser.")
        ("user-data-dir,u", po::value<std::string>(), "Specifies the directory that user data is kept.");
    // clang-format on

    po::positional_options_description p;
    p.add("user-data-dir", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    // Parse input arguments
    if (vm.count("help")) {
        std::cout << "Usage: openOutlook  -s options\n";
        std::cout << desc;
        return false;
    }

    std::string dataDir;
    if (vm.count("user-data-dir")) {
        dataDir = vm["user-data-dir"].as<std::string>();
    } else {
        dataDir = "/local-ssd/hungdang/cache/";
    }

    utils::run(Tools::SandboxResources<std::string>::WebViewer, {"--user-data-dir=" + dataDir});

    return true;
}

int main(int ac, char *av[]) {
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
