#include <boost/lexical_cast.hpp>

#include <iostream>  
#include <string>  

int main(int argc, char*argv[]) 
{
    int a    = boost::lexical_cast<int>("123");  
    double b = boost::lexical_cast<double>("123.12");
    const double d = 123.12;  
    std::string s = boost::lexical_cast<std::string>(d);   
    std::cout << a << std::endl; 
    std::cout << b << std::endl;  
    std::cout << s << std::endl;    

    try 
    {  
        int i = boost::lexical_cast<int>("xyz");  
    }  
    catch(boost::bad_lexical_cast& e)  
    {  
        std::cout << e.what() << std::endl;  
        return 1;  
    } 
    return 0; 
}  

