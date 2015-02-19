#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>
#include <list>
#include "base/Basic.hpp"

const bool unitTest()
{
    {
        std::map<std::string, int> m;
        m["Test"] = 1;
        m["Tes1t"] = 2;
        Tools::disp_pair(m, "Map: ");
    }

    {
        std::vector<int> x;
        x.push_back(1);
        x.push_back(1);
        Tools::disp(x, "Vector: ");
    }

    {
        std::list<std::string> x;
        x.push_back("Hello");
        x.push_back("World!");
        Tools::disp(x, "List: ");
    }

    {
        int a[]= {100, 20, 320, 40, 50};
        std::set<int> x (a, a + 5);
        Tools::disp(x, "Set: ");
    }

    return true;
}


int main()
{
    return unitTest() ? EXIT_SUCCESS : EXIT_FAILURE;
}
