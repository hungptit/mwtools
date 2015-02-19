LogFolder::LogFolder(const std::string & rootPath) 
    : RootFolderPath(rootPath), FolderPath()
{
    Tools::createDirectory(RootFolderPath); // Make sure that we the RootFolderPath is exist otherwise the next function call will throw.
    createFolderPathString();
    create();
}


LogFolder::~LogFolder() 
{
    // Do nothing
}


void LogFolder::createFolderPathString()
{
    FolderPath = Tools::getAbslutePath(RootFolderPath) + Tools::FileSeparator<std::string>::value + Sbtools::getTimeStampString();
}


const bool LogFolder::create()
{
    assert(!FolderPath.empty());
    return Tools::createDirectory(FolderPath);
}


const std::string & LogFolder::getFolderPath() const
{
    return FolderPath;
}


const bool LogFolder::unitTest()
{
    createFolderPathString();
    const std::string folderPath = getFolderPath();
    std::cout << folderPath << std::endl;    
    return create();
}
