#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "tools/Resources.hpp"
#include "utils/Process.hpp"
#include "utils/Utils.hpp"

std::vector<std::string> getRootLevelFolders(const boost::filesystem::path &rootPath) {
    namespace fs = boost::filesystem;
    std::vector<std::string> results;
    fs::directory_iterator endIter;
    fs::directory_iterator dirIter(rootPath);
    for (; dirIter != endIter; ++dirIter) {
        auto const status = dirIter->status();
        auto const ftype = status.type();
        if (ftype == boost::filesystem::directory_file) {
            results.emplace_back(dirIter->path().string());
        }
    }
    return results;
}


void createSandbox(const std::string &cluster) {
    const std::string cmdStr = "mw -using " + cluster + " sbs -c " + cluster;
    std::system(cmdStr.c_str());
    std::cout << "Command: " << cmdStr << "\n";
}

int main(int ac, char *av[]) {
    using namespace boost;
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");

    std::string cluster;
    std::vector<std::string> alias;
    
    // clang-format off
    desc.add_options()
        ("help,h", "This command will launch Matlab based on the given input arguments.")
        ("cluster,c", po::value<std::string>(&cluster)->default_value("Bmdlref"), "Used cluster.")
        ("alias,a", po::value<std::vector<std::string>>(&alias), "Aliasses for created sandboxes.");
    // clang-format on
    
    po::positional_options_description p;
    p.add("alias", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    // Parse input arguments
    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    if (alias.empty()) {
        createSandbox(cluster);
    } else {
        const boost::filesystem::path rootPath = boost::filesystem::current_path();
        std::vector<std::string> originalFolders = getRootLevelFolders(rootPath);
        std::unordered_set<std::string> dict(originalFolders.begin(), originalFolders.end());
        std::for_each(alias.begin(), alias.end(), [&dict, &cluster, &rootPath](auto const &anAlias) {
            createSandbox(cluster);
        
            // Find the new folder which is a new sandbox. This assumption is not process safed.
            const auto currentFolders = getRootLevelFolders(rootPath);
            for (auto aFolder : currentFolders) {
                if (dict.find(aFolder) == dict.end()) {
                    dict.insert(aFolder);
                    dict.insert(anAlias);
                
                    // Now create a symbolic link to the new sandbox and exit the for loop.
                    const std::string createAliasCommand = "ln -sf " + aFolder + " " + anAlias;
                    std::system(createAliasCommand.c_str());
                    std::cout << "Command: " << createAliasCommand << "\n";
                    break;
                }
            }
        });
        
    }



    return EXIT_SUCCESS;
}
