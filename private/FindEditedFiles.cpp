const bool FindEditedFiles::isWritableFile(const boost::filesystem::perms fileMode, const boost::filesystem::perms writableMode) const
{
    return (fileMode & writableMode) > 0;
}

void FindEditedFiles::initValidFileExtensions()
{
    ValidFileExtensions['.m'] = true;
    ValidFileExtensions['.mat'] = true;
    ValidFileExtensions['.cpp'] = true;
    ValidFileExtensions['.hpp'] = true;
    ValidFileExtensions['.h'] = true;
    ValidFileExtensions['.xml'] = true;
}

const bool FindEditedFiles::isValidFileExtension(const std::string & fileExtension)
{
    return ValidFileExtensions.find(fileExtension) != ValidFileExtensions.end();
}


const bool FindEditedFiles::isValidFile(const std::string & fileName) const
{
    const bool flag =
        (fileName.find(Sbtools::Resources<std::string>::SbtoolsFolder) == std::string::npos) && // Exclude .sbtools folder
        (fileName.find(Sbtools::Resources<std::string>::CVSFolder) == std::string::npos) && // Exclude CVS folder
        (fileName.find(Sbtools::Resources<std::string>::DerivedFolder) == std::string::npos);    // Exclude derived folder
    return flag;
}


const bool FindEditedFiles::isViewableFile(const std::string & fileName) const
{
    const bool flag =
        (fileName.find("~") == std::string::npos) &&
        (fileName.find("#") == std::string::npos) &&
        (fileName.find(".rps") == std::string::npos) &&
        (fileName.find(".dbg") == std::string::npos) &&
        (fileName.find("/en/") == std::string::npos) &&
        (fileName.find("/ja/") == std::string::npos) &&
        (fileName.find("TAGS") == std::string::npos) &&
        (fileName.find("/ja_JP/") == std::string::npos) &&
        (fileName.find("/zh_CN/") == std::string::npos) &&
        (fileName.find("TAGS") == std::string::npos) &&
        (fileName.find("TAGS") == std::string::npos) &&
        (fileName.find(".gen") == std::string::npos) &&
        (fileName.find(".class") == std::string::npos) &&
        (fileName.find(".tstamp") == std::string::npos) &&
        (fileName.find(".xcconfig") == std::string::npos) &&
        (fileName.find(".pbxproj") == std::string::npos) &&
        (fileName.find(".properties") == std::string::npos) &&
        (fileName.find("/simfeatures/") == std::string::npos) &&
        (fileName.find(".lbrepos_targets_glnxa64") == std::string::npos) &&
        (fileName.find(".lbrepos_targets_win64") == std::string::npos) &&
        (fileName.find(".lbrepos_targets_win32") == std::string::npos) &&
        (fileName.find(".lbrepos_targets_maci64") == std::string::npos) &&
        (fileName.find(".gdb_history") == std::string::npos);
    return flag;
}


size_t FindEditedFiles::searchWritableFiles(const std::string & folderName)
{
    boost::filesystem::path currentFolder(Tools::getAbslutePath(folderName));
    boost::filesystem::recursive_directory_iterator endIter;
    boost::filesystem::recursive_directory_iterator dirIter(currentFolder);
    const boost::filesystem::perms writableMode =
        boost::filesystem::owner_write |
        boost::filesystem::group_write |
        boost::filesystem::others_write;

    for( ; dirIter != endIter ; ++dirIter)
    {
        if (boost::filesystem::is_regular_file(*dirIter))
        {
            const boost::filesystem::perms fileMode = dirIter->status().permissions();
            if (isWritableFile(fileMode, writableMode))
            {
                const std::string filePath = dirIter->path().string();
                const std::string fileExtension = dirIter->path().extension().string();
                if (isValidFileExtension(fileExtension) && 
                    isValidFile(filePath) && 
                    (WritableFileLookupTable.find(filePath) == WritableFileLookupTable.end()))
                    WritableFiles.push_back(std::make_pair(filePath, fileMode)); // Keep this file
            }
        }
    }
    return WritableFiles.size();
}


void FindEditedFiles::print() const
{
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator it;
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator end = WritableFiles.end();
    size_t numberOfFiles = 0;
    for (it = WritableFiles.begin(); it != end; ++it)
    {
        const std::string fileName = it->first;
        if (isViewableFile(fileName))
        {
            std::cout << fileName << std::endl;
            numberOfFiles ++;
        }
    }
    std::cout << "Number of writable files: " << numberOfFiles << std::endl;
}


