#include <map>
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>

class CTestBind
{
    public:
        CTestBind()
        {
            values_.clear();
        }
    public:
        void Init();
        typedef boost::function< void(size_t key, std::string value) > OnMatchKey;
        void Find(size_t key, const OnMatchKey& onmatch);
    private:   
        typedef std::map<size_t, std::string> ISMap;
        ISMap values_;
};

void CTestBind::Init()
{
    values_.insert(std::make_pair(1, "11111"));
    values_.insert(std::make_pair(2, "22222"));
    values_.insert(std::make_pair(3, "33333"));
}

void CTestBind::Find(size_t key,const OnMatchKey& onmatch)
{
    ISMap::const_iterator it = values_.find(key);
    if(it == values_.end()){
        return ;
    }  
    onmatch(key, it->second);
}

void test_fun(size_t key, std::string value, std::string& valueret){
    valueret = value;
}

int main(int argc,char* argv[])
{
    CTestBind t_bind;
    t_bind.Init();
    std::string value;
    t_bind.Find(1,boost::bind(test_fun, _1, _2, boost::ref(value)));
    std::cout << "value=" << value << std::endl;

    value.clear();
    t_bind.Find(2,boost::bind(test_fun, _1, _2, boost::ref(value)));
    std::cout << "value=" << value << std::endl;

    value.clear();
    t_bind.Find(4,boost::bind(test_fun, _1, _2, boost::ref(value)));
    std::cout << "value=" << value << std::endl;
    return 0;
}

