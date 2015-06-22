#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Utils.hpp"
#include "sbtools/Utilities.hpp"
#include "sbtools/Resources.hpp"
#include "sbtools/Sandbox.hpp"

template <typename T> void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "backupSandbox - Run sbbackup to backup all changes for a curent sandbox.")(
        "comment,c", po::value<std::string>(), "Comment for back up changes. No special character except the white space.")(
        "database,d", po::value<std::string>(), "Database used to log all information.");

    po::positional_options_description p;
    p.add("comment", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: backupSandbox [options]\n";
        std::cout << desc;
        return;
    }

    std::string comment;
    if (vm.count("comment")) {
        comment = vm["comment"].as<std::string>();
    } else {
        std::cout << "Usage: backupSandbox [options]\n";
        std::cout << desc;
        return;
    }

    boost::filesystem::path database;
    if (vm.count("database")) {
        database = boost::filesystem::path(vm["database"].as<std::string>());
    } else {
        std::string sandboxPath = std::string(std::getenv("DEFAULT_SANDBOX"));
        if (sandboxPath.empty()) {
            std::cerr << "DEFAULT_SANDBOX environment variable is not set or invalid.\n";
        }
        database = boost::filesystem::path(sandboxPath) / boost::filesystem::path("backup/.database.db");
    }
    Tools::backupSandbox(comment, database);
    return;
}

int main(int ac, char *av[]) {
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
