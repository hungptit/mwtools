#ifndef Utilities_hpp_
#define Utilities_hpp_

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>

#include "base/TimeUtilities.hpp"
#include "base/Resources.hpp"

#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

#include "base/Basic.hpp"
#include "base/FileSystemUtilities.hpp"
#include "Resources.hpp"

namespace Sbtools 
{
    void copyFiles(const std::string & srcFolder, const std::string & desFolder);   
    bool hasPrefixString(const std::string & str, const std::string & prefix);    
    void getRunningFarm(std::string & farm, const std::string & sandbox);
    std::string getFarmOption(const std::string & farm);
    const std::string getLogDir(const std::string & seed);
    void replaceSubstring(std::string & stringBuf, const std::string & subString, const std::string & replaceString);
    std::string getBackupDir(const std::string & seed);
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
