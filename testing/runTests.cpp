#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/Utils.hpp"

void parseInputParameters(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("database,d", po::value<std::string>(), "Run folder")
        ("folder,f", po::value<std::string>(), "Run folder")
        ("list-file,l", po::value<std::string>(), "A test list file")
        ("sanbox,s", po::value<std::string>(), "Sanbox want to run. The default value is the current folder")
        ("farm,f", po::value<std::string>(), "This parameter will be decided based on the location of current "
         "sandbox, for example 'farm' of a local sandbox is local. "
         "When running from a network sandbox the default farm is 'devel'. "
         "And the default value of farm is 'local' for a local sandbox.");
    
    po::positional_options_description p;
    p.add("folder", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help") || (ac < 2)) {
        std::cout << "Usage: runTests [options]\n";
        std::cout << desc;
        std::cout << "Examples:" << std::endl;
        std::cout << "\t runTests -d test_folder_path - Run all tests "
                     "recursively in a given folder"
                  << std::endl;
        std::cout << "\t runTests test_folder_path - Run all tests "
                     "recursively in a given folder"
                  << std::endl;
        std::cout << "\t runTests submit.txt - Run Aslrtw test suite" << std::endl;
        std::cout << "\t runTests failed_tests.txt - Run all failed tests "
                     "in a text file"
                  << std::endl;
        std::cout << "\t runTests -f failed_tests.txt - Run all failed "
                     "tests in a text file"
                  << std::endl;
        return;
    }

    std::string listFile;
    if (vm.count("list-file")) {
        listFile = vm["list-file"].as<std::string>();
        std::cout << "List file: " << listFile << std::endl;
    }

    std::string runFolder;
    if (vm.count("folder")) {
        std::string strBuf = vm["folder"].as<std::string>();
        std::cout << "strBuf: " << strBuf << std::endl;
        if (Tools::isRegularFile(strBuf)) {
            // TODO: Allow to run a single test file or a list of test files.
            listFile = strBuf;
            std::cout << "List file: " << listFile << std::endl;
        }
        else if (Tools::isDirectory(strBuf)) {
            runFolder = strBuf;
            std::cout << "Run folder: " << runFolder << std::endl;
        }
        else {
            std::cerr << "The given parameter is neither a regular file "
                         "nor a folder: "
                      << strBuf
                      << std::endl;
            return;
        }
    }

    boost::filesystem::path aFolder = boost::filesystem::current_path();
    if (vm.count("sandbox")) {
        aFolder = boost::filesystem::path(vm["sandbox"].as<std::string>());
    }
    Tools::Sandbox sandbox(aFolder);
    sandbox.info();

    // Get the database file
    boost::filesystem::path database;
    if (vm.count("database")) {
        database = boost::filesystem::path(vm["database"].as<std::string>());
    }
    else {
        std::string sandboxPath = std::string(std::getenv("DEFAULT_SANDBOX"));
        std::cout << sandboxPath << "\n";
        if (sandboxPath.empty()) {
            std::cerr << "DEFAULT_SANDBOX environment variable is not set or invalid.\n";
        }
        database = boost::filesystem::path(sandboxPath) / boost::filesystem::path("backup/.database.db");
    }

    std::string farm;
    if (vm.count("farm")) {
        farm = vm["farm"].as<std::string>();
    }

    // Run sbruntests command.
    Tools::runTests(Tools::getFarmOption(farm, sandbox), listFile, runFolder, database);
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
