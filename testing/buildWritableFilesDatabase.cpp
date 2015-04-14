#include "sbtools/FindEditedFiles.hpp"
#include "sbtools/Sandbox.hpp"
#include "sbtools/Utilities.hpp"
#include "base/Resources.hpp"

bool runUnitTests(const std::string & folderPath, const bool verbose = false)
{
    const std::string fullFolderPath = Tools::getAbslutePath(folderPath);
    Tools::FindEditedFiles finder;
    const size_t fileNum = finder.searchWritableFiles(fullFolderPath);
    std::cout << "Number of writable files: " << fileNum << std::endl;   
    Tools::Sandbox sandbox(fullFolderPath);
    boost::filesystem::path aFile =
      boost::filesystem::path(sandbox.getSandboxPath()) /
      sandbox.getSandboxPath(".sbtools") /
      sandbox.getSandboxPath("database.txt"); 
    const std::string fileName = aFile.string();
    finder.createWritableFileDatabase(fileName);
    std::cout << "Database file: " << fileName << std::endl;
    return true;
}


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "getBaselineWritableFiles - Build the writable file database.")
        ("folder,d", po::value<std::string>(), "Folder used as baseline")
        ;

    po::positional_options_description p;
    p.add("folder", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: getBaselineWritableFiles [options]\n";
        std::cout << desc;
        std::cout << "Examples: \n";
        std::cout << "\t getBaselineWritableFiles\n";
        std::cout << "\t getBaselineWritableFiles path_to_a_folder\n";
        std::cout << "\t getBaselineWritableFiles -d path_to_a_folder\n";
        std::cout << "\t getBaselineWritableFiles --folder path_to_a_folder\n";
        return false;
    }

    std::string pathName;
    if (vm.count("folder"))
    {
        pathName = vm["folder"].as<std::string>();
    }
    else
    {
        pathName = Tools::getCurrentFolder();
    }
    
    std::cout << "Use this folder to build a writable database: "   << pathName << std::endl;

    const bool flag = runUnitTests(pathName, false);
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
