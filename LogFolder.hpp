#ifndef LogFolder_hpp_
#define LogFolder_hpp_

#include <string>
#include "boost/filesystem.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"
#include "utils/Utils.hpp"
#include <assert.h>


namespace Tools {
    class LogFolder {
      public:
        LogFolder(const boost::filesystem::path &parentFolder)
            : ParentFolder(parentFolder)
        {
            CurrentFolder = ParentFolder / boost::filesystem::path(Utils::getTimeStampString());
            if (!boost::filesystem::exists(ParentFolder)) {
                boost::filesystem::create_directories(ParentFolder);
            }
            
            boost::filesystem::create_directories(CurrentFolder);
            assert(boost::filesystem::exists(CurrentFolder));
        }
        
        const auto &getPath() const { return CurrentFolder; }
        
          private:
            boost::filesystem::path ParentFolder;
            boost::filesystem::path CurrentFolder;
        };
    }
#endif
