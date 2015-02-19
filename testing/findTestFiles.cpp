#include "sbtools/TestSuite.hpp"
#include "base/FileSystemUtilities.hpp"

bool runUnitTests(const std::string & folderPath, 
                  const std::vector< std::string > & searchStrings, 
                  const std::string & outputFile, 
                  const bool verbose = false)
{
    Sbtools::TestSuite testSuite;
    const size_t fileNum = testSuite.search(folderPath, testSuite.getRegexpString(searchStrings));
    std::cout << "Number of test files: " << fileNum << std::endl;    
    if (verbose) testSuite.print();
    if (!outputFile.empty()) testSuite.write(outputFile);
    return true;
}


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "findTestFiles - Find writable files in current folder.")
        ("folder,d", po::value<std::string>(), "Search folder's name")
        ("search_patterns,p", po::value< std::vector< std::string > >(), "Search patterns")
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
        std::cout << "\t findTestFiles /local-ssd/sandbox/Aslrtw/matlab/\n";
        std::cout << "\t findTestFiles -d /local-ssd/sandbox/Aslrtw/matlab/\n";
        std::cout << "\t findTestFiles --folder /local-ssd/sandbox/Aslrtw/matlab/\n";
        std::cout << "\t findTestFiles data/ -p CodeInfo -p Conversion --search_patterns geck";
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
    
    std::cout << "Searching for test files in this folder: " << pathName << std::endl; 

    std::vector< std::string > searchPatterns;
    if (vm.count("search_patterns"))
    {
        searchPatterns = vm["search_patterns"].as< std::vector< std::string > >();
    }

    
    std::string fileName;
    if (vm.count("output"))
    {
        fileName = vm["output"].as< std::string >();
    }

    const bool flag = runUnitTests(pathName, searchPatterns, fileName, true);
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
