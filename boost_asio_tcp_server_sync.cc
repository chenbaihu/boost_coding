/*
 *Boost.Asio成功的抽象了包括网络，串口、文件在内的同步和异步的I/O操作：
 * read(stream, buffer [, extra options]) 
 * async_read(stream, buffer [, extra options], handler) 
 * write(stream, buffer [, extra options]) 
 * async_write(stream, buffer [, extra options], handler)
 *Boost.Asio支持大多数操作系统，轻量可扩展的，目前网络部分支持TCP, UDP, IMCP，可以很方便的扩展自定义协议。
 * */

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/asio/impl/src.hpp>    // if use ssl

#include <iostream>
#include <stdio.h>

namespace  
{  
    std::string make_daytime_string()  
    {  
        using namespace std; // For time_t, time and ctime;  
        time_t now = time(0);  
        return ctime(&now);  
    }  
}  
   
void test_asio_synserver()  
{  
    typedef boost::asio::io_service IoService;  
    typedef boost::asio::ip::tcp TCP;  
  
    try  
    {  
        IoService ios;  
        /* 
            *以下构造函数相当于以下步骤 
            * basic_socket_acceptor<tcp> acceptor(io_service); 
            * acceptor.open(tcp::v4()); 
            * acceptor.bind(tcp::endpoint(8808)); 
            * acceptor.listen(0);//default  
        */ 

        /* 
            tcp::endpoint(端点)由以下三个部分组成 
            1. IP地址(address, address_v4, address_v6) 
            2. 端口号 
            3. 协议版本 
        */  
        TCP::acceptor acceptor(ios, TCP::endpoint(TCP::v4(), 8808));  
        for (;;)  
        {  
            TCP::socket socket(ios);  
            acceptor.accept(socket);  
            std::string message = make_daytime_string();  
  
            boost::system::error_code ignored_error;  
            boost::asio::write(socket,  
                               boost::asio::buffer(message),   
                               boost::asio::transfer_all(),   
                               ignored_error);  
            //PRINT_DEBUG(message);  
            printf("send message:%s\n", message.c_str());
        }  
    }  
    catch (std::exception& e)  
    {  
        std::cout << e.what() << std::endl;  
    }  
}  

int main(int argc, char*argv[])
{
    test_asio_synserver();
    return 0;
}

