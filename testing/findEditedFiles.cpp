#include "sbtools/FindEditedFiles.hpp"
#include "sbtools/Utilities.hpp"
#include "sbtools/Sandbox.hpp"
#include "sbtools/Resources.hpp"

bool runUnitTests(const std::string & folderPath, const bool verbose = false)
{
    Sbtools::FindEditedFiles finder;
    
    const std::string fullFolderPath = Tools::getAbslutePath(folderPath);   
    Sbtools::Sandbox sandbox(fullFolderPath);
    const std::string fileName = 
        sandbox.getSandboxPath() + 
        Tools::FileSeparator<std::string>::value + 
        Sbtools::Resources<std::string>::SbtoolsFolder + 
        Tools::FileSeparator<std::string>::value +
        Sbtools::Resources<std::string>::DatabaseFileName;

#ifdef DEBUG
    std::cout << "Database file: " << fileName << std::endl;
#endif

    finder.importWritableFileDatabase(fileName);
    
    const size_t fileNum = finder.searchWritableFiles(fullFolderPath);
    if (verbose) finder.print();
    // finder.write("output.txt");
    
    return true;
}


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "findWritableFiles - Find writable files in current folder.")
        ("folder,d", po::value<std::string>(), "Search folder's name")
        ;

    po::positional_options_description p;
    p.add("folder", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: findWritableFiles [options]\n";
        std::cout << desc;
        std::cout << "Examples: \n";
        std::cout << "\t findWritableFiles\n";
        std::cout << "\t findWritableFiles /local-ssd/sandbox/Aslrtw/matlab/\n";
        std::cout << "\t findWritableFiles -d /local-ssd/sandbox/Aslrtw/matlab/\n";
        std::cout << "\t findWritableFiles --folder /local-ssd/sandbox/Aslrtw/matlab/\n";
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
    
#ifdef DEBUG
    std::cout << "Searching for writable files in this folder: " << pathName << std::endl; 
#endif

    const bool flag = runUnitTests(pathName, true);
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
