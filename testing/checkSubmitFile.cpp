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

#include "cppformat/format.h"

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

    void print_results(std::vector<std::string> &results, const std::string &title) {
        fmt::print("{}:\n", title);
        for (auto const &item : results) {
            fmt::print("{}\n", item);
        }
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
                        path aFile(aLine.substr(it+3));
                        if (boost::filesystem::is_regular_file(aFile)) {
                            fmt::print("Deleted file: {0}\n", aLine.substr(it+3));
                        }
                    } else {
                        path aFile(aLine);
                        if (boost::filesystem::is_regular_file(aFile)) {
                            fmt::print("Modified file: {0}\n", aLine);
                        }
                    }
                }                    
                begin = pos + 1;
            };
            ++pos;
        }
        return std::make_tuple(modifiedFiles, deletedFiles);
    }

    
    template<typename T> auto diff_vector(std::vector<T> &x, std::vector<T> &y) {
        std::unordered_set<T> dict_of_x(x.begin(), x.end());
        decltype(x) dx, dy;
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
    auto results = parse_submit_file(submitFile);

    {
        utils::ElapsedTime<utils::SECOND> e;
        std::vector<utils::FileInfo> allEditedFiles, allNewFiles, allDeletedFiles;
        std::tie(allEditedFiles, allDeletedFiles, allNewFiles) =
            utils::diffFolders(dataFile, folders, verbose);

        // Now we will display the results
        std::cout << "---- Modified files: " << allEditedFiles.size() << " ----\n";
        NormalFilter f;
        print(allEditedFiles, f);

        std::cout << "---- New files: " << allNewFiles.size() << " ----\n";
        print(allNewFiles, f);

        std::cout << "---- Deleted files: " << allDeletedFiles.size() << " ----\n";
        print(allDeletedFiles, f);

        // Find  the different between actual data and data from the submit file.
        std::vector<std::string> modifiedFiles;
        std::vector<std::string> deletedFiles;
        
        for (auto const &item : allEditedFiles) {
            modifiedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));            
        }

        for (auto const &item : allNewFiles) {
            modifiedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));            
        }

        for (auto const & item : allDeletedFiles) {
            deletedFiles.emplace_back(std::get<utils::filesystem::PATH>(item));
        }
        
        print_results(modifiedFiles, "Modified files: ");
        print_results(deletedFiles, "Deleted files: ");

        // Display results.
    }
}
