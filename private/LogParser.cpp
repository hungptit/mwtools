template<> const std::string LogSearchPatternTrait<TEST_PASSED_STRING>::value = "PASSED";
template<> const std::string LogSearchPatternTrait<TEST_FAILED_STRING>::value = "FAILED";
template<> const std::string LogSearchPatternTrait<LOG_FILE_EXTENSION>::value = ".log";
template<> const std::string LogSearchPatternTrait<TEST_SUMMARY_STRING>::value = "*** SUMMARY OF TEST FILES ***";
template<> const std::string LogSearchPatternTrait<TEST_NOFAILURE_STRING>::value = "No failures.";
template<> const std::string LogSearchPatternTrait<TEST_ASSERTION_STRING>::value = "assertion";
template<> const std::string LogSearchPatternTrait<TEST_SEGV_STRING>::value = "segv";


LogParser::LogParser(const std::string & logFolder) : FolderPath(logFolder), LogFiles(), TestFiles(), Status()
{
    assert(!FolderPath.empty());
}


LogParser::~LogParser()
{
    // Do nothing
}


void LogParser::search(const std::string & logFolder)
{
    boost::filesystem::path currentFolder(logFolder);
    boost::filesystem::recursive_directory_iterator endIter;
    boost::filesystem::recursive_directory_iterator dirIter(currentFolder);
    for( ; dirIter != endIter ; ++dirIter)
    {
        if (boost::filesystem::is_regular_file(*dirIter))
        {
            const std::string fileExtension = dirIter->path().extension().string();
            if (isLogFile(fileExtension))
            {
                const std::string fileName = dirIter->path().string();
                LogFiles.push_back(fileName);
            }
        }
    }

    // Make sure that Status vector has the same size as that of Files vector.
    Status.resize(LogFiles.size());
}


void LogParser::run()
{
    std::cout << "Logging folder: " << FolderPath << std::endl;
    
    // Read the test file list
    readTestFiles(FolderPath + Tools::FileSeparator<std::string>::value + "list.txt");

    // Search for all log files
    search(FolderPath);         // Get the list of all log files
    
    assert(TestFiles.size() == LogFiles.size());
    
    std::vector<std::string>::const_iterator fileIter;
    std::vector<TestStatus>::iterator statusIter = Status.begin();
    for (fileIter = LogFiles.begin(); fileIter != LogFiles.end(); ++fileIter, ++statusIter)
    {
        *statusIter = parse(*fileIter);
    }
}


TestStatus LogParser::parse(const std::string & fileName)
{
    TestStatus status = Sbtools::TEST_FAILED;
    std::string fileBuffer = Sbtools::readFile(fileName);

    size_t start = fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_SUMMARY_STRING>::value);
    if (start == std::string::npos)
    {
        // The test is failed then dive in to get more detail information 
        if (fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_ASSERTION_STRING>::value, start) != std::string::npos)
        {
            status = Sbtools::TEST_FAILED_WITH_ASSERTION;
        }
        else if (fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_SEGV_STRING>::value, start) != std::string::npos)
        {
            status = Sbtools::TEST_FAILED_WITH_SEGV;            
        }
        else 
        {
            status = Sbtools::TEST_INCOMPLETE;
        }
    }
    else
    {
        if ((fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_PASSED_STRING>::value, start) != std::string::npos) || 
            (fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_NOFAILURE_STRING>::value, start) != std::string::npos))
        {
            // Passed
            status = Sbtools::TEST_PASSED;
        }
        else
        {
            if (fileBuffer.find(LogSearchPatternTrait<Sbtools::TEST_FAILED_STRING>::value, start) != std::string::npos)
            {
                // Failed
                status = Sbtools::TEST_FAILED;
            }
            else
            {
                assert(false);  // Do not know what to do here
            }
        }
    }
    return status;
}


const bool LogParser::unitTest()
{
    bool flag = true;
    run();

    flag &= TestFiles.size() == 19;
    flag &= LogFiles.size() == 19;
    std::cout << "Number of log files: " << LogFiles.size() << std::endl;
    std::cout << "Number of test files: " << TestFiles.size() << std::endl;
    report("");    
    return flag;
}


// Create a report from parsing results.
void LogParser::report(const std::string & fileName)
{
    if (fileName.empty())
    {
        std::vector<std::string>::const_iterator iter;
        std::vector<TestStatus>::const_iterator statusIter = Status.begin();
        for (iter = TestFiles.begin(); iter != TestFiles.end(); ++iter, ++statusIter)
        {
            std::cout << *iter << " -> " << getTestStatusString(*statusIter) << std::endl;
        }
    }
    else                        // Save report to a file based on its extension.
    {
        
    }
}


const bool LogParser::isLogFile(const std::string & extension)
{
    return (extension.compare(LogSearchPatternTrait<Sbtools::LOG_FILE_EXTENSION>::value) == 0);
}


void LogParser::readTestFiles(const std::string & fileName)
{
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
                TestFiles.push_back(lineBuf);
            }
        }
    }
    inputFile.close();
}


const std::string getTestStatusString(const TestStatus status)
{
    std::string value;
    switch (status)
    {
      case Sbtools::TEST_PASSED:
        value = "PASSED";
        break;

      case Sbtools::TEST_FAILED:
        value = "FAILED";
        break;

      case Sbtools::TEST_PASSED_WITH_WARNING:
        value = "PASSED_WITH_WARNING";
        break;

      default:
        value = "Unrecognized TestStatus code!";
        break;
    }
    return value;
}
