#ifndef CleanupTmpFolder_hpp_
#define CleanupTmpFolder_hpp_

#include <string>

#include <boost/filesystem.hpp>
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

namespace Tools {
class CleanupTmpFolder {
  public:
    CleanupTmpFolder() : TmpFolder(boost::filesystem::temp_directory_path()){};

    void clean() {
        boost::filesystem::recursive_directory_iterator endIter;
        boost::filesystem::recursive_directory_iterator dirIter(TmpFolder);
        const std::string searchPattern =
            (TmpFolder / boost::filesystem::path("file")).string();
        for (; dirIter != endIter; ++dirIter) {
            if (boost::filesystem::is_regular_file(*dirIter)) {
                const std::string fileName = dirIter->path().string();
                if (fileName.find(searchPattern) != std::string::npos) {
                    boost::filesystem::remove(dirIter->path());
                }
            }
        }
    }

  private:
    boost::filesystem::path TmpFolder;
};
}
#endif
