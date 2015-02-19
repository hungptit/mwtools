const bool TestDatabase::hasRunTimeInfo(const std::string & fileName)
{
    std::map< std::string, std::vector<double> >::const_iterator currentElement = Database.find(fileName);
    return (currentElement != Database.end());
}


const double TestDatabase::getRunTime(const std::string & fileName)
{
    std::map< std::string, std::vector<double> >::const_iterator currentElement = Database.find(fileName);   
    if (currentElement != Database.end())
    {
        const std::vector<double> & val = currentElement->second;
        const double sum = std::accumulate(val.begin(), val.end(), 0.0);
        return sum / val.size();
    }
    else
    {
        return -1;;
    }
}


void TestDatabase::setRunTime(const std::string & fileName, const double t)
{
    std::map< std::string, std::vector<double> >::iterator currentElement = Database.find(fileName);
    std::map< std::string, std::vector<double> >::iterator end = Database.end();
    if (currentElement != end)
    {
        std::vector<double> & val = currentElement->second;
        val.push_back(t);
    }
    else
    {
        std::vector<double> val;
        val.push_back(t);
        Database[fileName] = val;
    }        
}

const bool TestDatabase::read(const std::string & fileName)
{   
    const boost::filesystem::path file(fileName);
    bool flag = false;
    if (boost::filesystem::is_regular_file(file))
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
                    const size_t pos = lineBuf.rfind(';');
                    const std::string key = lineBuf.substr(0, pos);
                    const std::string timeString = lineBuf.substr(pos + 1);
                    std::vector<double> runTimes;
                    getRunTimeFromString(timeString, runTimes);
                    Database[key] = runTimes;
                }
            }
        }
        inputFile.close();
        flag = true;
    }
    else
    {
        std::cerr << "Cannot open file \"" << fileName << "\" to read!" << std::endl;
    }
    return flag;
}


void TestDatabase::write(const std::string & fileName)
{
    std::ofstream outputFile;
    outputFile.open(fileName.c_str());
    std::map< std::string, std::vector<double> >::const_iterator it;
    for (it = Database.begin(); it != Database.end(); ++it)
    {
        outputFile << it->first << ";";
        std::vector<double>::const_iterator currentElement;
        const std::vector<double> runTimes = it->second;
        for (currentElement = runTimes.begin(); currentElement != runTimes.end(); ++currentElement)
            outputFile << *currentElement << ",";
        outputFile << std::endl;
    }
    outputFile.close();
}


const size_t TestDatabase::getRunTimeFromString(const std::string & strBuf, std::vector<double> & runTimes)
{
    size_t start = 0;
    size_t stop = strBuf.find(',');
    const size_t N = strBuf.size();
    std::string subString;
    while (stop != std::string::npos)
    {
        subString = strBuf.substr(start, stop - start);
        if (!subString.empty()) runTimes.push_back(boost::lexical_cast<double>(subString));
        start = stop + 1;
        stop = strBuf.find(',', stop + 1);
    }
    return runTimes.size();
}


void TestDatabase::getRunTimeString(const std::vector<double> & runTimes, std::string & strBuf)
{
    strBuf.clear();
    std::vector<double>::const_iterator currentElement;
    for (currentElement = runTimes.begin(); currentElement != runTimes.end(); ++currentElement)
        strBuf += boost::lexical_cast<std::string>(*currentElement) + ",";
}


void TestDatabase::print()
{
    std::string strBuf;
    std::map< std::string, std::vector<double> >::const_iterator it;
    for (it = Database.begin(); it != Database.end(); ++it)
    {
        getRunTimeString(it->second, strBuf);
        std::cout << it->first << ":" << strBuf << std::endl; 
    }
    std::cout << "Size of database: " << Database.size() << std::endl;
}


const bool TestDatabase::unitTest()
{
    bool flag = true;
    const std::string filesep = Tools::FileSeparator<std::string>::value;
    {
        std::vector<double> val;
        val.push_back(1);
        val.push_back(1.1345413);
        std::string strBuf;
        getRunTimeString(val, strBuf);
        std::cout << strBuf << std::endl;
        flag &= strBuf == "1,1.1345413,";        
    }

    {
        std::vector<double> val;
        getRunTimeFromString("1,2,", val);
        flag &= (val[0] == 1) && (val[1] == 2);
    }


    {
        flag &= !read("fsfds");
        const std::string fileName = "data" + filesep + "run_time_database.txt";
        flag &= read(fileName);
        print();
        flag &= Database.size() == 4;
        write("database.txt");
    }

    {
        const std::string key = "matlab"+ filesep + "test" + filesep + "ss2mdlref" + filesep + "tSubsystemConversion.m";
        std::cout << "run time = " << getRunTime(key) << std::endl;
        flag &= getRunTime(key) == 2.5;
    }

    {
        const std::string key = "matlab"+ filesep + "test" + filesep + "ss2mdlref" + filesep + "tSubsystemConversion.m";
        setRunTime(key, 0);
        setRunTime(key, 2);
        std::cout << "run time = " << getRunTime(key) << std::endl;
        flag &= getRunTime(key) == 2;
        print();
    }

    {
        const std::string key = "matlab"+ filesep + "test" + filesep + "ss2mdlref" + filesep + "tSubsystemConversion.m";
        std::cout << hasRunTimeInfo(key) << std::endl;
        std::cout << hasRunTimeInfo(key) << std::endl;
        flag &= hasRunTimeInfo(key);
        
        const std::string failedKey = "matlab"+ filesep + "test" + filesep + "ss2mdlref" + filesep + "tSubsystemConversion1.m";
        flag &= !hasRunTimeInfo(failedKey);
    }
    
    return flag;
}
