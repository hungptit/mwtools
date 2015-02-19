namespace Sbtools 
{
    TestSuite::TestSuite() : FileList()
    {
        FileList.reserve(1024); // Preallocate memory to FileList to improve performance.
    }
    
    
    TestSuite::~TestSuite() 
    {
        // Do nothing
    }       
    

    size_t TestSuite::search(const std::string & folderName, const std::string & searchPatternString)
    {
        Tools::FileFinder finder;
        std::vector<std::string> fileExtensions;
        fileExtensions.push_back(".m");
        const size_t num = finder.search(folderName, searchPatternString, fileExtensions);
        FileList = finder.get(); // Make a copy of found tests.
        return num;
    }


    void TestSuite::print() const
    {
        std::vector< std::string >::const_iterator it;    
        std::vector< std::string >::const_iterator end = FileList.end();
        size_t numberOfFiles = 0;
        for (it = FileList.begin(); it != end; ++it)
        {
            if (isViewableFile(*it))
            {
                std::cout << *it << std::endl;
                numberOfFiles ++;
            }
        }
    
        std::cout << "Number of test files: " << numberOfFiles << std::endl;
    }


    void TestSuite::write(const std::string & fileName)
    {
        std::ofstream outputFile;
        outputFile.open(fileName.c_str());
        std::vector<std::string>::const_iterator it;
        std::vector<std::string>::const_iterator end = FileList.end();
        for (it = FileList.begin(); it != end; ++it)
        {
            outputFile << *it << std::endl;
        }    
        outputFile.close();
    }


    void TestSuite::read(const std::string & fileName)
    {
        std::ifstream inputFile(fileName.c_str());
        while ( ! (inputFile>>std::ws).eof() ) { // skip any trailing space
            std::string testname;
            inputFile >> testname; // this skips any leading space
            FileList.push_back( testname );
        }
    }


    bool TestSuite::unitTest()
    {
        const std::string listFile = "testsuites.txt";
        bool flag = true;

        {
            const std::string folderPath = "data";
            const size_t fileNum = search(folderPath);
            std::cout << "Number of test files: " << fileNum << std::endl;
            print();
            write(listFile);           
            flag &= FileList.size() == 21;
            FileList.clear();
        }

        {
            const std::string folderPath = "data";
            std::vector< std::string > v;
            std::cout << getRegexpString(v) << std::endl;
            v.push_back("codeInfo");
            std::cout << getRegexpString(v) << std::endl;
            v.push_back("CodeInfo");
            std::cout << getRegexpString(v) << std::endl;

            const size_t fileNum = search(folderPath, getRegexpString(v));
            print();
            flag &= FileList.size() == 2;
        }
    
        {
            read(listFile);
            flag &= FileList.size() == 23;
        }   
    
        return flag;
    }


    // TODO: Need to make this method work on Windows
    const std::string TestSuite::getRegexpString(const std::vector< std::string > & patternStrings)
    {
        std::string regexString;
        if (patternStrings.empty())
        {
            regexString = Tools::FileSeparator<std::string>::value + "t[^" + Tools::FileSeparator<std::string>::value + "]*.m$";
        }
        else
        {
            std::vector< std::string >::const_iterator it = patternStrings.begin();
            std::string buf = "(" + (*it);
            ++it;
            for ( ; it != patternStrings.end(); ++it)
                buf += "|" + (*it);        
            buf += ")";
            regexString = buf + ".*" + Tools::FileSeparator<std::string>::value + "t[^" + Tools::FileSeparator<std::string>::value + "]*.m$" + 
                "|" + Tools::FileSeparator<std::string>::value + "t[^"+ Tools::FileSeparator<std::string>::value + "]*" + buf + "[^" + 
                Tools::FileSeparator<std::string>::value + "]*.m$";
        }

        return regexString;
    }


    const bool TestSuite::isViewableFile(const std::string & fileName) const
    {
        const bool flag =
            (fileName.find(Resources<std::string>::CVSFolder) == std::string::npos) &&
            (fileName.find(Resources<std::string>::DerivedFolder) == std::string::npos);
        return flag;
    }


    void TestSuite::info() const
    {
        std::cout << "Number of test files: " << FileList.size() << std::endl;
    }
}
