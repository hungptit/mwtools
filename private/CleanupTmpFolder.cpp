CleanupTmpFolder::CleanupTmpFolder(const std::string & tmpFolder)
{
    TmpFolder = tmpFolder;
}


CleanupTmpFolder::~CleanupTmpFolder()
{
    // Do nothing
}


void CleanupTmpFolder::clean()
{
    boost::filesystem::path currentFolder(TmpFolder);
    boost::filesystem::recursive_directory_iterator endIter;
    boost::filesystem::recursive_directory_iterator dirIter(currentFolder);
    const std::string searchPattern = TmpFolder + "file";
    for( ; dirIter != endIter ; ++dirIter)
    {
        if (boost::filesystem::is_regular_file(*dirIter))
        {
            const std::string fileName = dirIter->path().string();
            if (isDeletable(fileName, searchPattern))
            {               
                // std::cout << "Remove: " << fileName << std::endl;
                boost::filesystem::remove(dirIter->path());
            }
        }
    }
}


bool CleanupTmpFolder::isDeletable(const std::string & fileName, const std::string & searchPattern)
{
    return fileName.find(searchPattern) != std::string::npos;
}
