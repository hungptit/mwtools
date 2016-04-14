#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "tools/Utilities.hpp"
#include "utils/Utils.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>

namespace {
    struct TestInfo {
        std::string RunFolder;
        std::string LogDir;
        std::string Command;
        std::string Time;
    };

    void runTests(const std::string &runFolder, bool locale_ja) {
        using boost::filesystem::path;
        std::vector<std::string> args;
        path aPath(runFolder);

        bool isLocal = (boost::filesystem::is_regular_file(runFolder) &&
                        isLocalSandbox(boost::filesystem::current_path())) ||
                       isLocalSandbox(runFolder);
        if (isLocal) {
            args.emplace_back("-local");
            args.emplace_back("all");
        } else {
            args.emplace_back("-autofarm");
            args.emplace_back("devel");
        }

        if (boost::filesystem::is_directory(aPath)) {
            args.emplace_back("-runallunder_mp");
            args.emplace_back(runFolder);
        } else {
            if (aPath.extension() == ".m") {
                // Run a given test file.
                args.emplace_back("-runsuite");
                args.emplace_back(runFolder);
            } else {
                if (runFolder.find("submit") == 0) {
                    // Assume submit*.txt is a submit file
                    args.emplace_back("-cfg");
                    args.emplace_back("dacore");
                    args.emplace_back("-F");
                    args.emplace_back(runFolder);
                } else { 
                    // This is a list of tests users want to run
                    args.emplace_back("-testsuites");
                    args.emplace_back(runFolder);
                }
            }
        }

        // Construct the log folder.
        auto defaultSandbox = std::getenv("DEFAULT_SANDBOX");
        auto logDir = path(defaultSandbox) / path("logs") / path(utils::getTimeStampString());

        // Update sbruntets arguments
        args.emplace_back("-testlogarea");
        args.emplace_back(logDir.string());

        if (locale_ja) {
            args.emplace_back("-apply-macros");
            args.emplace_back("global.locale_ja_utf8");
        }

        // Execute the sbruntest command
        std::string command = "sbruntests " + Tools::args2string(args);
        std::cout << "Run command: " << command << std::endl;
        std::system(command.c_str());

        // Log required information to a SQLite database
        using namespace Poco::Data::Keywords;
        path database = path(defaultSandbox) / path("backup" / path(".database.db"));
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", database.string());
        TestInfo info = {boost::filesystem::current_path().string(), logDir.string(), command,
                         utils::getTimeStampString()};
        session << "CREATE TABLE IF NOT EXISTS TestInfo (RunFolder VARCHAR(1024), LogFolder "
                   "VARCHAR(1024), Command VARCHAR(1024), Time Date);",
            now;
        Poco::Data::Statement insert(session);
        insert << "INSERT INTO TestInfo VALUES(?, ?, ?, ?)", use(info.RunFolder),
            use(info.LogDir), use(info.Command), use(info.Time);
        insert.execute();
    }

    void parseInputParameters(int ac, char *av[]) {
        using namespace boost;
        namespace po = boost::program_options;

        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "runTests - run tests using sbruntests script.")(
            "locale_ja_utf8,j", "Use Japanese locale for all sessions.")(
            "database,d", po::value<std::string>(),
            "Run folder")("folder,f", po::value<std::string>(), "Run folder")(
            "sanbox,s", po::value<std::string>(),
            "Sanbox want to run. The default value is the current folder");

        po::positional_options_description p;
        p.add("folder", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help") || (ac < 2)) {
            std::cout << "Usage: runTests [options]\n";
            std::cout << desc;
            std::cout << "Examples:" << std::endl;
            std::cout << "\t runTests test_folder_path - Run all tests "
                         "recursively in a given folder"
                      << std::endl;
            std::cout << "\t runTests submit.txt - Run Aslrtw test suite" << std::endl;
            std::cout << "\t runTests failed_tests.txt - Run all failed tests "
                         "in a text file"
                      << std::endl;
            return;
        }

        if (vm.count("folder")) {
            bool locale_ja = vm.count("locale_ja_utf8");
            runTests(vm["folder"].as<std::string>(), locale_ja);
        } else {
            std::cerr << "You need to provide a test folder or a submit file or a file which "
                         "has a list of tests!";
        }
    }
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
