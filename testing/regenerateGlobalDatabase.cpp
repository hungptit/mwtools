#include <tuple>
#include "utils/Process.hpp"

int main() {
    Tools::run("sbglobal", {"-gentags"});
    Tools::run("sblocate", {"-gendb"});
    Tools::run("sbid", {"-gendb"});
    Tools::run("sbgentbxcache", {});
    return EXIT_SUCCESS;
}
