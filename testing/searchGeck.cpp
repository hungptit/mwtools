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

    try {
        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "searchGeck - Search gecko for the given geck ID.")(
            "geck_id,i", po::value<std::string>(), "A desired geck ID");

        po::positional_options_description p;
        p.add("geck_id", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: searchGeck geck_id\n";
            std::cout << desc;
            return false;
        }

        std::string geckId;
        if (vm.count("geck_id")) {
            geckId = vm["geck_id"].as<std::string>();
        } else {
            geckId = "";
        }

        // Build options for web viewer
        std::string optionString;
        if (geckId.empty()) {
            optionString = Tools::SandboxResources<std::string>::Gecko;
        } else {
            optionString =
                Tools::SandboxResources<std::string>::Gecko + "view?Record=" + geckId;
        }

        Utils::run(Tools::SandboxResources<std::string>::WebViewer, {optionString});
        // std::cout << "Command: " << cmdStr << std::endl;
    }

    catch (std::exception &e) {
        std::cout << e.what() << "\n";
        return false;
    }

    return true;
}

int main(int ac, char *av[]) {
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
