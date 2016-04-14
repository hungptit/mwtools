#ifndef Sandbox_hpp_
#define Sandbox_hpp_

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/regex.hpp"
#include "utils/Utils.hpp"
#include <fstream>
#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/filesystem.hpp>
#include <locale>

#include "cppformat/format.h"

namespace {
    bool isLocalSandbox(const boost::filesystem::path &aPath) {
        const std::string pathString = boost::filesystem::canonical(aPath).string();
        const std::string localString = "/local/";
        const std::string localSSDString = "/local-ssd/";
        return (pathString.find(localString, 0) == 0) ||
               (pathString.find(localSSDString, 0) == 0);
    }

    bool isSandboxFolder(const boost::filesystem::path &aPath) {
        return boost::filesystem::is_regular_file(aPath /
                                                  boost::filesystem::path(".last_sunc"));
    }
}

namespace Tools {
    void replaceSubstring(std::string &stringBuf, const std::string &subString,
                          const std::string &replaceString) {
        size_t pos = 0;
        while (pos != std::string::npos) {
            pos = stringBuf.find(subString, pos);
            if (pos != std::string::npos)
                stringBuf.replace(pos, subString.length(), replaceString);
        }
    }

    class Sandbox {
      public:
        Sandbox(const boost::filesystem::path &folderPath) : FolderPath(folderPath) {
            getSandboxRootFolder();
            getPerfectPath();
            getSandboxTimeStamp();
        }

        const std::string &getSandboxPath() const { return SandboxPath.string(); };
        const std::string &getPerfectPath() const { return PerfectPath.string(); };

        void info() const {
            using namespace boost::posix_time;
            using namespace boost::gregorian;
            fmt::print("Sandbox path: {}\n", SandboxPath.string());
            fmt::print("Perfect path: {}\n", PerfectPath.string());
            fmt::print("Time stamp: {}\n", to_simple_string(TimeStamp));
        }

        bool isLocal() const { return isLocalSandbox(SandboxPath); }

      private:
        boost::filesystem::path FolderPath;
        boost::filesystem::path SandboxPath;
        boost::filesystem::path PerfectPath;
        boost::posix_time::ptime TimeStamp;

        void getSandboxRootFolder() {
            boost::filesystem::path aFolder = FolderPath;
            while (!isSandboxFolder(aFolder)) {
                aFolder = aFolder.parent_path();
                if (aFolder.empty()) {
                    throw std::runtime_error("A given folder is not inside a MW sandbox: " +
                                             FolderPath.string());
                }
            }
            SandboxPath = aFolder;
        }

        void getSandboxTimeStamp() {
            std::string strBuf = PerfectPath.string();
            boost::match_results<std::string::const_iterator> what;
            const boost::match_flag_type flags = boost::match_default;
            std::string::const_iterator begin = strBuf.begin();
            std::string::const_iterator end = strBuf.end();
            boost::regex expression("[0-9]{4}.[0-9]{2}.[0-9]{2}.h[0-9]{2}m[0-9]{2}s[0-9]{2}");
            std::string timeString;
            if (boost::regex_search(begin, end, what, expression, flags)) {
                timeString = what[0];
                replaceSubstring(timeString, "_", "-");
                replaceSubstring(timeString, "-h", " ");
                replaceSubstring(timeString, "m", ":");
                replaceSubstring(timeString, "s", ":");
                TimeStamp = boost::posix_time::time_from_string(timeString);
            }
        }

        void getPerfectPath() {
            std::ifstream inputFile(
                (SandboxPath / boost::filesystem::path(".last_sunc")).string());
            // Read the perfect path in the content of a .last_sunc file.
            if (inputFile.is_open()) {
                while (!inputFile.eof()) {
                    std::string lineBuf;
                    getline(inputFile, lineBuf);
                    if (!lineBuf.empty()) {
                        PerfectPath = lineBuf;
                        break;
                    }
                }
            }
            inputFile.close();
        }
    };

    std::string getFarmOption(const std::string &farm, const Sandbox &sandbox) {
        std::string options;
        const std::string farmStr = (sandbox.isLocal()) ? ("local") : (farm);

        if (farmStr == "local") {
            options = "";
        }

        else if (farm.empty()) {
            options = "-autofarm devel ";
        }

        else {
            options = "-autofarm devel ";
        }

        return options;
    }
}
#endif
