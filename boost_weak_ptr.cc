#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>

class parent;
class children;

typedef boost::shared_ptr<parent> parent_ptr;
typedef boost::shared_ptr<children> children_ptr;
typedef boost::weak_ptr<parent>   parent_weak_ptr;

class parent
{
    public:
        ~parent() { std::cout <<"destroying parent\n"; }

    public:
        children_ptr children;
};

class children
{
    public:
        ~children() { std::cout <<"destroying children\n"; }

    public:
        //parent_ptr parent;
        parent_weak_ptr parent;    //用户解决循环应用(circular reference), 如果这里使用parent_ptr，即shared_ptr 将出现循环引用
};


void test()
{
    parent_ptr father(new parent());
    children_ptr son(new children);

    father->children = son;
    son->parent = father;
}

int main(int argc, char* argv[])
{
    std::cout<<"begin test...\n";
    test();
    std::cout<<"end test.\n";
    return 0;
}