void FindEditedFiles::viewDatabase(const std::string & title) const
{
    if (!title.empty()) std::cout << title << std::endl;
    boost::unordered_map<std::string, boost::filesystem::perms>::const_iterator it;
    boost::unordered_map<std::string, boost::filesystem::perms>::const_iterator end = WritableFileLookupTable.end();
    for (it = WritableFileLookupTable.begin(); it != end; ++it)
        std::cout << it->first << ", " << it->second << std::endl;
    std::cout << "Number of files in database: " << WritableFileLookupTable.size() << std::endl;
}


void FindEditedFiles::write(const std::string & fileName)
{
    std::ofstream outputFile;
    outputFile.open(fileName.c_str());
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator it;
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator end = WritableFiles.end();
    for (it = WritableFiles.begin(); it != end; ++it)
    {
        outputFile << it->first << std::endl;
    }
    outputFile.close();
}


// @todo Move unit test out of this class. Use the adapter pattern for wrting out unit test?
bool FindEditedFiles::unitTest()
{
    bool flag = true;

    {
        std::cout << "Make sure that the database is imported correctly ===>" << std::endl;
        const std::string fileName = 
            "data" + Tools::FileSeparator<std::string>::value + 
            Resources<std::string>::SbtoolsFolder + Tools::FileSeparator<std::string>::value +
            Resources<std::string>::DatabaseFileName;
        std::cout << "Database file: " << fileName << std::endl;
        importWritableFileDatabase(fileName);
        viewDatabase("Test database");
        flag &= WritableFileLookupTable.size() == 19;
        std::cout << "Number of writable file lookup table -> " << WritableFileLookupTable.size() << std::endl;
    }


    {
        std::cout << "Make sure that the database is written correctly ===>" << std::endl;
        const std::string folderPath = "data";
        const std::string fullFolderPath = Tools::getAbslutePath(folderPath);
        const size_t fileNum = searchWritableFiles(fullFolderPath);
        std::cout << "Number of writable files found -> " << WritableFiles.size() << std::endl;
        flag &= WritableFiles.size() == 34;

        createWritableFileDatabase(Resources<std::string>::DatabaseFileName);
        WritableFiles.clear();
        WritableFileLookupTable.clear();
        importWritableFileDatabase(Resources<std::string>::DatabaseFileName);
        viewDatabase("Test database");

        std::cout << "Number of writable files in the lookup table -> " << WritableFileLookupTable.size() << std::endl;
        flag &= WritableFileLookupTable.size() == 34;
        WritableFileLookupTable.clear();
    }


    {
        std::cout << "Test the number of writable files ===>" << std::endl;
        const std::string folderPath = "data";
        const std::string fullFolderPath = Tools::getAbslutePath(folderPath);
        const size_t fileNum = searchWritableFiles(fullFolderPath);
        flag &= WritableFiles.size() == 34;
        std::cout << "Number of writable files -> " << WritableFiles.size() << std::endl;
        print();
    }

    return flag;
}


void FindEditedFiles::createWritableFileDatabase(const std::string & fileName)
{
    std::ofstream outputFile;
    outputFile.open(fileName.c_str());
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator it;
    std::vector< std::pair<std::string, boost::filesystem::perms> >::const_iterator end = WritableFiles.end();
    for (it = WritableFiles.begin(); it != end; ++it)
    {
        outputFile << it->first << "," << it->second << std::endl;
    }
    outputFile.close();
}


void FindEditedFiles::importWritableFileDatabase(const std::string & fileName)
{
    typedef boost::filesystem::perms FilePermision;
    std::ifstream inputFile;
    inputFile.open(fileName.c_str());
    std::string lineBuf;
    if (inputFile.is_open())
    {
        while (!inputFile.eof())
        {
            getline(inputFile, lineBuf);
            if (!lineBuf.empty())
            {
                const size_t pos = lineBuf.rfind(',');
                const std::string key = lineBuf.substr(0, pos);
                const std::string accessMode = lineBuf.substr(pos + 1);
                const FilePermision fileMode = static_cast<FilePermision>(boost::lexical_cast<int>(accessMode));
                WritableFileLookupTable[key] = fileMode;
            }
        }
    }
    inputFile.close();
}

