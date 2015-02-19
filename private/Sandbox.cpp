Sandbox::Sandbox(const std::string & folderPath) : 
    FolderPath(folderPath), SandboxPath(), PerfectPath(), TimeStamp() 
{
    getRootFolder(FolderPath);
    getSandboxPath();
    getSandboxTimeStamp();
}


Sandbox::~Sandbox() 
{
    // Do nothing
}


const std::string & Sandbox::getSandboxPath() const
{
    return SandboxPath;
}


bool Sandbox::getRootFolder(const std::string & path)
{
    const boost::filesystem::path currentFolder(path);
    if (isRootFolder(path))
    {
        SandboxPath = path;
        computePerfectPath();
        getSandboxTimeStamp();
        return true;
    }
    else
    {
        if (path == currentFolder.root_path().string())
        {
            return false;
        }
        else
        {
            const std::string parentPath = currentFolder.parent_path().string();
            return getRootFolder(parentPath);
        }
    }

}


bool Sandbox::isRootFolder(const std::string & path)
{
    const std::string fileName = path + Tools::FileSeparator<std::string>::value + ".last_sunc";
    boost::filesystem::path currentFile(fileName);
    return boost::filesystem::is_regular_file(currentFile);
}


void Sandbox::getSandboxTimeStamp()
{
    boost::match_results<std::string::const_iterator> what;
    const boost::match_flag_type flags = boost::match_default;
    std::string::const_iterator begin = PerfectPath.begin();
    std::string::const_iterator end = PerfectPath.end();
    boost::regex expression("[0-9]{4}.[0-9]{2}.[0-9]{2}.h[0-9]{2}m[0-9]{2}s[0-9]{2}");   
    std::string timeString;
    if (boost::regex_search(begin, end, what, expression, flags))
    {
        timeString = what[0];
        Sbtools::replaceSubstring(timeString, "_", "-");        
        Sbtools::replaceSubstring(timeString, "-h", " ");
        Sbtools::replaceSubstring(timeString, "m", ":");
        Sbtools::replaceSubstring(timeString, "s", ":");
        TimeStamp = boost::posix_time::time_from_string(timeString);
    }
}


bool Sandbox::unitTest()
{
    bool flag = true;

    {
        const std::string fileSep = Tools::FileSeparator<std::string>::value;
        const std::string folderPath = "data" + fileSep + "matlab" + fileSep + "test";
        flag &= getRootFolder(folderPath);
        std::cout << "flag = " << flag << std::endl;
        flag &= getSandboxPath() == "data";
        std::cout << "flag = " << flag << std::endl;
        std::cout << "getSandboxPath() = " << getSandboxPath() << std::endl;
        info();
        std::string ts("2013-Apr-14 15:27:12");
        boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
        flag &= t == TimeStamp;
    }

#ifdef __linux
    {
        SandboxPath = Tools::FileSeparator<std::string>::value + "local" + Tools::FileSeparator<std::string>::value + "sandbox";
        flag &= isLocal();
        
        SandboxPath = Tools::FileSeparator<std::string>::value + "local-ssd" + Tools::FileSeparator<std::string>::value + "sandbox";
        flag &= isLocal();
    }
#endif

    return flag;
}


void Sandbox::info() const
{
    std::cout << "Sandbox path: " << SandboxPath << std::endl;
    std::cout << "Perfect path: " << PerfectPath << std::endl;
    std::cout << "Sandbox time: " << TimeStamp << std::endl;
}


bool Sandbox::isLocal()
{
    const std::string fileSep = Tools::FileSeparator<std::string>::value;

    // TODO: What is the name of the local folder in Window?
    const std::string localFolder = fileSep + "local" + fileSep;
    const std::string localSSDFolder = fileSep + "local-ssd" + fileSep;

    const bool flag = (SandboxPath.find(localFolder, 0) == 0) || (SandboxPath.find(localSSDFolder, 0) == 0);
    return flag;
}


void Sandbox::computePerfectPath()
{
    std::ifstream inputFile;
    const std::string fileName = SandboxPath + Tools::FileSeparator<std::string>::value + ".last_sunc";
    inputFile.open(fileName.c_str());
    std::string lineBuf;
    if (inputFile.is_open()) {
        while (!inputFile.eof()) 
        {
            getline(inputFile, lineBuf);
            if (!lineBuf.empty()) 
            {
                PerfectPath = lineBuf;
                break;
            }
        }
    }
    inputFile.close();
}
