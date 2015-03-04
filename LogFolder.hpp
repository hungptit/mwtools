#ifndef LogFolder_hpp_
#define LogFolder_hpp_

#include <string>
#include "boost/filesystem.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/TimeUtilities.hpp"
#include "utils/Resources.hpp"

namespace Tools 
{
    class LogFolder
    {
      public:
        LogFolder(const std::string & rootPath);
        ~LogFolder();        
        bool create();
        const std::string & getFolderPath() const;
        bool unitTest();
        
      private:
        std::string RootFolderPath;
        std::string FolderPath;
        void createFolderPathString();        
    };
#include "private/LogFolder.cpp"    
}

#endif
