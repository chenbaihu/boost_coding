//boost  
#include <boost/thread.hpp> 
#include <boost/asio.hpp> 

//stl  
#include <string>  
#include <iostream>  

using namespace std;  

int main()  
{  
    boost::asio::io_service io_service;  
    boost::asio::ip::udp::socket udp_socket(io_service);  
    boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("127.0.0.1"), 7474);  

    udp_socket.open(local_add.protocol());  
    udp_socket.bind(local_add);  

    char receive_buffer[1024] = {0};  
    while (true)  
    {  
        boost::asio::ip::udp::endpoint send_point;  
        udp_socket.receive_from(boost::asio::buffer(receive_buffer, 1024), send_point);  
        cout << "recv:" << receive_buffer << endl;  
        udp_socket.send_to(boost::asio::buffer(receive_buffer), send_point);  

        memset(receive_buffer, 0, 1024);  
    }  

    return 1;  
}


