
#ifndef Utilities_hpp_
#define Utilities_hpp_

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>

#include "utils/Utils.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "Poco/Data/Session.h"
#include "Poco/Data/SQLite/Connector.h"

#include "Resources.hpp"
#include "sbtools/Sandbox.hpp"
#include "sbtools/LogFolder.hpp"

#include "utils/Process.hpp"

namespace Tools {   
    bool isLocalSandbox(const boost::filesystem::path &p) {
        using boost::filesystem::path;
        path local("/local"), localssd("/local-ssd");
        auto aPath = boost::filesystem::canonical(p).parent_path();
        while (!aPath.empty()) {
            if ((aPath == local) || (aPath == localssd)) {
                return true;
            }
            aPath = aPath.parent_path();
        } 
        return false;
    }    

    std::string args2string(std::vector<std::string> &args) {
        std::string results;
        for (auto arg : args) {
            results += arg + " ";
        }
        return results;
    }

    std::string readFile(const std::string &fileName) {
        std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
        if (!in) {
            std::runtime_error("Cannot read the content of \"" + fileName + "\". Error code" +
                               std::to_string(errno));
        }
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }    
}

#endif
