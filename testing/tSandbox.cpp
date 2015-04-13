#include "sbtools/Sandbox.hpp"
#include "gtest/gtest.h"

TEST(RemoveWhiteSpace, Positive) {
    Tools::Sandbox sandbox("/local-ssd/sandbox/advisor");
    sandbox.info();
}
