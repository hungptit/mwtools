#include "utils/FileSearch.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Process.hpp"
#include "utils/Timer.hpp"
#include "utils/Utils.hpp"

#include <fstream>
#include <iostream>

#include "boost/algorithm/string.hpp"
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

#include "tools/Utilities.hpp"

auto console = spdlog::stdout_color_mt("console");

namespace {
   
    using IArchive = cereal::BinaryInputArchive;
    using OArchive = cereal::BinaryOutputArchive;

    // [test_path, runtime]
    using TestInfo = std::tuple<std::string, double>;
    constexpr double DefaultRunTime = 300; // The default run time is 5 minutes.
   
    struct RunLikeBatParams {
        std::string Sandbox;
        std::vector<std::string> Folders;
        int Display;
        std::string LogDir;
        bool Verbose;
        int NumberOfLoops;
        std::string RuntimeDatabase;
        std::string LogDatabase;
        std::string DatabaseKey;
    };

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
                const std::string ext = aPath.extension().string();
                console->info("Extension {}", ext);
                if (ext == ".m") {
                    data.push_back(std::make_tuple(item, infodb.getRuntimeInfo(item)));
                }

                if (ext == ".txt") {
                    // O(n) algorithm for parsing a failed test list.
                    const std::string stem = aPath.stem().string();
                    std::string buffer = utils::read(aPath.string());
                    if (stem == "failed_testsuites") {
                        console->info("Parse the tests list from a failed test file: \"{}\"",
                                      aPath.string());
                        size_t end = 0;
                        while (1) {
                            auto begin = buffer.find("-runsuite \"", end);
                            if (begin == std::string::npos) {
                                break;
                            }

                            end = buffer.find("%", begin);
                            if (end == std::string::npos) {
                                break;
                            }

                            auto const aSubstr = buffer.substr(begin + 11, end - begin - 12);
                            auto aTestFile = (path("matlab") / path(aSubstr)).string();
                            data.push_back(
                                std::make_tuple(aTestFile, infodb.getRuntimeInfo(aTestFile)));
                            console->info("Add {} to the test list.", aTestFile);
                        }

                    } else {
                        // Assume this text file has a list of tests that users want to run.
                        // Each test will be on a single line.
                        // TODO: Implement this
                    }
                }
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

    template <typename Param> TestInfo exec(Param &&params, const TestInfo &aTestFile) {
        utils::Timer localTimer;
        fmt::MemoryWriter writer;
        writer << "xterm -display :" << params.Display
               << " -e mw -using pwd runlikebat -nogenpathcache -testlist "
               << std::get<0>(aTestFile) << " -logs " << params.LogDir;
        console->info("Run {0} test using command: {1}", std::get<0>(aTestFile), writer.str());
        std::system(writer.str().c_str());
        return std::make_tuple(std::get<0>(aTestFile),
                               localTimer.toc() / localTimer.ticksPerSecond());
    }

    template <typename Param> auto scanTestLogs(Param &&params) {
        using path = boost::filesystem::path;
        auto htmlFile = path(params.LogDir) / path("report.html");
        auto failedTestListFile = path(params.LogDir) / path("failed_testsuites.txt");
        std::string command = "sbscanlog -C " + params.LogDir + " -ohtml-and-view " +
                              htmlFile.string() + " -failedtestlist " +
                              failedTestListFile.string();
        std::system(command.c_str());
        console->info("Scan all log files using command \"{}\".", command);
        console->info("Created a report here \"{}\".", htmlFile.string());
    }

    template <typename Params>
    auto readRuntimeInfo(Params &&params, const std::unique_ptr<rocksdb::DB> &db) {
        RuntimeInfo runtimeInfo;

        // If we cannot open the database then skip it.
        if (!db) {
            params.DatabaseKey = "Bmdlref";

            std::string value;
            rocksdb::Status s = db->Get(rocksdb::ReadOptions(), params.DatabaseKey, &value);
            if (s.ok()) {
                std::istringstream is(value);
                {
                    IArchive input(is);
                    input(runtimeInfo);
                }
            } else {
                console->info("Cannot read key \"{}\" from \"{}\" database!", params.DatabaseKey,
                              params.RuntimeDatabase);
            }
        }

        return runtimeInfo;
    }

