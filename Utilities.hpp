
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
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Resources.hpp"
#include "sbtools/Sandbox.hpp"
#include "sbtools/LogFolder.hpp"

namespace Tools {
    const boost::filesystem::path getNetworkSandboxPath() {
        return boost::filesystem::path(std::getenv("DEFAULT_SANDBOX"));
    }

    const boost::filesystem::path getBackupPath() {
        return getNetworkSandboxPath() / boost::filesystem::path("backup");
    }

    boost::filesystem::path getBackupDir(const std::string &seed) {
        std::string strBuf = seed;
        replaceSubstring(strBuf, " ", "_");
        return boost::filesystem::path(strBuf) /
               boost::filesystem::path(Tools::getTimeStampString());
    }

    const std::string backupSandbox(const std::string &comment) {
        const boost::filesystem::path backupDir = getBackupDir(comment);
        const boost::filesystem::path backupPath = getBackupPath();
        const std::string cmdStr =
            "sbbackup -l " + backupDir.string() + " -r " + backupPath.string();
        Tools::run(cmdStr);
        return (backupPath / backupDir).string();
    }

    std::string getSbruntestsCommand(const std::string &farm,
                                     const std::string &listFile,
                                     const std::string &runFolder);
    std::string readFile(const std::string &fileName);
    const std::string backupSandbox(const std::string &comment);
    const std::string
    generateGlobalDatabaseCommand(const std::string &sandboxPath);
    const std::string getDatabaseFileName(const std::string &sandboxPath);

#include "private/Utilities.cpp"
}

#endif
