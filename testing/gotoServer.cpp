#include <map>
#include <string>
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "utils/Utils.hpp"

class ServerAcesss 
{
  public:
    ServerAcesss() : UserMap() {init();}
    ~ServerAcesss() {}

    void access(const std::string & serverName, const std::string & userName = "hungptit")
    {
        Tools::run(getSshCommand(serverName, userName));
    }
    
  private:
    std::map < std::string, std::string > UserMap;

    void init()
    {
        UserMap["bioemag.nmsu.edu"] = "hungptit";
        UserMap["c-24-60-165-128.hsd1.ma.comcast.net"] = "hungptit";
    }
    

    const std::string getSshCommand(const std::string & serverName, const std::string & userName)
    {
        std::string commands = "ssh " + getUserName(serverName, userName) + "@" + serverName;
        return commands;
    }

    
    const std::string getUserName(const std::string & serverName, const std::string & name)
    {
        std::string userName;
        if (name.empty())
        {
            std::map< std::string, std::string >::const_iterator currentItem = UserMap.find(serverName);
            if (currentItem == UserMap.end())
            {
                userName = "hungdang";
            }
            else 
            {
                userName = UserMap[serverName];
            }
        }
        else
        {
            userName = name;
        }
        return userName;
    }
};


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "gotoServer: .")
        ("server,s", po::value<std::string>(), "Server name")
        ("user,u", po::value<std::string>(), "User name")
        ;

    po::positional_options_description p;
    p.add("server", -1);
        
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
        std::cout << "Usage: gotoServer [options]\n";
        std::cout << desc;
        return false;
    }

    std::string serverName;
    if (vm.count("server"))
    {
        serverName = vm["server"].as<std::string>();
    }
    else
    {
        serverName = "c-24-60-165-128.hsd1.ma.comcast.net";
    }

    std::string userName;
    if (vm.count("user"))
    {
        userName = vm["user"].as<std::string>();
    }

    ServerAcesss server;
    server.access(serverName, userName);
    
    return EXIT_SUCCESS;
}
