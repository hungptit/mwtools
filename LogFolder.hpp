#ifndef LogFolder_hpp_
#define LogFolder_hpp_

#include <string>
#include "boost/filesystem.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/TimeUtilities.hpp"
#include "utils/Resources.hpp"

namespace Tools {
class LogFolder {
  public:
    LogFolder::LogFolder(const boost::filesystem::path &parentFolder)
        : ParentFolder(parentFolder),
          FolderPath(ParentFolder /
                     boost::filesystem::path(Sbtools::getTimeStampString())) {
        if (!boost::filesystem::exists(RootFolderPath)) {
            boost::filesystem::create_directories(RootFolderPath);
        }

        boost::filesystem::create_directories(FolderPath);
        assert(boost::filesystem::exists(FolderPath));

        const auto &getPath() const { return CurrentFolder; }

      private:
        boost::filesystem::path ParentFolder;
        boost::filesystem::path CurrentFolder;
    };
}

#endif
