#ifndef SyncSandbox_hpp_
#define SyncSandbox_hpp_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Utilities.hpp"
#include "FindEditedFiles.hpp"
#include "base/Basic.hpp"

namespace Sbtools
{
    class SyncSandbox
    {
      public:
        SyncSandbox(const std::string & SandboxName, const std::vector< std::string > & platforms, const std::string & perfectArea);
        ~SyncSandbox();
        
        void sync();

      private:
        const std::string & SandboxName;
        const std::vector< std::string > & Platforms;
        const std::string & PerfectArea;

        static const std::string BatTreeFileName;
        static const std::string SyncOptFileName;

        void createBattreeFile(const std::string & sandboxPath);
        void createConfigFile(const std::string & sandboxPath);
        void info();
    };
#include "private/SyncSandbox.cpp"
}

#endif
