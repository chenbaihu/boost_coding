#include "MapServiceHandler.h"

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/server/TSimpleServer.h"
#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/server/TNonblockingServer.h"

#include <stdio.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace apache::thrift::concurrency;

using boost::shared_ptr;

#define THREAD_STACK_SIZE 64 //MB

//启动map服务，nPort监听端口，nThreadNum工作线程数量
int main()
{
    int nPort      = 23456; 
    int nThreadNum = 12;

    boost::shared_ptr<MapServiceHandler> handler(new MapServiceHandler());

    boost::shared_ptr<TProcessor>        processor(new MapServiceProcessor(handler));                                                                 
    boost::shared_ptr<TProtocolFactory>  protocolFactory(new TBinaryProtocolFactory());       
    boost::shared_ptr<ThreadManager>     threadManager = ThreadManager::newSimpleThreadManager(nThreadNum); 

    boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(
            new PosixThreadFactory(apache::thrift::concurrency::PosixThreadFactory::ROUND_ROBIN,
                    apache::thrift::concurrency::PosixThreadFactory::NORMAL,
                    THREAD_STACK_SIZE)
            );
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    TNonblockingServer server(processor, protocolFactory, nPort, threadManager);
    server.serve();
}



