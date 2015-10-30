// TODO: Improve this using the memory mapped file.
std::string readFile(const std::string &fileName) {
    std::ifstream in(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!in) {
        std::runtime_error("Cannot read the content of \"" + fileName + "\". Error code" +
                           std::to_string(errno));
    }
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return (contents);
}
