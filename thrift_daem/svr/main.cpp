#ifdef DAsyncMapCli
#include "AsyncMapServiceHandler.h"
#else
#include "MapServiceHandler.h"
#endif

#include "thrift/protocol/TBinaryProtocol.h"

#include "thrift/server/TSimpleServer.h"
#include "thrift/server/TNonblockingServer.h"

#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/THttpServer.h"

#include "thrift/async/TAsyncProtocolProcessor.h"
#include "thrift/async/TEvhttpServer.h"

#include <stdio.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;
using namespace ::apache::thrift::async;

using boost::shared_ptr;

#define THREAD_STACK_SIZE 64 //MB

//启动map服务，nPort监听端口，nThreadNum工作线程数量
int main()
{
    int nPort      = 23456; 
    int nThreadNum = 12;

    #ifdef DAsyncMapCli

    //boost::shared_ptr<TAsyncProcessor>       handler(new AsyncMapServiceHandler());
    //boost::shared_ptr<TProtocolFactory>      protocolFactory(new TBinaryProtocolFactory());       
    //boost::shared_ptr<TAsyncBufferProcessor> processor(new TAsyncBufferProcessor(handler, protocolFactory));
    //TEvhttpServer server(processor, nPort);
    //server.serve();
    
    #else

    shared_ptr<MapServiceHandler> handler(new MapServiceHandler());
    shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));
    shared_ptr<TServerTransport>  serverTransport(new TServerSocket(nPort));
    shared_ptr<TTransportFactory> transportFactory(new THttpServerTransportFactory());
    shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());
 
    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();

    //boost::shared_ptr<MapServiceHandler> handler(new MapServiceHandler());
    //boost::shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));                                                                 
    //boost::shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());       
    //boost::shared_ptr<ThreadManager>     threadManager = ThreadManager::newSimpleThreadManager(nThreadNum); 

    //boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(
    //        new PosixThreadFactory(apache::thrift::concurrency::PosixThreadFactory::ROUND_ROBIN,
    //                apache::thrift::concurrency::PosixThreadFactory::NORMAL,
    //                THREAD_STACK_SIZE)
    //        );
    //threadManager->threadFactory(threadFactory);
    //threadManager->start();

    //TNonblockingServer server(processor, protocolFactory, nPort, threadManager);
    //server.serve();
    #endif
    return 0;
}


