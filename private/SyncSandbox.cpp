const std::string SyncSandbox::BatTreeFileName = "battree";
const std::string SyncSandbox::SyncOptFileName = "sbsync.cfg";


SyncSandbox::SyncSandbox(const std::string & sandboxName, const std::vector< std::string > & platforms, const std::string & perfectArea) :
    SandboxName(sandboxName), Platforms(platforms), PerfectArea(perfectArea)
{
    // Do nothing
}


SyncSandbox::~SyncSandbox()
{
    // Do nothing
}


void SyncSandbox::createBattreeFile(const std::string & sandboxPath)
{
    std::ofstream outputFile;
    const std::string fileName = sandboxPath + Tools::FileSeparator<std::string>::value + BatTreeFileName;
    outputFile.open(fileName.c_str());
    outputFile << "ENV " << PerfectArea << std::endl;
    outputFile.close();
}


/** 
 * @todo Update this function
 * 
 * @param sandboxPath 
 */
void SyncSandbox::createConfigFile(const std::string & sandboxPath)
{
    std::ofstream outputFile;
    const std::string fileName = sandboxPath + Tools::FileSeparator<std::string>::value + SyncOptFileName;
    
    outputFile.open(fileName.c_str());

    // Sync with Aslrtw by default
    outputFile << "-cfg simulink" << std::endl;

    // Setup platform
    std::vector<std::string>::const_iterator it = Platforms.begin();
    for (; it != Platforms.end(); ++it)
        outputFile << "-P " << *it << std::endl;

    // Generate search database
    outputFile << "-gentags" << std::endl;
    outputFile << "-gensbid" << std::endl;

    outputFile.close();
}


/** 
 * @todo This function is designed to work with Aslrtw cluster. Need to make it work with any cluster.
 * 
 */
void SyncSandbox::sync()
{
    const std::string currentFolder = Tools::getCurrentFolder();
    const std::string sandboxPath = currentFolder + Tools::FileSeparator<std::string>::value + SandboxName;
    const bool isOk = Tools::createDirectory(sandboxPath);
    
    if (!isOk) throw std::runtime_error("Cannot create folder: " + sandboxPath);
    createBattreeFile(sandboxPath);

    // Todo: Need to create sbsync.cfg file
    createConfigFile(sandboxPath);

    // Sync the sandbox
    const std::string runCommand = "cd " + sandboxPath + "; sbsync -src-root " + PerfectArea;
    Tools::run(runCommand);
    
    // Generate the editable file database
    Sbtools::FindEditedFiles finder;
    finder.searchWritableFiles(sandboxPath + Tools::FileSeparator<std::string>::value + "matlab");
    finder.createWritableFileDatabase(getDatabaseFileName(sandboxPath));
}


void SyncSandbox::info()
{
    std::cout << "Sandbox name" << SandboxName << std::endl;
    std::cout << "Perfect area" << PerfectArea << std::endl;
    Tools::disp(Platforms, "Platforms: ");
}
