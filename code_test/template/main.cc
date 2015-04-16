/*
 * c++中模板是的声明和实现是不能分开的。(如果要分开，需要编译器支持，目前都gcc可能支持，起塔编译器都不支持)
 * http://www.zhihu.com/question/20630104
 */
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

