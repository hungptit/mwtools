#ifndef TestDatabase_hpp_
#define TestDatabase_hpp_

#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <numeric>
#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"

#include "base/Resources.hpp"

namespace Sbtools
{
    class TestDatabase
    {
      public:
        TestDatabase() {}
        ~TestDatabase() {}
        
        const bool hasRunTimeInfo(const std::string & fileName);
        const double getRunTime(const std::string & fileName);
        void setRunTime(const std::string & fileName, const double t);
        const bool read(const std::string & fileName);
        void write(const std::string & fileName);
        void print();
        const bool unitTest();

      private:
        std::map< std::string, std::vector<double> > Database;
        const size_t getRunTimeFromString(const std::string & strBuf, std::vector<double> & runTimes);
        void getRunTimeString(const std::vector<double> & runTimes, std::string & strBuf);
    };
    #include "private/TestDatabase.cpp"
}

#endif
