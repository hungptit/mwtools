#include <vector>
#include <map>
#include "utils/FileFinder.hpp"
#include "base/Basic.hpp"
#include "base/FileSystemUtilities.hpp"
#include "gtest/gtest.h"


size_t find(const std::string & folderPath, bool verbose = false)
{
    std::vector<std::string> ext;
    ext.push_back(".m");
    ext.push_back(".cpp");
    ext.push_back(".hpp");
    ext.push_back(".mat");
    ext.push_back(".mdl");
    ext.push_back(".slx");
    ext.push_back(".sldd");
    ext.push_back(".xml");
    ext.push_back(".mtf");

    Tools::MWFileExtensionSearchStrategy extObj(ext);
    Tools::DefaultFileNameSearchStrategy nameObj; 

    Tools::FileFinder<Tools::EditedFileSearchStrategy, 
                      Tools::MWFileExtensionSearchStrategy, 
                      Tools::DefaultFileNameSearchStrategy> finder(extObj, nameObj);
    size_t fileNum = finder.search(folderPath);
    std::cout << "Number of files: " << fileNum << std::endl;
    if (verbose) finder.print();
    return fileNum;
}


TEST(DefaultSearchStrategy, Positive)
{
    EXPECT_TRUE(find("data/matlab") == 282);
    EXPECT_TRUE(find("data/matlab/test/CodeInfo/") == 15);
    EXPECT_TRUE(find("data/matlab/test/ss2mdlref/") == 267);
}


TEST(DefaultSearchStrategy, Negative)
{
    ASSERT_ANY_THROW(find("data1/matlab"));
}


TEST(DefaultSearchEditedFileStrategy, Positive)
{
    EXPECT_TRUE(find("data/matlab", false) == 282);
    EXPECT_TRUE(find("data/matlab/test/CodeInfo/", false) == 15);
    EXPECT_TRUE(find("data/matlab/test/ss2mdlref/") == 267);
}
