#ifdef DAsyncMapCli
#include "AsyncMapServiceHandler.h"
#else
#include "MapServiceHandler.h"
#endif

#include "thrift/protocol/TBinaryProtocol.h"

#include "thrift/server/TSimpleServer.h"
#include "thrift/server/TNonblockingServer.h"
#include "thrift/server/TThreadPoolServer.h"

#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/THttpServer.h"

#include "thrift/async/TAsyncProtocolProcessor.h"
#include "thrift/async/TEvhttpServer.h"

#include <stdio.h>
#include <signal.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;
using namespace ::apache::thrift::async;

using boost::shared_ptr;

#define THREAD_STACK_SIZE 64 //MB
#include <stdint.h>
uint64_t g_total_recv = 0; 
void StatisHandle() 
{ 
    static time_t last_print_time = time(NULL);
    static uint64_t last_total_recv = 0;
    time_t cur_time = time(NULL);
    if (cur_time-last_print_time>=1) {
        uint32_t recv_per_second = (uint32_t)(g_total_recv-last_total_recv);
        last_total_recv = g_total_recv;
        last_print_time = cur_time;
        fprintf(stdout, "recv_per_second=%lu\tg_total_recv=%llu\n", (unsigned long int)recv_per_second, (unsigned long long int)g_total_recv);
    }
}

//启动map服务，nPort监听端口，nThreadNum工作线程数量
int main()
{
    signal(SIGPIPE, SIG_IGN);
    int nPort      = 23456; 
    int nThreadNum = 12;

    #ifdef DAsyncMapCli

    //boost::shared_ptr<TAsyncProcessor>       handler(new AsyncMapServiceHandler());
    //boost::shared_ptr<TProtocolFactory>      protocolFactory(new TBinaryProtocolFactory());       
    //boost::shared_ptr<TAsyncBufferProcessor> processor(new TAsyncBufferProcessor(handler, protocolFactory));
    //TEvhttpServer server(processor, nPort);
    //server.serve();
    
    #else 

    //boost::shared_ptr<MapServiceHandler> handler(new MapServiceHandler());
    //boost::shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));
    //boost::shared_ptr<TTransportFactory> transportFactory(new THttpServerTransportFactory());
    //boost::shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());
    //boost::shared_ptr<TServerTransport>  serverTransport(new TServerSocket(nPort));
    //TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    //server.serve();

    //boost::shared_ptr<MapServiceHandler> handler(new MapServiceHandler());
    //boost::shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));                                                                 
    //boost::shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());       

    //boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(
    //        new PosixThreadFactory(apache::thrift::concurrency::PosixThreadFactory::ROUND_ROBIN,
    //                apache::thrift::concurrency::PosixThreadFactory::NORMAL,
    //                THREAD_STACK_SIZE)
    //        );
    //boost::shared_ptr<ThreadManager>     threadManager = ThreadManager::newSimpleThreadManager(nThreadNum); 
    //threadManager->threadFactory(threadFactory);
    //threadManager->start();

    //TNonblockingServer server(processor, protocolFactory, nPort, threadManager); 
    //server.serve();
    
    boost::shared_ptr<MapServiceHandler> handler(new MapServiceHandler());
    boost::shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));
    boost::shared_ptr<TServerTransport>  serverTransport(new TServerSocket(nPort));
    boost::shared_ptr<TTransportFactory> transportFactory(new THttpServerTransportFactory());
    boost::shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());

    try {
        boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(
                    new PosixThreadFactory(apache::thrift::concurrency::PosixThreadFactory::ROUND_ROBIN,
                        apache::thrift::concurrency::PosixThreadFactory::NORMAL,
                        THREAD_STACK_SIZE)
                    );
        boost::shared_ptr<ThreadManager>     threadManager = ThreadManager::newSimpleThreadManager(nThreadNum); 
        threadManager->threadFactory(threadFactory);
        threadManager->start();

        ::apache::thrift::server::TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);
        server.setTimeout(1000);
        server.serve();
    } catch (TException& tx) { 
        fprintf(stderr, "failed,tw.what=%s\n", tx.what()); 
    }
    #endif
    return 0;
}


