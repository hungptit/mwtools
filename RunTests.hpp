#ifndef RunTest_hpp_
#define RunTest_hpp_

#include <string>
#include "TestSuite.hpp"
#include "Sandbox.hpp"
#include "LogFolder.hpp"
#include "LogParser.hpp"
#include "TestDatabase.hpp"
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace Tools
{
    class RunTests
    {
      public:
        RunTests(const std::string & sandboxPath, const std::string & fileName, const std::string & logFolder, const int numberOfThreads);
        ~RunTests();
        void run();

      private:
        Sandbox CurrentSandbox;
        LogFolder CurrentLogFolder;
        TestSuite TestFiles;
        TestDatabase Database;
        int NumberOfThreads;

        static void exec(const std::string & fileName, const std::string & logFile, const std::string & sandboxPath);
        static std::string createRunCommand(const std::string & testFile, const std::string & sandboxPath);
    };
#include "private/RunTests.cpp"
}
#endif
