#ifndef ZFSUtils_hpp_
#define ZFSUtils_hpp_

#include "base/FileSystemUtilities.hpp"
#include "base/Basic.hpp"
#include "sbtools/Utilities.hpp"
#include "sbtools/FindEditedFiles.hpp"

namespace Sbtools
{
    class ZFSUtils
    {
      public:
        explicit ZFSUtils (const std::string & masterSandbox, const std::vector<std::string> & sandboxes, const std::string & perfectArea);
        ~ZFSUtils ();
        
        void sync();
        void createClones(const std::vector<std::string> & sandbox);
        void destroyClones(const std::vector<std::string> & sandbox);
        
      private:
        std::string SandboxPath; // Will be the current folder
        std::string SandboxZFSPath; // The ZFS path, for example local-ssd/sandbox
        std::string PerfectArea; // The perfect area want to sync with for example Aslrtw
        std::string MasterSandbox; // The master sandbox's name, for example Aslrtw
        std::vector<std::string> Sandboxes; // List of clones
        std::vector<std::string> BackupFolders; // List of backup for all clones if they are exist.

        void info();
        const std::string syncCommand(const std::string & sandbox);
        const std::string createClonesCommand(const std::vector<std::string> & sandboxes);
        const std::string destroyClonesCommand(const std::vector<std::string> & sandboxes);
        const std::string regenerateDatabase(const std::vector<std::string> & sandboxes);
        const std::string buildMasterSandboxCommand();
        void generateFindEditedFileDatabase(const std::vector<std::string> & sandboxes);        
    };
#include "private/ZFSUtils.cpp"
}
#endif
