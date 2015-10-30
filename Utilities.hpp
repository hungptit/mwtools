
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
    const boost::filesystem::path getNetworkSandboxPath() { return boost::filesystem::path(std::getenv("DEFAULT_SANDBOX")); }

    const boost::filesystem::path getBackupPath() { return getNetworkSandboxPath() / boost::filesystem::path("backup"); }

    boost::filesystem::path getBackupDir(const std::string &seed) {
        std::string strBuf = seed;
        replaceSubstring(strBuf, " ", "_");
        return boost::filesystem::path(strBuf) / boost::filesystem::path(Tools::getTimeStampString());
    }

    struct BackupInfo {
        std::string CurrentDir;
        std::string Path;
        std::string Comment;
        std::string Time;
        std::string Info;
    };

    void backupSandbox(const std::string &comment, const boost::filesystem::path &database) {
        const boost::filesystem::path backupDir = getBackupDir(comment);
        const boost::filesystem::path backupPath = getBackupPath();
        std::string results = Tools::run("sbbackup", {"-l", backupDir.string(), "-r", backupPath.string()});

        // Record all information to SQLite database
        const auto backupFolder = backupPath / backupDir;
        using namespace Poco::Data::Keywords;
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", database.string());
        session << "CREATE TABLE IF NOT EXISTS BackupInfo (Sandbox VARCHAR(1024), Path VARCHAR(1024), Comment VARCHAR(1024), Time Date, Text Info);", now;

        BackupInfo info = {boost::filesystem::current_path().string(), backupFolder.string(), comment, getTimeStampString(), results};
        Poco::Data::Statement insert(session);
        insert << "INSERT INTO BackupInfo VALUES(?, ?, ?, ?, ?)", use(info.CurrentDir), use(info.Path), use(info.Comment), use(info.Time), use(info.Info);
        insert.execute();

        // Display the verbose information
        std::cout << "Database: " << database << "\n";
        std::cout << "Backup folder: " << backupFolder << "\n";
        std::cout << "Output of sbbackup command: \n" << results << "\n";
    }

    struct TestInfo {
        std::string RunFolder;
        std::string LogDir;
        std::string Command;
        std::string Time;
    };

    
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

    void runTests(const std::string &runFolder) {
        using boost::filesystem::path;
        std::vector<std::string> args;
        path aPath(runFolder);
        
        if (isLocalSandbox(runFolder)) {
            std::cout << "Local sandbox\n"; 
            args.emplace_back("-local");
            args.emplace_back("all");
        } else {
            std::cout << "Network sandbox\n";
            args.emplace_back("-autofarm");
            args.emplace_back("devel");
        }

        if (boost::filesystem::is_directory(aPath)) {
            args.emplace_back("-runallunder_mp");
            args.emplace_back(runFolder);
        } else {
            if (runFolder.find("submit") == 0) { // Assume submit*.txt is a submit file
                args.emplace_back("-F");
                args.emplace_back(runFolder);
            } else {            // This is a list of tests users want to run
                args.emplace_back("-testsuites");
                args.emplace_back(runFolder);
            } 
        }
        
        // Construct the log folder.
        auto defaultSandbox = std::getenv("DEFAULT_SANDBOX");
        auto logDir = path(defaultSandbox) / path("logs") / path(Tools::getTimeStampString());
        
        // Update sbruntets arguments
        args.emplace_back("-testlogarea");
        args.emplace_back(logDir.string());

        // Execute the sbruntest command
        // Tools::print(args);
        auto results = Tools::run("sbruntests", args);
        std::cout << results;

        // Log required information to a SQLite database
        { 
            using namespace Poco::Data::Keywords;
            path database = path(defaultSandbox) / path("backup" / path(".database.db"));
            Poco::Data::SQLite::Connector::registerConnector();
            Poco::Data::Session session("SQLite", database.string());
            
            std::string command;
            TestInfo info = {boost::filesystem::current_path().string(), logDir.string(), command, getTimeStampString()};

            session << "CREATE TABLE IF NOT EXISTS TestInfo (RunFolder VARCHAR(1024), LogFolder VARCHAR(1024), Command VARCHAR(1024), Time Date);", now;
            Poco::Data::Statement insert(session);
            insert << "INSERT INTO TestInfo VALUES(?, ?, ?, ?)", use(info.RunFolder), use(info.LogDir), use(info.Command), use(info.Time);
            insert.execute();
        }
    }
#include "private/Utilities.cpp"
}

#endif
