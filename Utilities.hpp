
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

namespace Tools {
    const boost::filesystem::path getNetworkSandboxPath() { return boost::filesystem::path(std::getenv("DEFAULT_SANDBOX")); }

    const boost::filesystem::path getBackupPath() { return getNetworkSandboxPath() / boost::filesystem::path("backup"); }

    boost::filesystem::path getBackupDir(const std::string &seed) {
        std::string strBuf = seed;
        replaceSubstring(strBuf, " ", "_");
        return boost::filesystem::path(strBuf) / boost::filesystem::path(Tools::getTimeStampString());
    }

    struct BackupInfo {
        std::string path;
        std::string comment;
        std::string time;
    };

    void backupSandbox(const std::string &comment, const boost::filesystem::path &database) {
        const boost::filesystem::path backupDir = getBackupDir(comment);
        const boost::filesystem::path backupPath = getBackupPath();
        const std::string cmdStr = "sbbackup -l " + backupDir.string() + " -r " + backupPath.string();
        Tools::run(cmdStr);

        // Record all information to a SQLite database
        const auto backupFolder = backupPath / backupDir;
        using namespace Poco::Data::Keywords;
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", database.string());
        session << "CREATE TABLE IF NOT EXISTS BackupInfo (Path VARCHAR(1024), Comment VARCHAR(1024), Time Date);", now;

        BackupInfo info = {backupFolder.string(), comment, getTimeStampString()};
        Poco::Data::Statement insert(session);
        insert << "INSERT INTO BackupInfo VALUES(?, ?, ?)", use(info.path), use(info.comment), use(info.time);
        insert.execute();

        // Display the verbose information
        std::cout << "Backup command: " << cmdStr << "\n";
        std::cout << "Database: " << database << "\n";
        std::cout << "Backup folder: " << backupFolder << "\n";
    }
#include "private/Utilities.cpp"
}

#endif
