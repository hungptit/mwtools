#ifndef Utilities_hpp_
#define Utilities_hpp_

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>

#include "utils/TimeUtilities.hpp"
#include "utils/Resources.hpp"

#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

#include "utils/Basic.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/TimeUtilities.hpp"
#include "Resources.hpp"

namespace Tools 
{
    const boost::filesystem::path getNetworkSandboxPath() {
        return boost::filesystem::path (std::getenv("DEFAULT_SANDBOX"));
    }

    const boost::filesystem::path getBackupPath() {
        return getNetworkSandboxPath() / boost::filesystem::path("backup");
    }      

    
    void copyFiles(const std::string & srcFolder, const std::string & desFolder) {
        boost::filesystem::copy_directory(boost::filesystem::path(srcFolder), boost::filesystem::path(desFolder));
    }

    bool hasPrefixString(const std::string & str, const std::string & prefix);    
    void getRunningFarm(std::string & farm, const std::string & sandbox);
    std::string getFarmOption(const std::string & farm);
    const std::string getLogDir(const std::string & seed);
    void replaceSubstring(std::string & stringBuf, const std::string & subString, const std::string & replaceString);

    boost::filesystem::path getBackupDir(const std::string & seed)
    {
        std::string strBuf = seed;
        replaceSubstring(strBuf, " ", "_");
        return boost::filesystem::path(strBuf) /  boost::filesystem::path(getTimeStampString());
    }
    
    std::string getSbruntestsCommand(const std::string & farm, 
                                     const std::string & listFile, 
                                     const std::string & runFolder);
    std::string readFile(const std::string & fileName);
    const std::string backupSandbox(const std::string & comment);
    const std::string generateGlobalDatabaseCommand(const std::string & sandboxPath);
    const std::string getDatabaseFileName(const std::string & sandboxPath);

#include "private/Utilities.cpp"
}

#endif
