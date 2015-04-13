// TODO: Need to support multiple test folders and test files.
RunTests::RunTests(const std::string & sandboxPath, const std::string & testFolder, const std::string & logFolder, const int numberOfThreads) :
    CurrentSandbox(sandboxPath), TestFiles(), CurrentLogFolder(logFolder), NumberOfThreads(numberOfThreads)
{
    CurrentSandbox.info();

    CurrentLogFolder.create();

    // The given testFolder could be either a test folder or a text file which has a list of tests.
    if (Tools::isRegularFile(testFolder))
    {
        TestFiles.read(testFolder);
    }
    else
    {
        TestFiles.search(Tools::getAbslutePath(testFolder));
    }
    
    TestFiles.info();
}


std::string RunTests::createRunCommand(const std::string & testFile, const std::string & sandboxPath)
{
#ifdef _WIN64
    return std::string(
        "call "
        "sb "
        "-nosetmwe -showenv "
        "-priority=low "
        // "-mlprefroot $HOME "
        "-skip-job-archive-check "
        " -s " + sandboxPath + " "
        "-memmgr fastdebug "
        "-timing "
        "-nodesktop -nosplash "
        "-r 'cd(matlabroot);runsuite('" + testFile + "');exit'"
        );
#elif _WIN32
    return std::string(
        "call "
        "sb "
        "-nosetmwe -showenv "
        "-priority=low "
        // "-mlprefroot $HOME "
        "-skip-job-archive-check "
        " -s " + sandboxPath + " "
        "-vmemorylimit=4g "
        "-memmgr fastdebug "
        "-timing "
        "-nodesktop -nosplash "
        "-r 'cd(matlabroot);runsuite('" + testFile + "');exit'"
        );
#else  // Use this command for other OSs (Linux and Mac)
    std::string cmdStr;        
    cmdStr += "sb -nosetmwe -showenv -priority=low -mlprefroot $HOME -skip-job-archive-check -vmemorylimit=4g -memmgr fastdebug -timing -nodesktop -nosplash";
    cmdStr += " -s " + sandboxPath;
    cmdStr += " -r \"cd(matlabroot);runsuite('" + testFile +  "'); exit;\"";
    return cmdStr;
#endif
}


void RunTests::exec(const std::string & fileName, const std::string & logFile, const std::string & sandboxPath)
{
    const std::string cmd = createRunCommand(fileName, sandboxPath) + " > " + logFile; // Direct the output to a log file.
    std::cout << cmd << std::endl;
    system(cmd.c_str());
}


void RunTests::run()
{
    const std::vector< std::string > & list = TestFiles.getTestSuite();
    std::cout << "Number of tests: " << list.size() << std::endl;
    TestFiles.write(CurrentLogFolder.getFolderPath() + Tools::FileSeparator<std::string>::value + "list.txt");

    // Run tests in parallel using threadpool
    {
        boost::threadpool::pool tp(NumberOfThreads);
        std::vector<std::string>::const_iterator it;
        int counter = 0;
        for (it = list.begin(); it != list.end(); ++it, counter ++)
        {
            const std::string testFile = *it;
            const std::string logFile = CurrentLogFolder.getFolderPath() + 
                Tools::FileSeparator<std::string>::value + boost::lexical_cast<std::string>(counter) + ".log";            
            tp.schedule(boost::bind(&RunTests::exec, testFile, logFile, CurrentSandbox.getSandboxPath()));
        }
    }

    // Parse log files and create a report
    {
        std::cout << "Parse the results from the logging folder: " << CurrentLogFolder.getFolderPath() << std::endl;
        LogParser parser(CurrentLogFolder.getFolderPath());
        parser.run();
        parser.report();
    }
}
