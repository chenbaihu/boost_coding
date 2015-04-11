//boost  
#include <boost/asio.hpp> 

//stl  
#include <iostream>  

using namespace std;  

int main()  
{  
    boost::asio::io_service io_service;  
    boost::asio::ip::udp::socket socket(io_service);  
    boost::asio::ip::udp::endpoint end_point(boost::asio::ip::address::from_string("127.0.0.1"), 7474);  
    socket.open(end_point.protocol());  
    char receive_buffer[1024] = {0};  
    while (true)  
    {  
        cout << "input:";  
        string input_data;  
        cin >> input_data;  
        cout << endl;  

        try  
        {  
            socket.send_to(boost::asio::buffer(input_data.c_str(), input_data.size()), end_point);  

            socket.receive_from(boost::asio::buffer(receive_buffer, 1024), end_point);  

            cout << "recv:" << receive_buffer << endl;  
        }  
        catch (boost::system::system_error &e)  
        {  
            cout << "process failed:" << e.what() << endl;  
        }  
    }  
}

