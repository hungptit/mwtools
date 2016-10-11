#include "utils/FileUtils.hpp"
#include "utils/Process.hpp"
#include "utils/TemporaryDirectory.hpp"
#include "utils/Timer.hpp"

#include <fstream>
#include <iostream>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "fmt/format.h"

#include "tbb/tbb.h"

namespace {
    std::string exec(const std::string &sandbox, const std::string &aTestFile,
                     const int displayNum) {
        fmt::MemoryWriter writer;
        auto logFile = boost::filesystem::temp_directory_path() /
                       boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
        writer << "xterm -display :" << displayNum << " -l -lf " << logFile.string()
               << " -e mrun_tests -s " << sandbox << " -t " << aTestFile;
        fmt::print("Run command: {}\n", writer.str());
        std::system(writer.str().c_str());
        return utils::read(logFile.string());
    }
}

int main(const int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    std::vector<std::string> testFiles;
    std::vector<int> displayIDs;
    const std::vector<int> defaultDisplayIDs{1, 2, 3, 4};
    auto sandboxPath = boost::filesystem::current_path();
    std::string sandbox;

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("locale_ja_utf8,j", "Use Japanese locale for all sessions.")
        ("display,d", po::value<std::vector<int>>(&displayIDs), "A list of display IDs")
        ("test-files,t", po::value<std::vector<std::string>>(&testFiles), "Folders and test files users want to run.")
        ("sanbox,s", po::value<std::string>(&sandbox)->default_value(sandboxPath.string()), "A sandbox want to use to run tests. The default value is the current folder");
    // clang-format on

    po::positional_options_description p;
    p.add("test-files", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    if (testFiles.empty()) {
        throw(std::runtime_error("Need to provide folders and/or tests want to run."));
    }

    if (displayIDs.empty()) displayIDs = defaultDisplayIDs;
    
    const bool verbose = vm.count("help");
    tbb::concurrent_vector<std::string> results;

    auto runTestObj = [&sandbox, &testFiles, &displayIDs, &results, verbose](const int idx) {
        auto const &aTestFile = testFiles[idx];
        results.push_back(exec(sandbox, aTestFile, displayIDs[idx%displayIDs.size()]));
    };
    
    const int size = testFiles.size();
    tbb::parallel_for(0, size, 1, runTestObj);

    for (auto const &item : results) {
        fmt::print("==== Results ====\n{}", item);
    }

    return EXIT_SUCCESS;
}
