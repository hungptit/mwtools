#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>

#include "utils/Basic.hpp"
#include "sbtools/Utilities.hpp"
#include "sbtools/Resources.hpp"

template <typename T>
bool parseInputParameters(int ac, char* av[])
{
    using namespace boost;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "backupSandbox - Run sbbackup to backup all changes for a curent sandbox.")
        ("comment,c", po::value<std::string>(), "Comment for back up. No special character except the white space.")
        ;

    po::positional_options_description p;
    p.add("comment", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
              options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: backupSandbox [options]\n";
        std::cout << desc;
        return false;
    }

    std::string comment;
    if (vm.count("comment"))
    {
        comment = vm["comment"].as<std::string>();
    }
    else 
    {
        std::cout << "Usage: backupSandbox [options]\n";
        std::cout << desc;
        return false;
    }

    std::cout << "Backup folder: "<< Tools::backupSandbox(comment) << std::endl;    
    return true;
}


int main(int ac, char* av[])
{
    parseInputParameters<std::string>(ac, av);
    return EXIT_SUCCESS;
}