    template <typename Param>
    auto runTests(Param &&params, RuntimeInfo &&runtimeInfo, bool verbose) {

        // Repeate the process "loop" times
        for (int loopIdx = 0; loopIdx < params.NumberOfLoops; ++loopIdx) {
            // Welcome message
            if (params.NumberOfLoops > 1) {
                console->info("==== Loop {} ====", loopIdx);
            }
            console->info("All results will be logged to \"{}\"!", params.LogDir);
            console->info("Read test run time information from \"{}\" database!",
                          params.RuntimeDatabase);
            auto files = getTestInfos(params.Folders, runtimeInfo);

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
            auto runTestObj = [&params, &files, &results, verbose](const int idx) {
                auto testResults = exec(params, files[idx]);
                results.push_back(testResults);

                console->info("{0} is finished in {1} seconds!", std::get<0>(testResults),
                              std::get<1>(testResults));
                console->info("==> {0}/{1} tests are completed!", results.size(), files.size());
            };

            const int size = files.size();

            tbb::parallel_for(0, size, 1, runTestObj);

            // Write updated runtime information to database.
            // console->info("Write updated run time information to \"{}\" database!",
            //               params.RuntimeDatabase);
            // for (auto const &item : results) {
            //     runtimeInfo.setRuntimeInfo(item);
            // }
            // std::stringstream output;
            // {
            //     OArchive oar(output);
            //     oar(runtimeInfo);
            // }
            // db->Put(rocksdb::WriteOptions(), params.DatabaseKey, output.str());
        }
    }
}

int main(const int argc, char *argv[]) {

    namespace po = boost::program_options;
    using path = boost::filesystem::path;

    po::options_description desc("Allowed options");

    RunLikeBatParams params;

    auto sandboxPath = boost::filesystem::current_path();

    // Create the log folder
    path defaultLogDir = (path(std::getenv("DEFAULT_SANDBOX")) / path("logs") /
                          path("mrunlikebat") / path(utils::getTimeStampString()));

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("folders,t", po::value<std::vector<std::string>>(&params.Folders), "Folders and test files users want to run.")
        ("log-dir,l", po::value<std::string>(&params.LogDir)->default_value(defaultLogDir.string()), "An output log folder")
        ("display,o", po::value<int>(&params.Display)->default_value(1), "An output display ID")
        ("loop,r", po::value<int>(&params.NumberOfLoops)->default_value(1), "Number of loops")
        ("run-time-info,u", po::value<std::string>(&params.RuntimeDatabase)->default_value("/local-ssd/runtime_database"), "Test run time database.")
        ("sanbox,s", po::value<std::string>(&params.Sandbox)->default_value(sandboxPath.string()), "A sandbox want to use to run tests. ");
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

    if (params.Folders.empty()) {
        throw(std::runtime_error("Need to provide folders and/or tests want to run."));
    }

    auto aFolder(params.LogDir);
    if (!boost::filesystem::exists(aFolder)) {
        boost::filesystem::create_directories(aFolder);
    }

    const bool verbose = vm.count("verbose");
    console->info("Run all tests using runlikebat");

    // Open a database then read test runtime information.
    // std::unique_ptr<rocksdb::DB> db(utils::open(params.RuntimeDatabase));
    // auto runtimeInfo = readRuntimeInfo(params, db);

    RuntimeInfo runtimeInfo;
    
    // Run all tests
    runTests(params, std::move(runtimeInfo), verbose);

    // Scan all log files and create a HTML report in the log folder
    scanTestLogs(params);
    console->info("All results are logged to \"{}\"!", params.LogDir);

    // Send an 
    Tools::sendMail(params.LogDir);
    
    // Return
    return EXIT_SUCCESS;
}
