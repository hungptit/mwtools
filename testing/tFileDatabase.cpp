#include <vector>
#include <map>
#include <boost/unordered_map.hpp>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

#include "base/FileFinder.hpp"
#include "base/Basic.hpp"
#include "base/FileSystemUtilities.hpp"
#include "gtest/gtest.h"

#include "Poco/Data/Common.h"
#include "Poco/Data/SQLite/Connector.h"

namespace Tools
{
    void save()
    {
        using namespace Poco::Data;
        SQLite::Connector::registerConnector();
        Session ses("SQLite", "sample.db");
        SQLite::Connector::unregisterConnector();
    }
    

    template <typename SearchStrategy>
    class FileDatabaseCreator
    {
      public:
        typedef std::vector< std::string > FileNameContainer;
        typedef std::vector< boost::filesystem::perms > FileAttributeContainer;

        FileDatabaseCreator (SearchStrategy & sobj) : StrategyObject(sobj) {}
        
        size_t search(const std::string & folderName)
        {
            boost::filesystem::path currentFolder(folderName);
            boost::filesystem::recursive_directory_iterator endIter;
            boost::filesystem::recursive_directory_iterator dirIter(currentFolder);
            for( ; dirIter != endIter ; ++dirIter)
            {
                boost::filesystem::file_status fs = dirIter->status();
                if (StrategyObject.isValidFile(fs))
                {
                    if (StrategyObject.isValidFileExtension(dirIter->path().extension().string()))
                    {
                        const std::string fileName = dirIter->path().string();
                        if (StrategyObject.isValidFileName(fileName))
                        {
                            FileNames.push_back(fileName);
                            FileAttributes.push_back(fs.permissions());
                        }
                    }
                }
            }
            return FileNames.size();
        }

        void print()
        {
            FileNameContainer::const_iterator it1;
            FileAttributeContainer::const_iterator it2;
            for (it1 = FileNames.begin(), it2 = FileAttributes.begin(); 
                 it1 != FileNames.end(), it2 != FileAttributes.end(); ++it1, ++it2)
            {
                std::cout << (*it1) << std::endl;
            }
            std::cout << FileNames.size() << std::endl;
        }

        
        const FileNameContainer & getFileNames() {return FileNames;}
        const FileAttributeContainer & getFileAttributes() {return FileAttributes;}
        

        void write();
        void read();

      private:
        SearchStrategy & StrategyObject;
        std::vector< std::string > FileNames;
        std::vector< boost::filesystem::perms > FileAttributes;
    };
}



template <typename SearchStrategy>
size_t find(const std::string & folderPath, bool verbose = false)
{
    typedef typename Tools::FileDatabaseCreator<SearchStrategy> Finder;
    
    SearchStrategy p;
    Finder finder(p);
    size_t fileNum = finder.search(folderPath);
    std::cout << "Number of files: " << fileNum << std::endl;
    if (verbose) finder.print();
    const typename Finder::FileNameContainer & data = finder.getFileNames();
   
    // Save file information to database
    // {
    //     using namespace Poco::Data;
    //     using namespace Poco::Data::Keywords;
    //     SQLite::Connector::registerConnector();
    //     Session ses("SQLite", "sample.db");
    //     ses << "CREATE TABLE WritableFiles (id integer primary key, FileName text not null, FilePermisison integer, unique(FileName))", now;
    //     ses << "CREATE UNIQUE INDEX FileNameIndex ON WritableFiles(FileName ASC)", now;
    //     SQLite::Connector::unregisterConnector();
    // }

    return fileNum;
}


int main()
{
    std::cout << "Number of items: " << find<Tools::DefaultSearchEditedFileStrategy>("/local-ssd/sandbox/advisor/matlab/", true) << std::endl;
    return 0;
}
