/*
 * http://blog.csdn.net/huang_xw/article/details/8518740
 * http://www.cppblog.com/merlinfang/archive/2014/08/11/207969.aspx
 * Boost.Asio成功的抽象了包括网络，串口、文件在内的同步和异步的I/O操作：
 * read(stream, buffer [, extra options]) 
 * async_read(stream, buffer [, extra options], handler) 
 * write(stream, buffer [, extra options]) 
 * async_write(stream, buffer [, extra options], handler)
 * Boost.Asio支持大多数操作系统，轻量可扩展的，目前网络部分支持TCP, UDP, IMCP，可以很方便的扩展自定义协议。
 * */

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/asio/impl/src.hpp>    // if use ssl

#include <stdio.h>
  
typedef boost::asio::io_service IoService;  
typedef boost::asio::ip::tcp TCP;  

std::string make_daytime_string()  
{  
    using namespace std;  
    time_t now = std::time(NULL);  
    return ctime(&now);  
}  

class TcpConnection  
    : public boost::enable_shared_from_this<TcpConnection>  {
public:  
    typedef boost::shared_ptr<TcpConnection> pointer;  

    static pointer create(IoService& io_service)  
    {  
        return pointer(new TcpConnection(io_service));  
    }  

    TCP::socket& socket()  
    {  
        return socket_;  
    }  

    void start()  
    {  
        message_ = make_daytime_string();  

        boost::asio::async_write(  
                socket_,   
                boost::asio::buffer(message_),   
                boost::bind(&TcpConnection::handle_write,   
                    shared_from_this(),   
                    boost::asio::placeholders::error,   
                    boost::asio::placeholders::bytes_transferred));  
    }  
private:  
    TcpConnection(IoService& io_service)  
        : socket_(io_service)  
    {  
    }  

    void handle_write(const boost::system::error_code& error,  
            size_t bytes_transferred)  
    {  
        printf("write data, bytes_transferred=%ld\n", bytes_transferred);  
    }  

    TCP::socket socket_;  
    std::string message_;  
};  

class TcpServer  
{  
public:  
    TcpServer(IoService& io_service)  
        : acceptor_(io_service, TCP::endpoint(TCP::v4(), 10000))  
    {  
        start_accept();  
    }  
private:  
    void start_accept()  
    {  
        TcpConnection::pointer new_connection =  
            TcpConnection::create(acceptor_.get_io_service());  

        acceptor_.async_accept(  
                new_connection->socket(),   
                boost::bind(&TcpServer::handle_accept,   
                    this,   
                    new_connection,   
                    boost::asio::placeholders::error));  
    }  

    void handle_accept(TcpConnection::pointer new_connection,  
            const boost::system::error_code& error)  
    {  
        if (!error)  
        {  
            new_connection->start();  
            start_accept();  
        }  
    }  

    TCP::acceptor acceptor_;  
};  

// TcpConnection与TcpServer封装后  
void test_asio_asynserver()  
{  
    try  
    {  
        IoService io_service;  
        TcpServer server(io_service);  

        // 只有io_service类的run()方法运行之后回调对象才会被调用  
        io_service.run();  
    }  
    catch (std::exception& e)  
    {  
        std::cerr << e.what() << std::endl;  
    }  
}


int main(int argc, char*argv[])
{
    test_asio_asynserver();
    return 0;
}


