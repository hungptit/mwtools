#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "boost/thread/thread.hpp"
#include "sbtools/RunTests.hpp"

int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "findTestFiles - Find writable files in current folder.")
        ("sandbox,s", po::value< std::string >(), "Path to the sandbox want to run")
        ("list,t", po::value<std::string>(), "Run all tests listed in the given list file.")
        ("folder,d", po::value<std::string>(), "Run all tests under this folder. "
         "If this parameter is not set runLocalTests will run all tests under the current folder.")        
        ("search_patterns,p", po::value< std::vector< std::string > >(), "Search patterns for test files. "
         "If this parameter is empty then runLocalTests will run all tests.")
        ("log,l", po::value< std::string >(), "Test log area")
        ("threads_num,n", po::value< int >(), "Number of threads")
        ;

    po::positional_options_description p;
    p.add("folder", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: runLocalTests [options]\n";
        std::cout << desc;
        return false;
    }

    std::string testFolder;
    if (vm.count("folder"))
    {
        testFolder = vm["folder"].as<std::string>();
    }
    else
    {
        if (vm.count("list"))
            testFolder = vm["list"].as<std::string>();
        else
            testFolder = Tools::getCurrentFolder();
    }   
    
    std::cout << "Searching for test files in: " << testFolder << std::endl; 

    std::vector< std::string > searchPatterns;
    if (vm.count("search_patterns"))
    {
        searchPatterns = vm["search_patterns"].as< std::vector< std::string > >();
    }

    std::string sandboxPath;
    if (vm.count("sandbox"))
    {
        sandboxPath = vm["sandbox"].as< std::string >();
    }
    else
    {
        sandboxPath = Tools::getCurrentFolder();
    }
    std::cout << "Sandbox path: " << sandboxPath << std::endl;


    std::string logFolder;
    if (vm.count("log"))
    {
        logFolder = vm["log"].as< std::string >();
    }
    else
    {
        logFolder = Tools::TemporaryFolder<std::string>::value + Tools::FileSeparator<std::string>::value 
            + "logs" + Tools::FileSeparator<std::string>::value;
    }
    std::cout << "Log folder: " << logFolder << std::endl;
    
    int threads_num = boost::thread::hardware_concurrency() - 1; 
    if (vm.find("threads_num") != vm.end())
    {
        threads_num = vm["threads_num"].as< int >();
    }    
    std::cout << "Number of threads: " << threads_num << std::endl;

    Sbtools::RunTests test(sandboxPath, testFolder, logFolder, threads_num);
    test.run();
    
    const bool flag = true;;
    return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}
