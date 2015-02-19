#ifndef FileUtilities_hpp_
#define FileUtilities_hpp_
namespace Tools
{
    const bool createDirectory(const std::string & folderName);
    const bool isRegularFile(const std::string & str);    
    const bool isDirectory(const std::string & str);      
    const std::string getCurrentFolder();
}
#endif
