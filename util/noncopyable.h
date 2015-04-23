#ifndef __NONCOPYABLE_H
#define __NONCOPYABLE_H

namespace util {

class Noncopyable  
{  
protected:  
    Noncopyable() {}  
    ~Noncopyable() {}  

private:  // emphasize the following members are private  
    Noncopyable( const Noncopyable& );  
    const Noncopyable& operator=( const Noncopyable& );  
};  

} // end of namespace util

#endif

#ifdef _TEST_

class TestClass: public util::Noncopyable  
{
public:  
    TestClass(){};  
    TestClass(int i){};  
};  

int main()  
{  
    TestClass cl1();  
    TestClass cl2(1);  

    // TestClass cl3(cl1);    // error  
    // TestClass cl4(cl2);    // error  

    return 0;  
}  

#endif

