#include "sbtools/Sandbox.hpp"
#include "gtest/gtest.h"

TEST(Sandbox, Positive) {
    Tools::Sandbox sandbox(boost::filesystem::path("/local-ssd/sandbox/advisor"));
    sandbox.info();
}


TEST(Sandbox, Negative) {
    ASSERT_ANY_THROW(Tools::Sandbox sandbox(boost::filesystem::path("/local/projects/")));
}
