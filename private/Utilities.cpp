std::string getSbruntestsCommand(const std::string &farmOpts,
                                 const std::string &listFile,
                                 const std::string &runFolder) {
    using boost::filesystem::path;
    std::string command = "sbruntests " + farmOpts;
    std::string seed;

    if (!runFolder.empty()) {
        seed += "runtests";
        command += "-runallunder_mp " + runFolder;
    }

    else if (!listFile.empty()) {
        if (listFile.find("submit") == 0) {
            command += "-testsuites dacore -F " + listFile;
        } else {
            command += "-testsuites " + listFile;
            seed += "runtests";
        }
    }

    else {
        throw("Invalid options");
    }

    boost::filesystem::path parentFolder = path(std::getenv("DEFAULT_SANDBOX")) / path("logs") / path(seed);
    Tools::LogFolder logDir(parentFolder);
    command += " -testlogarea " + logDir.getPath().string();
    return command;
}

std::string readFile(const std::string &fileName) {
    std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!in)
        std::runtime_error("Cannot read the content of \"" + fileName +
                           "\". Error code" +
                           boost::lexical_cast<std::string>(errno));

    // Read file content into a string
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return (contents);
}

const std::string
generateGlobalDatabaseCommand(const std::string &sandboxPath) {
    std::string cmdStr;
    if (!sandboxPath.empty()) {
        cmdStr += "cd " + sandboxPath + ";";
    }

    cmdStr +=
        +"sbglobal -gentags; sblocate -gendb; sbid -gendb; sbgentbxcache;";
    return cmdStr;
}

// const std::string getDatabaseFileName(const std::string &sandboxPath) {
//     const std::string fileName =
//         sandboxPath + Tools::FileSeparator<std::string>::value +
//         SandboxResources<std::string>::SbtoolsFolder +
//         Tools::FileSeparator<std::string>::value +
//         SandboxResources<std::string>::DatabaseFileName;
//     return fileName;
// }
