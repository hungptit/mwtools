#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define BOOST_THREAD_VERSION 4
#include "boost/config.hpp"
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/thread.hpp"
#include "boost/thread.hpp"
#include "boost/thread/future.hpp"
#include "boost/unordered_set.hpp"

#include "utils/FileSearch.hpp"
#include "utils/FolderDiff.hpp"
#include "utils/LevelDBIO.hpp"
#include "utils/Timer.hpp"

#include "fmt/format.h"

#include <sstream>
#include <string>
#include <vector>

namespace {
    struct NormalFilter {
      public:
        bool isValid(utils::FileInfo &item) {
            return (std::find(ExcludedExtensions.begin(), ExcludedExtensions.end(),
                              std::get<utils::filesystem::EXTENSION>(item)) ==
                    ExcludedExtensions.end());
        }

      private:
        std::vector<std::string> ExcludedExtensions = {".p", ".d", ".o", ".ts"};
    };

    template <typename Container, typename Filter> void print(Container &data, Filter &f) {
        for (auto item : data) {
            if (f.isValid(item)) {
                fmt::print("{}\n", std::get<utils::filesystem::PATH>(item));
            }
        }
    }

    struct BasicFilter {
      public:
        bool isValid(const std::string &item) {
            using path = boost::filesystem::path;
            path aPath(item);
            return (std::find(ExcludedExtensions.begin(), ExcludedExtensions.end(),
                              aPath.extension()) ==
                    ExcludedExtensions.end());
        }

      private:
        std::vector<std::string> ExcludedExtensions = {".p", ".d", ".o", ".ts", ".m~", ".m#";}
    };

    template<typename Filter>
    void print_results(std::vector<std::string> &results, const std::string &title) {
        BasicFilter filter;
        using path = boost::filesystem::path;
        fmt::MemoryWriter writer;
        fmt::print("---- {} ----\n", title);
        for (auto const &item : results) {
            if (filter.isValid(item)) {
                writer << item << "\n";
            }             
        }
        fmt::print("{}", writer.str());
    }

    auto parse_submit_file(const std::string &fileName) {
        std::vector<std::string> modifiedFiles;
        std::vector<std::string> deletedFiles;
        using path = boost::filesystem::path;
        auto buffer = utils::read(fileName);
        size_t begin = 0, pos = 0;
        auto buflen = buffer.size();
        std::string deleteSign("-d ");
        while (pos < buflen) {
            if (buffer[pos] == '\n') {
                auto aLine = buffer.substr(begin, pos - begin);
                if (!aLine.empty()) {
                    auto it = aLine.find(deleteSign);
                    if (it != std::string::npos) {
                        path aFile(aLine.substr(it + 3));
                        if (boost::filesystem::is_regular_file(aFile)) {
                            // fmt::print("Deleted file: {0}\n", aLine.substr(it + 3));
                            deletedFiles.emplace_back(aLine.substr(it + 3));
                        }
                    } else {
                        path aFile(aLine);
                        if (boost::filesystem::is_regular_file(aFile)) {
                            modifiedFiles.emplace_back(aLine);
                            // fmt::print("Modified file: {0}\n", aLine);
                        }
                    }
                }
                begin = pos + 1;
            };
            ++pos;
        }
        return std::make_tuple(modifiedFiles, deletedFiles);
    }

    template <typename T> auto diff_vector(std::vector<T> &x, std::vector<T> &y) {
        std::unordered_set<T> dict_of_x(x.begin(), x.end());
        std::vector<T> dy;
        for (auto const &item : y) {
            if (dict_of_x.find(item) == dict_of_x.end()) {
                dy.emplace_back(item);
            } else {
                dict_of_x.erase(item);
            }
        }
        return std::make_tuple(std::vector<T>(dict_of_x.begin(), dict_of_x.end()), dy);
    }

    auto get_sandbox_diff(const std::string &dataFile, std::vector<std::string> &folders,
                          bool verbose) {
        utils::ElapsedTime<utils::SECOND> e;
        std::vector<utils::FileInfo> allEditedFiles, allNewFiles, allDeletedFiles;
        std::tie(allEditedFiles, allDeletedFiles, allNewFiles) =
            utils::diffFolders(dataFile, folders, verbose);

        // Find  the different between actual data and data from the submit file.
        std::vector<std::string> modifiedFiles;
        std::vector<std::string> deletedFiles;

        for (auto const &item : allEditedFiles) {
            modifiedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));
        }

        for (auto const &item : allNewFiles) {
            modifiedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));
        }

        for (auto const &item : allDeletedFiles) {
            deletedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));
        }

        return std::make_tuple(modifiedFiles, deletedFiles);
    }
}

int main(int argc, char *argv[]) {
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
    ("help,h", "Print this help")
    ("verbose,v", "Display searched data.")
    ("folders,f", po::value<std::vector<std::string>>(), "Search folders.")
    ("submit-file,s", po::value<std::string>(), "A submit file.")
    ("database,d", po::value<std::string>(), "File database.");
    // clang-format on

    po::positional_options_description p;
    p.add("submit-file", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        fmt::print("Example:\n\tcheckSubmitFile matlab/toolbox\n");
        return 0;
    }

    bool verbose = false;
    if (vm.count("verbose")) {
        verbose = true;
    }

    // Search folders
    std::vector<std::string> folders;
    if (vm.count("folders")) {
        folders = vm["folders"].as<std::vector<std::string>>();
    }

    std::string submitFile;
    if (vm.count("submit-file")) {
        submitFile = vm["submit-file"].as<std::string>();
    } else {
        submitFile = "submit.txt";
    }

    if (verbose) {
        fmt::print("Submit file: {}\n", submitFile);
    }

    // Get file database
    std::string dataFile;
    if (vm.count("database")) {
        dataFile = vm["database"].as<std::string>();
    } else {
        dataFile = (boost::filesystem::path(utils::Resources::Database)).string();
    }

    if (verbose) {
        std::cout << "Database: " << dataFile << std::endl;
    }

    // Read file information from a submit file
    auto submitResults = parse_submit_file(submitFile);
    auto sandboxDiff = get_sandbox_diff(dataFile, folders, verbose);

    // Display results.
    fmt::print("==== Summary ====\n");

    {
        auto modifiedResults =
            diff_vector(std::get<0>(sandboxDiff), std::get<0>(submitResults));
        print_results<BasicFilter>(std::get<0>(modifiedResults), "Missing modified or new files: ");
        print_results<BasicFilter>(std::get<1>(modifiedResults),
                      "Files have not been changed but they are listed in a submit file: ");
    }

    {
        auto deletedResults = diff_vector(std::get<1>(sandboxDiff), std::get<1>(submitResults));
        print_results<BasicFilter>(std::get<0>(deletedResults), "Missing deleted files: ");
        print_results<BasicFilter>(std::get<1>(deletedResults),
                      "Files have been marked as deleted, however, they are still exist: ");
    }
}
