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

#include "utils/FolderDiff.hpp"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"

auto console = spdlog::stdout_color_mt("console");

namespace {
    // [test_path, runtime]
    using TestInfo = std::tuple<std::string, double>;
    constexpr double DefaultRunTime = 300; // The default run time is 5 minutes.

    class RuntimeInfo {
      public:
        double getRuntimeInfo(const std::string &aTestFile) const {
            auto it = TestRuntimeInfo.find(aTestFile);
            if (it != TestRuntimeInfo.end()) {
                return it->second;
            } else {
                return DefaultRunTime;
            }
        }

        void setRuntimeInfo(const TestInfo &info) {
            TestRuntimeInfo[std::get<0>(info)] = std::get<1>(info);
        }

        template <typename Archive> void serialize(Archive &ar) { ar(TestRuntimeInfo); }

      private:
        std::unordered_map<std::string, double> TestRuntimeInfo;
    };

    struct TestFilter {
      public:
        bool isValid(utils::FileInfo &item) const {
            return !item.Stem.empty() && (item.Stem[0] == 't') && (item.Extension == ".m");
        }
    };

    auto getTestInfos(std::vector<std::string> &folders, const RuntimeInfo &infodb) {
        std::vector<std::string> files;
        std::vector<TestInfo> data;

        // Search for files in the given folders.
        using path = boost::filesystem::path;
        using Container = std::vector<path>;
        utils::filesystem::SimpleVisitor<Container, utils::filesystem::NormalPolicy> visitor;
        Container searchFolders;
        std::vector<std::string> testFiles;
        for (auto item : folders) {
            path aPath(item);
            if (boost::filesystem::is_directory(aPath)) {
                searchFolders.emplace_back(path(item));
            } else {
                data.push_back(std::make_tuple(item, infodb.getRuntimeInfo(item)));
            }
        }
        utils::filesystem::dfs_file_search(searchFolders, visitor);
        TestFilter f1;
        auto results = utils::filter(visitor.getResults(), f1);
        std::for_each(results.cbegin(), results.cend(), [&files, &data, &infodb](auto &item) {
            data.emplace_back(item.Path, infodb.getRuntimeInfo(item.Path));
        });

        std::sort(data.begin(), data.end(), [](auto &first, auto &second) {
            return std::get<1>(first) > std::get<1>(second);
        });

        return data;
    }

    TestInfo exec(const std::string &, const TestInfo &aTestFile, const int display,
                  const std::string &logDir) {
        utils::Timer localTimer;
        fmt::MemoryWriter writer;
        writer << "xterm -display :" << display
               << " -e mw -using pwd runlikebat -nogenpathcache -testlist "
               << std::get<0>(aTestFile) << " -logs " << logDir;
        console->info("Run {0} test using command: {1}", std::get<0>(aTestFile), writer.str());
        std::system(writer.str().c_str());
        return std::make_tuple(std::get<0>(aTestFile),
                               localTimer.toc() / localTimer.ticksPerSecond());
    }
}

int main(const int argc, char *argv[]) {
    using IArchive = cereal::BinaryInputArchive;
    using OArchive = cereal::BinaryOutputArchive;

    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    std::vector<std::string> folders;

    int display;
    auto sandboxPath = boost::filesystem::current_path();
    std::string sandbox, logDir;
    
    // Create the log folder
    auto defaultLogDir = boost::filesystem::temp_directory_path() /
                         boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");

    int loop;

    std::string runTimeDatabase;

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("folders,t", po::value<std::vector<std::string>>(&folders), "Folders and test files users want to run.")
        ("log-dir,l", po::value<std::string>(&logDir)->default_value(defaultLogDir.string()), "An output log folder")
        ("display,o", po::value<int>(&display)->default_value(0), "An output display ID")
        ("loop,r", po::value<int>(&loop)->default_value(1), "Number of loops")
        ("run-time-info,u", po::value<std::string>(&runTimeDatabase)->default_value("/local-ssd/runtime_database"), "Test run time database.")
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

    const bool verbose = vm.count("verbose");
    console->info("Welcome to a multithreaded runlikebat!");
    
    // Repeate the process "loop" times
    for (int loopIdx = 0; loopIdx < loop; ++loopIdx) {
        // Welcome message
        console->info("==== Loop {} ====", loopIdx);
        console->info("All results will be logged to \"{}\"!", logDir);

        console->info("Read test run time information from \"{}\" database!", runTimeDatabase);
        std::unique_ptr<rocksdb::DB> db(utils::open(runTimeDatabase));
        RuntimeInfo runtimeInfo;
        const std::string aDatabaseKey("Bmdlref");

        {
            std::string value;
            rocksdb::Status s = db->Get(rocksdb::ReadOptions(), aDatabaseKey, &value);
            if (s.ok()) {
                std::istringstream is(value);
                {
                    IArchive input(is);
                    input(runtimeInfo);
                }
            } else {
                console->info("Cannot read key \"{}\" from \"{}\" database!", aDatabaseKey,
                              runTimeDatabase);
            }
        }

        auto files = getTestInfos(folders, runtimeInfo);

        if (verbose) {
            console->info("Below tests will be executed in top down order:");
            double totalTime(0);
            for (auto const &info : files) {
                console->info("\"{0}\" -> expected run time {1} seconds", std::get<0>(info),
                              std::get<1>(info));
                totalTime += std::get<1>(info);
            }
            console->info("==== Linear run time {} seconds ====", totalTime);
        }

        // Run all tests using threads
        tbb::concurrent_vector<TestInfo> results;
        console->info("Start to run {} tests", files.size());
        auto runTestObj = [&sandbox, &files, &results, &logDir, display,
                           verbose](const int idx) {
            auto testResults = exec(sandbox, files[idx], display, logDir);
            results.push_back(testResults);

            console->info("{0} is finished in {1} seconds!", std::get<0>(testResults),
                          std::get<1>(testResults));
            console->info("==> {0}/{1} tests are completed!", results.size(), files.size());
        };

        const int size = files.size();

        tbb::parallel_for(0, size, 1, runTestObj);

        // Write updated runtime information to database.
        console->info("Write updated run time information to \"{}\" database!",
                      runTimeDatabase);
        for (auto const &item : results) {
            runtimeInfo.setRuntimeInfo(item);
        }
        std::stringstream output;
        {
            OArchive oar(output);
            oar(runtimeInfo);
        }
        db->Put(rocksdb::WriteOptions(), aDatabaseKey, output.str());
    }

    // Scan the log folder when all tests are finished.
    std::string command = "sbscanlog -c " + logDir;
    std::system(command.c_str());

    console->info("All results will be logged to \"{}\"!", logDir);
    
    // Return
    return EXIT_SUCCESS;
}
