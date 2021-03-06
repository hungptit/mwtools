#include "utils/FileSearch.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Process.hpp"
#include "utils/Timer.hpp"
#include "utils/Utils.hpp"

#include <fstream>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "fmt/format.h"

#include "spdlog/spdlog.h"
#include "tbb/tbb.h"

#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/Session.h"

auto console = spdlog::stdout_color_mt("console");
auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");

namespace {

    struct TestFilter {
      public:
        bool isValid(utils::FileInfo &item) const {
            return !item.Stem.empty() && (item.Stem[0] == 't') && (item.Extension == ".m");
        }
    };

    auto getTestFiles(std::vector<std::string> &folders) {
        std::vector<std::string> files;

        // Search for files in the given folders.
        using path = boost::filesystem::path;
        using Container = std::vector<path>;
        utils::filesystem::SimpleVisitor<Container, utils::filesystem::NormalPolicy> visitor;
        Container searchFolders;
        for (auto item : folders) {
            searchFolders.emplace_back(path(item));
        }
        utils::filesystem::dfs_file_search(searchFolders, visitor);
        TestFilter f1;
        auto results = utils::filter(visitor.getResults(), f1);
        std::for_each(results.cbegin(), results.cend(),
                      [&files](auto &item) { files.emplace_back(item.Path); });
        return files;
    }

    std::tuple<std::string, std::string> exec(const std::string &sandbox, const std::string &aTestFile, const int displayNum) {
        fmt::MemoryWriter writer;
        auto logFile = boost::filesystem::temp_directory_path() /
                       boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
        writer << "xterm -display :" << displayNum << " -l -lf " << logFile.string()
               << " -e mrun_tests -s " << sandbox << " -t " << aTestFile;
        console->info("Run {0} test using command: {1}\n", aTestFile, writer.str());
        logger->info("Run command: {}\n", writer.str());
        std::system(writer.str().c_str());
        return std::make_tuple(aTestFile, utils::read(logFile.string()));
    }
}

int main(const int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    std::vector<std::string> folders;
    std::vector<int> displayIDs;

    constexpr int NumberOfDisplay = 12;
    std::vector<int> defaultDisplayIDs(NumberOfDisplay, 0);
    int n = 0;
    std::generate(defaultDisplayIDs.begin(), defaultDisplayIDs.end(), [&n] { return ++n; });

    auto sandboxPath = boost::filesystem::current_path();
    std::string sandbox;

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("locale_ja_utf8,j", "Use Japanese locale for all sessions.")
        ("display,d", po::value<std::vector<int>>(&displayIDs), "A list of display IDs")
        ("folders,t", po::value<std::vector<std::string>>(&folders), "Folders and test files users want to run.")
        ("sanbox,s", po::value<std::string>(&sandbox)->default_value(sandboxPath.string()), "A sandbox want to use to run tests. The default value is the current folder");
    // clang-format on

    po::positional_options_description p;
    p.add("folders", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    if (folders.empty()) {
        throw(std::runtime_error("Need to provide folders and/or tests want to run."));
    }

    auto files = getTestFiles(folders);

    if (displayIDs.empty())
        displayIDs = defaultDisplayIDs;

    const bool verbose = vm.count("help");
    tbb::concurrent_vector<std::tuple<std::string, std::string>> results;

    // Welcome message
    console->info("Welcome to mget_test_results!");

    // Run all tests
    auto runTestObj = [&sandbox, &files, &displayIDs, &results, verbose](const int idx) {
        std::string aTestFile = files[idx];
        auto testResults = exec(sandbox, aTestFile, displayIDs[idx % displayIDs.size()]);
        results.push_back(testResults);

        // Log  to the console
        console->info("{} is finished!", aTestFile);
        console->info("Current job status -> {0}/{1} tests are completed!\n", results.size(),
                      files.size());
        // Log to files
        logger->info("{} is finished!", aTestFile);
        logger->info("{} test log:", std::get<1>(testResults));
    };

    const int size = files.size();
    tbb::parallel_for(0, size, 1, runTestObj);

    // Write results to SQLite database
    using namespace Poco::Data::Keywords;
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", "logs/database.db");
    session << "CREATE TABLE IF NOT EXISTS TestLog (Sandbox VARCHAR(1024), Test "
               "VARCHAR(1024), Log VARCHAR(102400));",
        now;
    Poco::Data::Statement insert(session);
    for (auto &item : results) {
        insert << "INSERT INTO TestLog VALUES(?, ?, ?)", use(sandbox), use(std::get<0>(item)),
            use(std::get<1>(item));
    }
    insert.execute();

    return EXIT_SUCCESS;
}
