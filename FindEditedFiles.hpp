
#ifndef FindEditedFiles_hpp_
#define FindEditedFiles_hpp_

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

#include "base/FileSystemUtilities.hpp"
#include "base/Resources.hpp"
#include "sbtools/Resources.hpp"
#include "base/Basic.hpp"

namespace Sbtools 
{
    class FindEditedFiles
    {
      public:
        FindEditedFiles() : WritableFiles() 
        {
            WritableFiles.reserve(1000);
            initValidFileExtensions();            
        }
        
        ~FindEditedFiles() {}
        
        size_t searchWritableFiles(const std::string & folderName);
        void write(const std::string & fileName);
        void createWritableFileDatabase(const std::string & fileName);
        void importWritableFileDatabase(const std::string & fileName);
        void print() const;
        void viewDatabase(const std::string & title = "") const;
       
      private:
        std::vector< std::pair<std::string, boost::filesystem::perms> > WritableFiles;       
        boost::unordered_map<std::string, boost::filesystem::perms> WritableFileLookupTable;
        boost::unordered_map<std::string, bool> ValidFileExtensions;

        // Private methods
        void initValidFileExtensions();
        const bool isWritableFile(const boost::filesystem::perms fileMode, const boost::filesystem::perms writableMode) const;
        const bool isValidFileExtension(const std::string & fileExtension);
        const bool isValidFile(const std::string & fileName) const;
        const bool isViewableFile(const std::string & fileName) const;
    }; 
    
#include "private/FindEditedFiles.cpp"
};

#endif
