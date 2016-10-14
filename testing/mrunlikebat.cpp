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

auto console = spdlog::stdout_color_mt("console");

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

    std::tuple<std::string, std::string> exec(const std::string &, const std::string &aTestFile,
                                              const int display, const std::string &logDir) {
        fmt::MemoryWriter writer;
        auto logFile = boost::filesystem::temp_directory_path() /
                       boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
        writer << "xterm -display :" << display << " -e mw -using pwd runlikebat -testlist "
               << aTestFile << " -logs " << logDir;
        console->info("Run {0} test using command: {1}\n", aTestFile, writer.str());
        std::system(writer.str().c_str());
        return std::make_tuple(aTestFile, logDir);
    }
}

int main(const int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    std::vector<std::string> folders;

    int display;
    auto sandboxPath = boost::filesystem::current_path();
    std::string sandbox, logDir;
    // Create the log folder
    auto defaultLogDir = boost::filesystem::temp_directory_path() /
                         boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("locale_ja_utf8,j", "Use Japanese locale for all sessions.")
        ("folders,t", po::value<std::vector<std::string>>(&folders), "Folders and test files users want to run.")
        ("log-dir,l", po::value<std::string>(&logDir)->default_value(defaultLogDir.string()), "An output log folder")
        ("display,d", po::value<int>(&display)->default_value(0), "An output display ID")
        ("sanbox,s", po::value<std::string>(&sandbox)->default_value(sandboxPath.string()), "A sandbox want to use to run tests. ");
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

    auto aFolder(logDir);
    if (!boost::filesystem::exists(aFolder)) {
        boost::filesystem::create_directories(aFolder);
    }

    auto files = getTestFiles(folders);

    const bool verbose = vm.count("help");
    tbb::concurrent_vector<std::tuple<std::string, std::string>> results;

    // Welcome message
    console->info("Welcome to a multithreaded runlikebat!");
    console->info("All results will be logged to \"{}\"!", logDir);

    // Run all tests
    auto runTestObj = [&sandbox, &files, &results, &logDir, display, verbose](const int idx) {
        std::string aTestFile = files[idx];
        auto testResults = exec(sandbox, aTestFile, display, logDir);
        results.push_back(testResults);

        // Log  to the console
        if (verbose) {
            console->info("{} is finished!", aTestFile);
            console->info("==> {0}/{1} tests are completed!\n", results.size(),
                          files.size());
        }
    };

    const int size = files.size();
    tbb::parallel_for(0, size, 1, runTestObj);

    // Scan the log folder when all tests are finished.
    std::string command = "sbscanlog -c " + logDir;
    std::system(command.c_str());
    
    return EXIT_SUCCESS;
}
