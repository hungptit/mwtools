#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

template <typename T> void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "openCluster - Open a given cluster job queue.")(
        "cluster,k", po::value<std::string>(), "Cluster name for example Bmdlref.");

    po::positional_options_description p;
    p.add("cluster", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: openCluster  -s cluster\n";
        std::cout << desc;
        return;
    }

    std::string cluster;
    if (vm.count("cluster")) {
        cluster = vm["cluster"].as<std::string>();
    } else {
        cluster = "Bmdlref";
    }

    utils::run(Tools::SandboxResources<std::string>::WebViewer,
               {Tools::SandboxResources<std::string>::Batcave + cluster});
}

int main(int ac, char *av[]) {
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
