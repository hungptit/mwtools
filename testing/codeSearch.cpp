#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Utils.hpp"
#include "utils/Process.hpp"
#include "tools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "codeSearch - Search for the given keyword using code search database.")
            ("options,o", po::value<std::string>(), "Search options")
            ("file,f", po::value<std::string>(), "Search for a file")
            ("m-file,m", po::value<std::string>(), "Search for a M file")
            ("cpp-file,c", po::value<std::string>(), "Search for a cpp file")
            ("header-file,h", po::value<std::string>(), "Search for a header file")
            ;

        po::positional_options_description p;
        p.add("options", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Parse input arguments
        if (vm.count("help")) {
            std::cout << "Usage: codeSearch  -s options\n";
            std::cout << desc;
            return false;
        }

        std::string options;
        if (vm.count("options"))
        {
            options = "SearchResults.do?searchTerm=" + vm["options"].as<std::string>() + 
                "&searchField=TEXT&sort=PATH&fileType=M&fileType=Model&fileType=Java&fileType=C%2B%2B&fileType=Fortran&fileType=C%2FC%2B%2B+Header&fileType=C&fileType=XML&fileType=Resource&fileType=TLC&fileType=Makefiles&fileType=Makefile+Lists&fileType=MTF&fileType=IXF&fileType=Requirements&fileType=Module+Dependencies&fileType=Shell+Scripts&fileType=Simscape&fileType=Standard+ML&fileType=Perl&fileType=Message+Table&fileType=Translation+Table&fileType=C%23&fileType=Objective+C%2B%2B&fileType=JavaScript&fileType=Visual+Basic&fileType=Chart&fileType=Configuration&fileType=Web&fileType=Python+source&sourceDir=Activate.app&sourceDir=CVS&sourceDir=cefclient&sourceDir=Contents&sourceDir=Deactivate.app&sourceDir=X11&sourceDir=bin&sourceDir=bootstrap&sourceDir=cdimages&sourceDir=config&sourceDir=deployment_server&sourceDir=derived&sourceDir=doc&sourceDir=doxygen&sourceDir=etc&sourceDir=examples&sourceDir=extern&sourceDir=external&sourceDir=foundation&sourceDir=foundation_libraries&sourceDir=help&sourceDir=install&sourceDir=internaldoc&sourceDir=ja&sourceDir=java&sourceDir=lib&sourceDir=licenses&sourceDir=makefiles&sourceDir=makerules&sourceDir=math&sourceDir=mex&sourceDir=mcr&sourceDir=msvc_headers&sourceDir=notebook&sourceDir=pbr&sourceDir=platform&sourceDir=polyspace&sourceDir=resources&sourceDir=rtw&sourceDir=runtime&sourceDir=settings&sourceDir=simulink&sourceDir=src&sourceDir=standalone&sourceDir=stateflow&sourceDir=sys&sourceDir=test&sourceDir=toolbox&sourceDir=tools&sourceDir=webimages&indexId=1&indexDir=";
        }
        else 
        {
            options = "";
        }

        // Modify the options for special cases

        // Launch code search
        utils::run(Tools::SandboxResources<std::string>::WebViewer,
                   {Tools::SandboxResources<std::string>::CodeSearch + options});
        const std::string cmdStr = Tools::SandboxResources<std::string>::WebViewer + " " +
                                   Tools::SandboxResources<std::string>::CodeSearch + options;
        std::cout << "Command: " << cmdStr << std::endl;
    }

    catch (std::exception &e) {
        std::cout << e.what() << "\n";
        return false;
    }
    
    return true;
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
