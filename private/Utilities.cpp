// TODO: Improve this using the memory mapped file.
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


// TODO: Rewite this function to take advantage of new run command.
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
