#include <tr1/memory>
#include <iostream>

typedef std::tr1::shared_ptr<int> IntPtr;
typedef std::tr1::weak_ptr<int> WeakIntPtr;

int main(int argc,char *argv[])
{
    IntPtr obj(new int(10));
    if (obj!=NULL)  {
        std::cout << *obj << std::endl;
    }  

    //使用weak_ptr指向shared_ptr对象
    WeakIntPtr obj2 = obj;   

    //判断obj2指向的对象是否有效的方法
    if (obj2.use_count()==0)  {   // 判断对象是否有效，可以通过use_count和expired方法
        std::cout << "obj2 point obj free: use_count=" << obj2.use_count() << std::endl;
    } else  {
        std::cout << "obj2 point obj valid: use_count=" << obj2.use_count() << std::endl;
    }

    if (obj2.expired())  {
        std::cout << "obj2 point obj expired" << std::endl;
    } else  {
        std::cout << "obj2 point obj valid" << std::endl;
    }  

    //使用weak_ptr对象访问管理的对象
    IntPtr obj3 = obj2.lock();    // 要通过weak_ptr访问指向的对象，需要通过lock提升为shared_ptr；
    if (obj3==NULL)  {
        std::cout << "obj has free" << std::endl;
    } else  {
        std::cout << "obj2 point object value is:" << *obj3 << std::endl;
    }  

    obj.reset();                  // shared_ptr与weak_ptr指针不再管理对象，可以使用reset操作；
    std::cout << "obj.reset() but obj3 not reset, obj2 use_count=" << obj2.use_count() << std::endl;

    obj3.reset();
    std::cout << "obj.reset() and obj3.reset(), obj2 use_count=" << obj2.use_count() << std::endl;

    // weak_ptr不再管理对象
    if (obj2.expired())  {
        std::cout << "obj2 point obj expired" << std::endl;
    } else  {
        std::cout << "obj2 point obj valid" << std::endl;
    }
    return 0;
} 

