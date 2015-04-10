# boost comment
g++ -Wall -g noncopyable.cc -o noncopyable

g++ -Wall -g lexical_cast.cc -o lexical_cast

g++ -Wall -g boost_bind.cc -o boost_bind 

g++ -Wall -g boost_bind_ref.cc -o boost_bind_ref

# boost smart pointer

# boost container
g++ -Wall -g boost_intrusive_list.cc -o boost_intrusive_list

g++ -Wall -g boost_intrusive_list_typedef.cc -o boost_intrusive_list_typedef

g++ -Wall -g any_any_cast.cc -o any_any_cast 

# boost multi thread
g++ -Wall -g thread.cc -o  thread -lboost_thread-mt -lpthread

g++ -Wall -g thread_group.cc -o  thread_group -lboost_thread-mt -lpthread

g++ -Wall -g mutex.cc -o mutex -lboost_thread-mt -lpthread

g++ -Wall -g condition.cc -o condition -lboost_thread-mt -lpthread

g++ -Wall -g boost_thread_pool.cc -o boost_thread_pool -lboost_thread-mt -lpthread

# boost asio
g++ -Wall -g boost_asio_server_async.cc -o boost_asio_server_async -lboost_system-mt

g++ -Wall -g boost_asio_server_sync.cc -o boost_asio_server_sync -lboost_system-mt

g++ -Wall -g boost_asio_client_sync.cc -o boost_asio_client_sync -lboost_system-mt

g++ -Wall -g boost_asio_udp_server_sync.cc -o boost_asio_udp_server_sync -lboost_system-mt 

g++ -Wall -g boost_asio_udp_client_sync.cc -o boost_asio_udp_client_sync -lboost_system-mt

