#include <tr1/functional>
#include <tr1/memory>
#include <stdio.h>

using std::tr1::bind;

typedef std::tr1::function<void (int& request)> ProcessFunction;

void product(int& x, int& y)
{
    x *= 10;
    y *= 20;
}

int main()
{
    int x = 10;
    int y = 20;

    ProcessFunction t1 = std::tr1::bind(&product, std::tr1::ref(x), std::tr1::placeholders::_1);
    t1(y);
    printf("%d\t%d\n", x, y);

    //  std::tr1::ref(value_name)      
    //  std::tr1::cref(value_name)

    ProcessFunction t2 = std::tr1::bind(&product, std::tr1::placeholders::_1, std::tr1::ref(y));
    t2(x);
    printf("%d\t%d\n", x, y);
    return 0;
}

