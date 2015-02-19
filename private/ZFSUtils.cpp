ZFSUtils::ZFSUtils(const std::string & masterSandbox, const std::vector<std::string> & sandboxes, const std::string & perfectArea) :
    SandboxPath(Tools::getCurrentFolder()),
    SandboxZFSPath(SandboxPath.substr(1)),
    PerfectArea(perfectArea),
    MasterSandbox(masterSandbox),
    Sandboxes(sandboxes),
    BackupFolders()
{
    // Do nothing
}


ZFSUtils::~ZFSUtils()
{
    // Do nothing
}


/** 
 * @note Users have to backup and delete all clones before running this method. 
 * 
 */
void ZFSUtils::sync()
{
    info();

    std::string runCommand;

    // Destroy the latest snapshot
    runCommand += "sudo zfs destroy " +
        SandboxZFSPath + Tools::FileSeparator<std::string>::value + MasterSandbox + Resources<std::string>::LatestSnapshotName + ";";

    // Sync the master sandbox
    runCommand += syncCommand(SandboxPath + Tools::FileSeparator<std::string>::value + MasterSandbox);
    
    // Create snapshot of the master sandbox
    runCommand += "sudo zfs snap " +
        SandboxZFSPath + Tools::FileSeparator<std::string>::value + MasterSandbox + Resources<std::string>::LatestSnapshotName + ";";

    // Build the master sandbox
    runCommand += buildMasterSandboxCommand();

    // Create all clones of the master sandbox
    runCommand += createClonesCommand(Sandboxes);

    // Regenerate all databases
    runCommand += regenerateDatabase(Sandboxes);

    // For debugging purpose only
    std::cout << std::endl << "Shell commands: \"" << runCommand << "\"" << std::endl << std::endl;
    
    Tools::run(runCommand);
    generateFindEditedFileDatabase(Sandboxes);
}


void ZFSUtils::info()
{
    std::cout << "Sandbox path: " << SandboxPath << std::endl;
    std::cout << "ZFS path: " << SandboxZFSPath << std::endl;
    std::cout << "Master sandbox: " << MasterSandbox << std::endl;
    std::cout << "Perfect area: " << PerfectArea << std::endl;
    Tools::disp(Sandboxes, "Clones: ");
}


const std::string ZFSUtils::syncCommand(const std::string & sandbox)
{    
    // Do not backup the master sandbox
    const std::string runCommand = "cd " + SandboxPath + Tools::FileSeparator<std::string>::value + MasterSandbox + ";" 
        + "sbsync -nobackup;"; 
    return runCommand;
}


const std::string ZFSUtils::createClonesCommand(const std::vector<std::string> & sandboxes)
{
    std::string runCommand;
    std::vector<std::string>::const_iterator it = sandboxes.begin();
    for (; it != sandboxes.end(); ++it)
    {
        runCommand += "sudo zfs clone "
            + SandboxZFSPath + Tools::FileSeparator<std::string>::value + MasterSandbox + Resources<std::string>::LatestSnapshotName + " "
            + SandboxZFSPath + Tools::FileSeparator<std::string>::value + *it + ";";
    }
    return runCommand;
}


const std::string ZFSUtils::destroyClonesCommand(const std::vector<std::string> & sandboxes)
{
    std::string runCommand;
    std::vector<std::string>::const_iterator it = sandboxes.begin();
    for (; it != sandboxes.end(); ++it)
    {
        runCommand += "sudo zfs destroy "
            + SandboxZFSPath + Tools::FileSeparator<std::string>::value + *it + ";";
    }
    return runCommand;
}


void ZFSUtils::generateFindEditedFileDatabase(const std::vector<std::string> & sandboxes)
{
    std::vector<std::string>::const_iterator it = sandboxes.begin();
    for (; it != sandboxes.end(); ++it)
    {
        const std::string sandboxPath = SandboxPath + Tools::FileSeparator<std::string>::value + *it;
        Sbtools::FindEditedFiles finder;
        finder.searchWritableFiles(sandboxPath + Tools::FileSeparator<std::string>::value + "matlab");
        finder.createWritableFileDatabase(getDatabaseFileName(sandboxPath));
    }
}


void ZFSUtils::createClones(const std::vector<std::string> & sandboxes)
{
    const std::string runCommand = createClonesCommand(sandboxes);
    std::cout << "Shell command: " << runCommand << std::endl;
    Tools::run(runCommand);
}


void ZFSUtils::destroyClones(const std::vector<std::string> & sandboxes)
{
    const std::string runCommand = destroyClonesCommand(sandboxes);
    std::cout << "Shell command: " << runCommand << std::endl;
    Tools::run(runCommand);
}


const std::string ZFSUtils::regenerateDatabase(const std::vector<std::string> & sandboxes)
{
    std::string runCommand;
    std::vector<std::string>::const_iterator it = sandboxes.begin();
    for (; it != sandboxes.end(); ++it)
    {
        const std::string buf = Sbtools::generateGlobalDatabaseCommand(SandboxPath + Tools::FileSeparator<std::string>::value + *it);
        runCommand += buf;
    }
    return runCommand;
}


/** 
 * Build the master sandbox before creating clones. 
 * 
 * @note This function is more specific for Simulink team.
 * @return 
 */
const std::string ZFSUtils::buildMasterSandboxCommand()
{
    std::string runCommand;
    const std::string masterSandboxPath = SandboxPath + Tools::FileSeparator<std::string>::value + MasterSandbox;
    runCommand += "cd " + masterSandboxPath + "/matlab/src/simulink/;" + "sbmake -distcc;" + "cd " + SandboxPath + ";";
    return runCommand;
}
