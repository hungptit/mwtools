#ifndef Sandbox_hpp_
#define Sandbox_hpp_

#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include "boost/regex.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Utilities.hpp"
#include "base/Basic.hpp"

namespace Tools
{
    class Sandbox
    {
      public:
        Sandbox(const std::string & folderPath);
        ~Sandbox();
        const std::string & getSandboxPath() const;
        const std::string & getPerfectPath() const;
        void info() const;
        bool isLocal();
        bool unitTest();

      private:
        std::string FolderPath;       
        std::string SandboxPath;
        std::string PerfectPath;
        boost::posix_time::ptime TimeStamp;
        
        bool getRootFolder(const std::string & path);
        bool isRootFolder(const std::string & path);
        void getSandboxTimeStamp();
        void computePerfectPath();
    };
#include "private/Sandbox.cpp"
}

#endif
