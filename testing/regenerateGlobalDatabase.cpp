#include <tuple>
#include "utils/Process.hpp"

int main() {
    utils::run("sbglobal", {"-gentags"});
    utils::run("sblocate", {"-gendb"});
    utils::run("sbid", {"-gendb"});
    utils::run("sbgentbxcache", {});
    return EXIT_SUCCESS;
}
