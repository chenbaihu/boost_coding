# boost_coding

g++ -Wall -g thread.cc -o  thread -lboost_thread-mt -lpthread

g++ -Wall -g thread_group.cc -o  thread_group -lboost_thread-mt -lpthread

g++ -Wall -g mutex.cc -o mutex -lboost_thread-mt -lpthread

g++ -Wall -g condition.cc -o condition -lboost_thread-mt -lpthread

g++ -Wall -g noncopyable.cc -o noncopyable -lboost_thread-mt -lpthread

g++ -Wall -g any_any_cast.cc -o any_any_cast -lboost_thread-mt -lpthread

g++ -Wall -g lexical_cast.cc -o lexical_cast

g++ -Wall -g boost_bind.cc -o boost_bind 

g++ -Wall -g boost_bind_ref.cc -o boost_bind_ref

g++ -Wall -g boost_asio_server_async.cc -o boost_asio_server_async -lboost_system-mt

g++ -Wall -g boost_asio_server_sync.cc -o boost_asio_server_sync -lboost_system-mt

g++ -Wall -g boost_asio_client_sync.cc -o boost_asio_client_sync -lboost_system-mt
