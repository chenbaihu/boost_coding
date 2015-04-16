#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "t.h"

int main(int argc,char *argv[])
{
    Test<int> test(10);
    std::cout << test.get_t() << std::endl;
    test.set_t(15);
    std::cout << test.get_t() << std::endl;
    return 0;
}

