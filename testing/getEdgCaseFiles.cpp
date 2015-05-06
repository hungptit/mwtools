#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "sbtools/Utilities.hpp"
#include "sbtools/Sandbox.hpp"

template <typename T>
bool parserInputParameters(int ac, char* av[],
                           T & caseId,
                           std::string & srcFolder,
                           std::string & desFolder)
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "getGeckFiles - Download all files associated with given geck to a given folder.")
            ("case-id,c", po::value<T>(), "Case ID")
            ("source,s", po::value<std::string>(), "Source folder")
            ("destination,d", po::value<std::string>(), "Destination folder")
            ;

        po::positional_options_description p;
        p.add("case-id", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: getEdgCaseFiles [options]\n";
            std::cout << desc;
            return false;
        }

        if (vm.count("case-id"))
        {
            caseId = vm["case-id"].as<T>();
        }
        else 
        {
            std::cout << "Usage: getEdgCaseFiles [options]\n";
            std::cout << desc;
            return false;
        }
        
        // Default source and destination folders
        // TODO: How to specialize this for Windows?
        srcFolder = "/mathworks/home/edgcases/"  + lexical_cast<T>(caseId);
        desFolder += "/local/sandbox/gecks/edg/" + lexical_cast<T>(caseId);

        if (vm.count("source"))
        {
            srcFolder = vm["source"].as<std::string>();
        }

        if (vm.count("destination"))
        {
            desFolder = vm["destination"].as<std::string>();
        }
        
        // Display input parameters
        std::cout << "Case ID is: "         << caseId << "\n";
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
    std::string caseId;
    if (parserInputParameters<std::string>(ac, av, caseId, srcFolder, desFolder))
        Tools::copyDir(srcFolder, desFolder);
    return EXIT_SUCCESS;
}
