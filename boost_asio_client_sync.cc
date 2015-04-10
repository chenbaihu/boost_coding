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

void print_rsp(std::vector<char> buf, size_t len)
{
    size_t i = 0;
    for (i=0; i<len; i++) {
        std::cout << buf[i];
    }
    std::cout << std::endl;
}

void test_asio_synclient()  
{  
    typedef boost::asio::io_service IoService;

    // 该命名空间下有几个常用类: accetpt, resolver, endpoint, socket  
    typedef boost::asio::ip::tcp TCP;  

    try  
    {  
        IoService ios;  
        boost::system::error_code error;  

        // 1. 用解释器的方法, 常用来解析域名, 如  
        // query("www.163.com", "80"), 也可以 query("www.163.com","telnet")  
        // echo          7/tcp  
        // ftp           21/tcp                 # File Transfer Protocol (Control)  
        // telnet        23/tcp                 # Virtual Terminal Protocol  
        // smtp          25/tcp                 # Simple Mail Transfer Protocol  
        // time          37/tcp  timeserver     # Time  

        //TCP::resolver resolver(ios);  
        //TCP::resolver::query query("127.0.0.1", "10000");  
        //TCP::resolver::iterator endpoint_iterator = resolver.resolve(query);  
        //TCP::resolver::iterator end;  
        //TCP::socket socket(ios);  
        //socket.connect(*endpoint_iterator, error);  

        // 2. 用简便的方法  
        TCP::socket socket(ios);  
        TCP::endpoint endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 10000);  
        socket.connect(endpoint, error);  

        // 这里要判断一下, 否则没有连上会通过.  
        if (error)  
            throw boost::system::system_error(error);  

        while(true)  
        {  
            // boost::array<char, 128> buf;  
            std::vector<char> buf(128);  
            size_t len = socket.read_some(boost::asio::buffer(buf), error);  

            // 这是也要判断一下, 否则服务端运行断开, 这里会出现死循环.  
            if (error == boost::asio::error::eof)  
                break; // Connection closed cleanly by peer.  
            else if (error)  
                throw boost::system::system_error(error); // Some other error.  

            print_rsp(buf, len);

            // buf要注意控制大小。  
            socket.write_some(boost::asio::buffer(buf, len), error);  
        }  
    }  
    catch (std::exception& e)  
    {  
        std::cout << e.what() << std::endl;  
    }  
}  

int main(int argc, char*argv[])
{
    test_asio_synclient();
    return 0;
}
