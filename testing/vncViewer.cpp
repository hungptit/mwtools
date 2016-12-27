#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include <string>
#include <iostream>


int main(int ac, char *av[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "gotoServer: .")(
                                                  "server,s", po::value<std::string>(), "Server name")("user,u", po::value<std::string>(),
                                                             "User name");

    po::positional_options_description p;
    p.add("server", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        return EXIT_FAILURE;
    }

    std::string serverName;
    if (vm.count("server")) {
        serverName = vm["server"].as<std::string>();
    } else {
        serverName = "hungdang-deb8-64.dhcp.mathworks.com";
    }

    std::string userName;
    if (vm.count("user")) {
        userName = vm["user"].as<std::string>();
    } else {
      userName = "hungdang";
    }

    std::string command = "vncviewer " + serverName + ":1";
    std::cout << serverName << "\n";
    std::cout << userName << "\n";
    std::cout << command << "\n";
    std::system(command.c_str());
}
