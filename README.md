# libraries document 
    http://en.cppreference.com/w/
    http://zh.cppreference.com/w/cpp

    http://www.boost.org/doc/libs/1_53_0/
    latest version 1.57

# src download
    http://jaist.dl.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.gz
    http://jaist.dl.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.gz

# comment
    g++ -Wall -g noncopyable.cc -o noncopyable
    
    g++ -Wall -g lexical_cast.cc -o lexical_cast

# bind function 
    g++ -Wall -g boost_bind.cc -o boost_bind 
    
    g++ -Wall -g boost_bind_ref.cc -o boost_bind_ref

# smart_pointer  
    Smart Ptr Document: http://www.boost.org/doc/libs/1_53_0/libs/smart_ptr/smart_ptr.htm
    
    g++ -Wall -g tr1_shared_ptr.cc  -o tr1_shared_ptr
    g++ -Wall -g tr1_shared_ptr_FILE.cc -o tr1_shared_ptr_FILE

    g++ -Wall -g tr1_weak_ptr.cc -o tr1_weak_ptr
    g++ -Wall -g boost_weak_ptr.cc -o boost_weak_ptr 
    
    g++ -Wall -g boost_scoped_ptr.cc -o boost_scoped_ptr

# container
    g++ -Wall -g boost_intrusive_list.cc -o boost_intrusive_list
    
    g++ -Wall -g boost_intrusive_list_typedef.cc -o boost_intrusive_list_typedef
    
    g++ -Wall -g any_any_cast.cc -o any_any_cast 
    
# multi_thread
    g++ -Wall -g thread.cc -o  thread -lboost_thread-mt -lpthread
    
    g++ -Wall -g thread_group.cc -o  thread_group -lboost_thread-mt -lpthread
    
    g++ -Wall -g mutex.cc -o mutex -lboost_thread-mt -lpthread
    
    g++ -Wall -g condition.cc -o condition -lboost_thread-mt -lpthread
    
    g++ -Wall -g boost_thread_pool.cc -o boost_thread_pool -lboost_thread-mt -lpthread

# multi_process

# net
    g++ -Wall -g boost_asio_server_async.cc -o boost_asio_server_async -lboost_system-mt
    
    g++ -Wall -g boost_asio_server_sync.cc -o boost_asio_server_sync -lboost_system-mt
    
    g++ -Wall -g boost_asio_client_sync.cc -o boost_asio_client_sync -lboost_system-mt
    
    g++ -Wall -g boost_asio_udp_server_sync.cc -o boost_asio_udp_server_sync -lboost_system-mt 
    
    g++ -Wall -g boost_asio_udp_client_sync.cc -o boost_asio_udp_client_sync -lboost_system-mt

# file
    g++ -Wall -g -D_TEST_ file_util.cc -o file_util 


