#ifndef CleanupTmpFolder_hpp_
#define CleanupTmpFolder_hpp_

#include <string>

#include <boost/filesystem.hpp>
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"

namespace Sbtools
{
    class CleanupTmpFolder
    {
      public:
        CleanupTmpFolder(const std::string & tmpFolder = "/tmp/");
        ~CleanupTmpFolder();

        void clean();

      private:
        std::string TmpFolder;
        bool isDeletable(const std::string & fileName, const std::string & searchPattern);
    };
#include "private/CleanupTmpFolder.cpp"
}
#endif
