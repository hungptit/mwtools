#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include "sbtools/Utilities.hpp"
#include "utils/FileSystemUtilities.hpp"
#include "utils/Basic.hpp"
#include "gtest/gtest.h"

TEST(RemoveWhiteSpace, Positive) {
    std::string strBuf = "fdsfsd  fdsg dsgdsgd sgds";
    std::string strBuf_results = "fdsfsd..fdsg.dsgdsgd.sgds";
    std::cout << strBuf << std::endl;
    Tools::replaceSubstring(strBuf, " ", ".");
    std::cout << strBuf << std::endl;
    EXPECT_TRUE(strBuf == strBuf_results);
}
