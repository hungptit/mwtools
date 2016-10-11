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

namespace {
    auto run(std::string &sandbox, std::vector<std::string> &inputs, bool verbose = false) {
        utils::ElapsedTime<utils::SECOND> timer;
        using path = boost::filesystem::path;

        // Build the run command.
        fmt::MemoryWriter writer;
        writer << "sb -nodesktop -nosplash  -memmgr release -s " << sandbox << " -r \"";
        std::for_each(inputs.cbegin(), inputs.cend(), [&writer](auto const &aFile) {
            path aPath(aFile);
            writer << "cd(fullfile(matlabroot, '../', '" << aPath.parent_path().string()
                   << "')); ";
            writer << "runsuite " << aPath.stem().string() << ";";
        });
        writer << "exit;\"";

        // Execute MATLAB tests.
        if (verbose) {
            fmt::print("Run command: {}\n", writer.str());
        }
        std::system(writer.str().c_str());
    }
}

int main(const int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    std::vector<std::string> inputs;
    auto sandboxPath = boost::filesystem::current_path();
    std::string sandbox;

    // clang-format off
    desc.add_options()
        ("help,h", "runTests - run tests using sbruntests script.")
        ("verbose,v", "Display detail information.")
        ("locale_ja_utf8,j", "Use Japanese locale for all sessions.")
        ("database,d", po::value<std::string>(),"Run folder")
        ("test-files,t", po::value<std::vector<std::string>>(&inputs), "Folders and test files users want to run.")
        ("sanbox,s", po::value<std::string>(&sandbox)->default_value(sandboxPath.string()), "A sandbox want to use to run tests. The default value is the current folder");
    // clang-format on

    po::positional_options_description p;
    p.add("inputs", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    const bool verbose = vm.count("help");

    if (inputs.empty()) {
        throw(std::runtime_error("Need to provide folders and/or tests want to run."));
    }

    run(sandbox, inputs, verbose);

    return EXIT_SUCCESS;
}
