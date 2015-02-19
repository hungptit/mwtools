
// TODO: Make this better using boost filesystem
void copyFiles(const std::string & srcFolder, const std::string & desFolder)
{
    std::string cmdStr;
    cmdStr = "mkdir -p " + desFolder;
    (void) system(cmdStr.c_str());
    cmdStr = "cp -rv " + srcFolder + Tools::FileSeparator<std::string>::value + "* " + desFolder;
    (void) system(cmdStr.c_str());
}


bool hasPrefixString(const std::string & str, const std::string & prefix)
{
    return str.find(prefix) == 0;
}


void getRunningFarm(std::string & farm, const std::string & sandbox)
{
    const std::string pattern = Tools::FileSeparator<std::string>::value + "local";
    if (hasPrefixString(sandbox, pattern)) 
        farm = "local";
}


std::string getFarmOption(const std::string & farm)
{
    std::string options;

    if (farm == "local")
    {
        options = "";
    }

    else if (farm.empty())
    {
        options = "-autofarm devel ";
    }

    else
    {
        options = "-autofarm devel ";
    }

    return options;
}


const std::string getLogDir(const std::string & seed)
{
    const std::string logDir = " $DEFAULT_SANDBOX" + Tools::FileSeparator<std::string>::value + 
        "logs" + Tools::FileSeparator<std::string>::value + seed + Tools::FileSeparator<std::string>::value + getTimeStampString();
    return logDir;
}


std::string getBackupDir(const std::string & seed)
{
    std::string strBuf = seed;
    replaceSubstring(strBuf, " ", "_");
    std::string dirStr = strBuf + Tools::FileSeparator<std::string>::value + getTimeStampString();
    return dirStr;
}


std::string getSbruntestsCommand(const std::string & farm,
                                 const std::string & listFile,
                                 const std::string & runFolder)
{
    std::string command = "sbruntests " + Sbtools::getFarmOption(farm);
    std::string seed;

    if (!runFolder.empty())
    {
        seed += "runtests";
        command += "-runallunder_mp " + runFolder;
    }

    else if (!listFile.empty())
    {
        if (hasPrefixString(listFile, "submit")) // If given file has prefix "submit" then it is a submit file
        {
            std::string testSuite = "Aslrtw";
            command += "-F " + listFile + " -testsuites " + testSuite;
        }

        else
        {
            command += "-testsuites " + listFile;
            seed += "runtests";
        }
    }

    else
    {
        throw("Invalid options");
    }

    command += " -testlogarea " + Sbtools::getLogDir(seed);

    return command;
}


void replaceSubstring(std::string & stringBuf, const std::string & subString, const std::string & replaceString)
{
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = stringBuf.find(subString, pos);
        if (pos != std::string::npos)
            stringBuf.replace(pos, subString.length(), replaceString);
    }
}


std::string readFile(const std::string & fileName)
{
    std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!in) std::runtime_error("Cannot read the content of \"" + fileName + "\". Error code" + boost::lexical_cast<std::string>(errno));

    // Read file content into a string
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
}


const std::string backupSandbox(const std::string & comment)
{
    const std::string str = Sbtools::getBackupDir(comment);
    const std::string cmdStr = "sbbackup -l " + str + " -r " + Sbtools::Resources<std::string>::BackupDirectory;
    Tools::run(cmdStr);
    // std::cout << "Command: " << cmdStr << std::endl;
    std::string backupDir = Sbtools::Resources<std::string>::BackupDirectory + Tools::FileSeparator<std::string>::value + str;
    return Tools::getAbslutePath(backupDir);
}


const std::string generateGlobalDatabaseCommand(const std::string & sandboxPath)
{
    std::string cmdStr;
    if (!sandboxPath.empty())
    {
        cmdStr += "cd " + sandboxPath + ";";
    }
        
    cmdStr +=  + "sbglobal -gentags; sblocate -gendb; sbid -gendb; sbgentbxcache;";
    return cmdStr;
}


const std::string getDatabaseFileName(const std::string & sandboxPath)
{
    const std::string fileName = sandboxPath + Tools::FileSeparator<std::string>::value +
        Resources<std::string>::SbtoolsFolder + Tools::FileSeparator<std::string>::value + 
        Resources<std::string>::DatabaseFileName;
    return fileName;
}
