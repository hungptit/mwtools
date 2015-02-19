#include <vector>

#include "base/FileFinder.hpp"
#include "base/Basic.hpp"
#include "base/FileSystemUtilities.hpp"

bool find(const std::string & folderPath, 
                  std::vector<std::string> & searchPatterns,
                  std::vector<std::string> & fileExtensions,
                  const std::string & outputFile, 
                  const bool verbose = false)
{
    Tools::FileFinder finder;
    const size_t fileNum = finder.search(folderPath, searchPatterns, fileExtensions);
    if (verbose) finder.print();
    return true;
}


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "findTestFiles - Find writable files in current folder.")
        ("verbose,v", "Display input arguments.")
        ("folder,d", po::value<std::string>(), "Search folder's name")
        ("search_patterns,p", po::value< std::vector<std::string> >(), "Search patterns")
        ("file_extensions,e", po::value< std::vector<std::string> >(), "File extensions")
        ("output,o", po::value< std::string >(), "Output file name")
        ;

    po::positional_options_description p;
    p.add("folder", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: findTestFiles [options]\n";
        std::cout << desc;
        std::cout << "Examples: \n";
        std::cout << "\t findTestFiles\n";
        std::cout << "\t findTestFiles test_folder_path\n";
        std::cout << "\t findTestFiles -d test_folder_path\n";
        std::cout << "\t findTestFiles --folder test_folder_path\n";
        std::cout << "\t findTestFiles test_folder_path -p CodeInfo -p Conversion --search_patterns geck";
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
    
    std::vector<std::string> searchPatterns;
    if (vm.count("search_patterns"))
    {
        searchPatterns = vm["search_patterns"].as< std::vector<std::string> >();
    }

    std::vector<std::string> fileExtensions;
    if (vm.count("file_extensions"))
    {
        fileExtensions = vm["file_extensions"].as< std::vector<std::string> >();
    }

    
    std::string fileName;
    if (vm.count("output"))
    {
        fileName = vm["output"].as< std::string >();
    }

    const bool verbose = vm.find("verbose") != vm.end();

    if (verbose)
    {
        std::cout << "Searching for test files in this folder: " << pathName << std::endl; 
        Tools::disp(searchPatterns, "Search patterns:");
        Tools::disp(fileExtensions, "File extensions:");
    }


    const bool flag = find(pathName, searchPatterns, fileExtensions, fileName, true);
    return true ? EXIT_SUCCESS : EXIT_FAILURE;
}
