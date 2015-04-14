#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <iostream>

struct Func {
    void operator()(int x) {
        std::cout << x << std::endl;
    }  
} f;

int main(int argc,char *argv[])
{
    typedef boost::function<void(int)> FUNCTION;
    FUNCTION obj = boost::bind<void>(f, _1);            //没有指定返回值类型时
    obj(4);
    return 0;
}
