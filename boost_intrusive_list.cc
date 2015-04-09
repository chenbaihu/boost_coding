#include <boost/intrusive/list.hpp>
#include <iostream>

using namespace boost::intrusive;

typedef list_base_hook<link_mode<auto_unlink> > auto_unlink_hook;
class MyData: public auto_unlink_hook
{
    private:
        int data;
    public:
        MyData( int i ):data(i){}
        void show( ) const
        {
            std::cout<<data<<std::endl;
        }
};

int main()
{
    typedef list<MyData, constant_time_size<false> > DataList;
    DataList dl;

    MyData d1(1);   
    MyData d2(2);   
    MyData d3(3);
    MyData *d4 = new MyData(4);

    dl.push_front(d1);  //侵入式链表中存放的是对象实体，不能存放boost::shared_ptr等引用对象。
    dl.push_front(d2);  
    dl.push_front(d3);
    dl.push_front(*d4);
    
    d2.unlink();        //d2调用unlink以后它自己就从dl中删除了引用

    delete d4;          //delete d4后从dl中也删除了引用
    
    DataList::const_iterator it = dl.begin();
    for( ; it != dl.end(); it ++ ) {
        it->show();
    }
    return 0;
}

