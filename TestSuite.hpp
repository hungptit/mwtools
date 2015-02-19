#ifndef TESTSUITE_HPP_
#define TESTSUITE_HPP_

#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <fstream>
#include <string>

#include <boost/filesystem.hpp>
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"
#include "boost/program_options.hpp"
#include <boost/unordered_map.hpp>
#include "base/FileFinder.hpp"
#include "sbtools/Resources.hpp"

namespace Sbtools 
{
    class TestSuite
    {
      public:
        TestSuite(); 
        ~TestSuite();

        // using defaults for 
        // TestSuite(TestSuite&)
        // operator=(TestSuite&)

        const std::vector<std::string> & getTestSuite() const { return FileList; }
        size_t search(const std::string & folderName, const std::string & searchPatternString = "/t[^/]*.m$");
        void write(const std::string & fileName);
        void read(const std::string & fileName);
        void print() const;       
        void info() const;
        void clear();
        const std::string getRegexpString(const std::vector< std::string > & patternStrings);        
        bool unitTest();
        
      private:
        std::vector< std::string > FileList;              
        const bool isViewableFile(const std::string & fileName) const;
    };
};

#include "private/TestSuite.cpp"


#endif
