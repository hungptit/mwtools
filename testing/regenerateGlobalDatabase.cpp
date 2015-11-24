#include <tuple>
#include "utils/Process.hpp"

int main() {
    Utils::run("sbglobal", {"-gentags"});
    Utils::run("sblocate", {"-gendb"});
    Utils::run("sbid", {"-gendb"});
    Utils::run("sbgentbxcache", {});
    return EXIT_SUCCESS;
}
