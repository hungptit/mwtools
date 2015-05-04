#include "utils/Utils.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "sbtools/CleanupTmpFolder.hpp"

int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "findTestFiles - Find writable files in current folder.")
        ("tmp_folder,d", po::value<std::string>(), "Temporary folder")        ;

    po::positional_options_description p;
    p.add("folder", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    std::string tmpFolder;
    if (vm.count("tmp_folder"))
    {
        tmpFolder = vm["folder"].as<std::string>();
    }
    else
    {
        tmpFolder = "/tmp/";
    }
    
    // const boost::filesystem::path aPath(tmpFolder);
    // Tools::CleanupTmpFolder cleanObj(aPath);   
    return EXIT_SUCCESS;
}
