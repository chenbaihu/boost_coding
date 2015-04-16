#include <stdio.h>
#include <stdint.h>
#include <iostream>

#include "template.h"

template<typename T> 
void Test<T>::set_t(T t) {
    t_ = t;
}

template<typename T> 
T Test<T>::get_t() {
    return t_;
}

int main(int argc,char *argv[])
{
    Test<int> test(10);
    std::cout << test.get_t() << std::endl;
    test.set_t(15);
    std::cout << test.get_t() << std::endl;
    return 0;
}


