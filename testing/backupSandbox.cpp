#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include "tools/Resources.hpp"
#include "tools/Sandbox.hpp"
#include "tools/Utilities.hpp"
#include "utils/Utils.hpp"

namespace {
    const boost::filesystem::path getNetworkSandboxPath() {
        return boost::filesystem::path(std::getenv("DEFAULT_SANDBOX"));
    }

    const boost::filesystem::path getBackupPath() {
        return getNetworkSandboxPath() / boost::filesystem::path("backup");
    }

    boost::filesystem::path getBackupDir(const std::string &seed) {
        std::string strBuf = seed;
        Tools::replaceSubstring(strBuf, " ", "_");
        return boost::filesystem::path(strBuf) /
               boost::filesystem::path(utils::getTimeStampString());
    }

    struct BackupInfo {
        std::string CurrentDir;
        std::string Path;
        std::string Comment;
        std::string Time;
        std::string Info;
    };

    void backupSandbox(std::string &&comment, const boost::filesystem::path &database) {
        const boost::filesystem::path backupDir = getBackupDir(comment);
        const boost::filesystem::path backupPath = getBackupPath();
        std::string results =
            utils::run("sbbackup", {"-l", backupDir.string(), "-r", backupPath.string()});

        // Record all information to SQLite database
        const auto backupFolder = backupPath / backupDir;
        using namespace Poco::Data::Keywords;
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", database.string());
        session << "CREATE TABLE IF NOT EXISTS BackupInfo (Sandbox VARCHAR(1024), Path "
                   "VARCHAR(1024), Comment VARCHAR(1024), Time Date, Text Info);",
            now;

        BackupInfo info = {boost::filesystem::current_path().string(), backupFolder.string(),
                           comment, utils::getTimeStampString(), results};
        Poco::Data::Statement insert(session);
        insert << "INSERT INTO BackupInfo VALUES(?, ?, ?, ?, ?)", use(info.CurrentDir),
            use(info.Path), use(info.Comment), use(info.Time), use(info.Info);
        insert.execute();

        // Display the verbose information
        std::cout << "Backup information database ==> " << database << "\n";
        std::cout << "Backup folder ==> " << backupFolder << "\n";
        std::cout << "Output of sbbackup command: \n" << results << "\n";
        std::cout << "Backup folder ==> " << backupFolder << "\n";
    }

    void parseInputParameters(int ac, char *av[]) {
        using namespace boost;
        namespace po = boost::program_options;
        using path = boost::filesystem::path;
        
        path database, defaultDatabase = getBackupPath() / boost::filesystem::path(".database.db");
        std::vector<std::string> comments;
        
        po::options_description desc("Allowed options");

        // clang-format off
        desc.add_options()
            ("help,h", "backupSandbox - Run sbbackup to backup all changes for a curent sandbox.")
            ("comments,c", po::value<std::vector<std::string>>(&comments), "Comment for back up changes. No special character except the white space.")
            ("database,d", po::value<path>(&database)->default_value(defaultDatabase), "Database used to log all information.");
        // clang-format on
        
        po::positional_options_description p;
        p.add("comments", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: backupSandbox [options]\n";
            std::cout << desc;
            return;
        }

        std::string commentStr;
        if (comments.empty()) {
            commentStr = utils::getTimeStampString();
        } else {
            if (comments.size() < 2) {
                commentStr = std::move(comments[0]);
            } else {
                auto iter = comments.begin();
                commentStr = *iter;
                std::for_each(++iter, comments.end(), [&commentStr](auto &&item) {
                    commentStr += " " + item;
            });
            }
        }

        backupSandbox(std::move(commentStr), database);
        return;
    }
}

int main(int ac, char *av[]) {
    parseInputParameters(ac, av);
    return EXIT_SUCCESS;
}
