#ifndef LogParser_hpp_ 
#define LogParser_hpp_

#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <fstream>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"

#include "Utilities.hpp"

namespace Tools
{
    enum TestStatus {TEST_PASSED = 0, 
                     TEST_PASSED_WITH_WARNING = 1, 
                     TEST_PASSED_WITH_DROOL= 2, 
                     TEST_FAILED = 3, 
                     TEST_FAILED_WITH_ASSERTION= 4, 
                     TEST_FAILED_WITH_SEGV= 5, 
                     TEST_FAILED_WITH_OTHER_CAUSES = 6,
                     TEST_INCOMPLETE = 7};
    
    const std::string getTestStatusString(const TestStatus status);


    enum LogSearchPattern {TEST_PASSED_STRING, 
                           TEST_FAILED_STRING, 
                           TEST_ASSERTION_STRING,
                           TEST_NOFAILURE_STRING,
                           TEST_SEGV_STRING, 
                           TEST_DROOLING_STRING, 
                           TEST_SUMMARY_STRING,
                           LOG_FILE_EXTENSION};

    
    // Trait class for LogSearchPattern enum
    template <LogSearchPattern T>
    struct LogSearchPatternTrait
    {
        typedef LogSearchPattern value_type;
        static const std::string value;
    };


    class LogParser
    {
      public:
        LogParser(const std::string & logFolder);
        ~LogParser();
        void run();
        void report(const std::string & fileName = "");

        bool unitTest();
        
      private:
        std::string FolderPath;
        std::vector<std::string> LogFiles;
        std::vector<std::string> TestFiles;
        std::vector<TestStatus> Status;

        static const std::string SummaryPattern;
        static const std::string PassedPattern;
        static const std::string FailedPattern;
        static const std::string DroolingPattern;
        static const std::string AssertionPattern;
        static const std::string SegvPattern;
        static const std::string LogFileExtension;
        
        void search(const std::string & logFolder);
        const bool isLogFile(const std::string & fileName);      
        TestStatus parse(const std::string & fileName);
        void readTestFiles(const std::string & fileName);
    };
#include "private/LogParser.cpp"
}
#endif
