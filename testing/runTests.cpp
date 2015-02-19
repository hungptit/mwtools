#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/Basic.hpp"

template <typename Int>
bool parseInputParameters(int ac, char* av[], std::string & runCommand)
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "runTests - run tests using sbruntests script.")
            ("folder,d", po::value<std::string>(), "Run folder")
            ("list-file,l", po::value<std::string>(), "A test list file")
            ("sanbox,s", po::value<std::string>(), "Sanbox want to run. The default value is the current folder")
            ("farm,f", po::value<std::string>(), "This parameter will be decided based on the location of current sandbox, for example 'farm' of a local sandbox is local. "
             "When running from a network sandbox the default farm is 'devel'. And the default value of farm is 'local' for a local sandbox.")
            ;

        po::positional_options_description p;
        p.add("folder", -1);
        
        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
        
        if (vm.count("help") || (ac < 2)) {
            std::cout << "Usage: runTests [options]\n";
            std::cout << desc;
            std::cout << "Examples:" << std::endl;
            std::cout << "\t runTests -d test_folder_path - Run all tests recursively in a given folder" << std::endl;
            std::cout << "\t runTests test_folder_path - Run all tests recursively in a given folder" << std::endl;
            std::cout << "\t runTests submit.txt - Run Aslrtw test suite" << std::endl;
            std::cout << "\t runTests failed_tests.txt - Run all failed tests in a text file" << std::endl;
            std::cout << "\t runTests -f failed_tests.txt - Run all failed tests in a text file" << std::endl;
            return false;
        }
               
        std::string listFile;       
        if (vm.count("list-file"))
        {
            listFile = vm["list-file"].as<std::string>();
            std::cout << "List file: " << listFile << std::endl;
        }
        
        std::string runFolder;
        if (vm.count("folder"))
        {
            std::string strBuf = vm["folder"].as<std::string>();
            std::cout << "strBuf: " << strBuf << std::endl;
            if (Tools::isRegularFile(strBuf))
            {
                listFile = strBuf;
                std::cout << "List file: " << listFile << std::endl;
            }

            else if (Tools::isDirectory(strBuf))
            {                
                runFolder = strBuf;
                std::cout << "Run folder: " << runFolder << std::endl;
            }

            else
            {
                std::cerr << "The given parameter is neither a regular file nor a folder: " << strBuf << std::endl;
                return false;
            }            
        }
        
        std::string sandbox;
        if (vm.count("sandbox"))
        {
            sandbox = vm["sandbox"].as<std::string>();
        }
        else 
        {
            sandbox = Tools::getCurrentFolder();
        }
        std::cout << "Sandbox: " << sandbox << std::endl;

        std::string farm;
        if (vm.count("farm"))
        {
            farm = vm["farm"].as<std::string>();
        }       
        Tools::getRunningFarm(farm, sandbox);
        // std::cout << "Running farm: " << farm << std::endl;

        // Create the sbruntests command
        runCommand = Tools::getSbruntestsCommand(farm, listFile, runFolder);
        std::cout << "Run command: " << runCommand << std::endl;
    }

    catch(std::exception & e)
    {
        std::cout << e.what() << "\n";
        return false;
    }
        
    
    return true;
}


int main(int ac, char* av[])
{
    std::string command;
    if (parseInputParameters<size_t>(ac, av, command))
        Tools::run(command);
    return EXIT_SUCCESS;
}
