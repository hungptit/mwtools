
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
        std::string Path;
        std::string Comment;
        std::string Time;
    };

    void backupSandbox(const std::string &comment, const boost::filesystem::path &database) {
        const boost::filesystem::path backupDir = getBackupDir(comment);
        const boost::filesystem::path backupPath = getBackupPath();
        const std::string cmdStr = "sbbackup -l " + backupDir.string() + " -r " + backupPath.string();
        Tools::run(cmdStr);

        // Record all information to SQLite database
        const auto backupFolder = backupPath / backupDir;
        using namespace Poco::Data::Keywords;
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", database.string());
        session << "CREATE TABLE IF NOT EXISTS BackupInfo (Path VARCHAR(1024), Comment VARCHAR(1024), Time Date);", now;

        BackupInfo info = {backupFolder.string(), comment, getTimeStampString()};
        Poco::Data::Statement insert(session);
        insert << "INSERT INTO BackupInfo VALUES(?, ?, ?)", use(info.Path), use(info.Comment), use(info.Time);
        insert.execute();

        // Display the verbose information
        std::cout << "Backup command: " << cmdStr << "\n";
        std::cout << "Database: " << database << "\n";
        std::cout << "Backup folder: " << backupFolder << "\n";
    }

    struct TestInfo {
        std::string LogDir;
        std::string Command;
        std::string Time;
    };

    void runTests(const std::string &farmOpts, const std::string &listFile, const std::string &runFolder,
                  boost::filesystem::path &database) {
        using boost::filesystem::path;
        std::string command = "sbruntests " + farmOpts;
        std::string seed;

        // Prepare sbruntets command
        if (!runFolder.empty()) {
            seed += "runtests";
            command += "-runallunder_mp " + runFolder;
        } else if (!listFile.empty()) {
            if (listFile.find("submit") == 0) {
                command += "-testsuites dacore -F " + listFile;
            } else {
                command += "-testsuites " + listFile;
                seed += "runtests";
            }
        } else {
            throw("Invalid options");
        }

        Tools::LogFolder logDir(path(std::getenv("DEFAULT_SANDBOX")) / path("logs") / path(seed));
        command += " -testlogarea " + logDir.getPath().string();

        // Log required information to SQLite database
        { 
            using namespace Poco::Data::Keywords;
            Poco::Data::SQLite::Connector::registerConnector();
            Poco::Data::Session session("SQLite", database.string());
            session << "CREATE TABLE IF NOT EXISTS TestInfo (LogFolder VARCHAR(1024), Command VARCHAR(1024), Time Date);", now;

            TestInfo info = {logDir.getPath().string(), command, getTimeStampString()};
            Poco::Data::Statement insert(session);
            insert << "INSERT INTO TestInfo VALUES(?, ?, ?)", use(info.LogDir), use(info.Command), use(info.Time);
            insert.execute();
        }

        // Execute the sbruntest command
        std::cout << "Run command: " << command << std::endl;
        Tools::run(command);
    }
#include "private/Utilities.cpp"
}

#endif
