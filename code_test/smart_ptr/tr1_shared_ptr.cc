/*
 * shared_ptr常用方法：
 *     1、reset(新指针);       // 管理新的指针成员；
 *     2、get();               // 获取管理的裸指针；
 *     3、use_count();         // 获取有多少个智能指针管理着当前地址；
 *     4、unique();            // 判断当前指针是否只有当前对象管理；
 *     5、swap();              // 与另外一个智能指针对象交换内容；
 *
 * shared_ptr技术与陷阱：
 * 1、意外延长生命周期：
 *     boost::bind，因为boost::bind会把实参拷贝一份，如果参数是一个shared_ptr，那么对象的生命期就不会短语boost::function对象。
 *     class Foo { void doit(); };
 *     shard_ptr<Foo> foo_ptr(new Foo());
 *     boost::function<void()> func = boost::bind(&Foo::doit, foo_ptr);
 *     这样在func销毁前，foo_ptr就不会销毁。
 *
 * 2、shared_ptr作为函数参数的问题，因为要修改引用技术(而且拷贝的时候通常需要加锁)，shared_ptr的拷贝开销要比拷贝原始指针高很多，
 *     但是，实际上需要拷贝的时候并不多。多数情况下可以通过const reference来使用这个shared_ptr。
 * 例如：
 *     void save(const shared_ptr<Foo>& fFoo);
 *     如下：
 *     void onMessage(const std::string& msg) 
 *     {
 *         shared_ptr<Foo> pFoo(new Foo(msg));
 *         if (save(pFoo)) {    // 此时pFoo没有进行拷贝构造，那么引用技术也不会加1。备注：(如果save是一个异步调用，这里就会出现pFoo释放的问题)
 *            // TODO
 *         }
 *      }
 *
 * 3、给shared_ptr对象指定释放资源的函数：
 *     对普通数据成员或者类，shared_ptr通过默认的delete和析构函数将资源释放，但是如果我们要使用shared_ptr管理一个文件句柄fd或fp，或者sockfd，
 *     那么需要通过特殊的方式释放资源。
 *     #include <stdio.h>
 *     #include <tr1/memory>
 *
 *     typedef std::tr1::shared_ptr<FILE> FILEPtr;
 *      struct FreeFILE {
 *          void operator()(FILE* fp) {
 *              if (fp!=NULL) {
 *                  fclose(fp);
 *              }  
 *          }  
 *      };
 *      
 *      int main(int argc, char *argv[])
 *      {
 *          FILEPtr fp_ptr;
 *          fp_ptr.reset(fopen("./test.txt", "w"), FreeFILE());
 *          fprintf(fp_ptr.get(), "%s\n", "hello, this is a test");
 *          fp_ptr.reset();
 *      
 *          return 0;
 *      }
 */




#include <tr1/memory>
#include <iostream>

typedef std::tr1::shared_ptr<int> IntPtr;

int main(int argc,char *argv[])
{
    IntPtr obj(new int(10));                 //创建方式1
    if (obj!=NULL) {
        std::cout << *obj << std::endl;
    }  

    IntPtr obj2;
    obj2.reset(new int(11));                 //创建方式2 reset
    if (obj2!=NULL) {
        std::cout << *obj2 << std::endl;
    }  
    obj2.reset();                           // 释放
    if (obj2==NULL) {
        std::cout << "obj2 is NULL" << std::endl;
    }  

    IntPtr obj3;
    obj3 = IntPtr(new int(12));             //创建方式3 
    if (obj3!=NULL) {
        std::cout << *obj3 << std::endl;
    }  
    IntPtr obj4 = obj3;                     //拷贝
    std::cout << *obj4 << std::endl;

    //IntPtr obj5 = std::make_shared<int>(new int(12));               //创建方式4，推荐使用，但是我自己没有测试通过（我使用的是gcc/g++为4.4.6，好像4.7对C++支持比较好，可以使用）

    return 0;
}   





