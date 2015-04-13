#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "sbtools/Utilities.hpp"
#include "sbtools/Sandbox.hpp"

template <typename Int>
bool parserInputParameters(int ac, char* av[],
                           size_t & geckId,
                           std::string & srcFolder,
                           std::string & desFolder)
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "getGeckFiles - Download all files associated with given geck to a given folder.")
            ("geck-id,g", po::value<Int>(), "Geck ID")
            ("source,s", po::value<std::string>(), "Source folder")
            ("destination,d", po::value<std::string>(), "Destination folder")
            ;

        po::positional_options_description p;
        p.add("geck-id", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: getGeckFiles [options]\n";
            std::cout << desc;
            return false;
        }

        if (vm.count("geck-id"))
        {
            geckId = vm["geck-id"].as<Int>();
        }
        else 
        {
            std::cout << "Usage: getGeckFiles [options]\n";
            std::cout << desc;
            return false;
        }
        
        // Default source and destination folders
        srcFolder = "/mathworks/home/tester/bugs/g" + lexical_cast<std::string>(geckId);
        desFolder += "/local/sandbox/gecks/g" + lexical_cast<std::string>(geckId);

        if (vm.count("source"))
        {
            srcFolder = vm["source"].as<std::string>();
        }

        if (vm.count("destination"))
        {
            desFolder = vm["destination"].as<std::string>();
        }
        
        // Display input parameters
        std::cout << "Geck ID is: " << geckId << "\n";
        std::cout << "Source folder: "      << srcFolder << std::endl;
        std::cout << "Destination folder: " << desFolder << std::endl;
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
    std::string srcFolder;
    std::string desFolder;
    size_t geckId;
    if (parserInputParameters<size_t>(ac, av, geckId, srcFolder, desFolder))
        Tools::copyFiles(srcFolder, desFolder);
    return EXIT_SUCCESS;
}
