#ifndef sbtool_resources_hpp
#define sbtool_resources_hpp

// #include "utils/Basic.hpp"

namespace Tools
{
    template <typename T>
    struct SandboxResources
    {
        typedef T value_type;
        static const T SbtoolsFolder;
        static const T CVSFolder;
        static const T DerivedFolder;
        static const T DatabaseFileName;  
        static const T NetworkSandbox;     
        static const T BackupDirectory;
        static const T LatestSnapshotName;
        static const T SbSmartBuildCommand;
        static const T ReviewBoardCommand;
        static const T SbCommand;
        static const T SbCheckCommand;
        static const T WebViewer;
        static const T Gecko;
        static const T GeckoSearch;
        static const T Batcave;
        static const T CodeSearch;
        static const T Outlook;
        static const T InsidePage;
        static const T WikiPage;
        static const T Google;
        static const T ISOCppPage;
        static const T AtTaskPage;
        static const T Perforce;
        static const T PerforceDatabase;
    };   
    #include "private/Resources.cpp"
}
#endif
