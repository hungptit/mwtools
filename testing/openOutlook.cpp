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
    desc.add_options()("help,h", "openOutlook - Open the web version of Outlook.")(
        "options,k", po::value<std::string>(), "search options");

    po::positional_options_description p;
    p.add("options", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    // Parse input arguments
    if (vm.count("help")) {
        std::cout << "Usage: openOutlook  -s options\n";
        std::cout << desc;
        return false;
    }

    std::string options;
    if (vm.count("options")) {
        options = vm["options"].as<std::string>();
    } else {
        options = "mail";
    }

    const std::string cmdStr = Tools::SandboxResources<std::string>::WebViewer + " " +
                               Tools::SandboxResources<std::string>::Outlook + options;
    utils::run(Tools::SandboxResources<std::string>::WebViewer,
               {Tools::SandboxResources<std::string>::Outlook + options});
    // std::cout << "Command: " << cmdStr << std::endl;

    return true;
}

int main(int ac, char *av[]) {
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
