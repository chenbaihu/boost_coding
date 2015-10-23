#include "AsyncMapCli.h"

#ifdef DAsyncMapCli
#include "EventThread.h"
#include "EventWatcher.h"

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransportUtils.h"
#include "thrift/async/TEvhttpClientChannel.h"

#include <sys/types.h>
#include <sys/time.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::async;

AsyncMapCli::AsyncMapCli(clib::EventThreadPtr _eventThreadPtr, 
        char* _ip, uint16_t _port, 
        uint16_t _conn_timeout/*=100 ms*/, 
        uint16_t _send_timeout/*=100 ms*/, 
        uint16_t _recv_timeout/*=100 ms*/)
    : eventThreadPtr(_eventThreadPtr), 
    ip(std::string(_ip)), 
    port(_port), 
    conn_timeout(_conn_timeout), 
    send_timeout(_send_timeout), 
    recv_timeout(_recv_timeout) 
{
    cliInfo 
        << "ip="              << ip           << "\t"
        << "port="            << port         << "\t" 
        << "conn_timeout_ms=" << conn_timeout << "\t" 
        << "send_timeout="    << send_timeout << "\t" 
        << "recv_timeout="    << recv_timeout;
}

AsyncMapCli::~AsyncMapCli() 
{
} 

bool AsyncMapCli::Init() 
{ /*{{{*/
    if (ip.empty() || port==0 || conn_timeout==0 
            || send_timeout==0 || recv_timeout==0) { 
        fprintf(stderr, "Open\t"
                "cliInfo=%s\t"
                "check failed",  
                cliInfo.str().c_str());
        return false;
    }

    if (eventThreadPtr==NULL) {
        eventThreadPtr.reset(new clib::EventThread());
        if (!eventThreadPtr->Init(20000)) {
            fprintf(stderr, "Open eventThreadPtr->Init failed");
            return false;
        }
    }
    return true;
} /*}}}*/

bool AsyncMapCli::AddTask(clib::EventThread::TaskPtr task)
{
    eventThreadPtr->AddTask(task);
    return true;
}

#endif //#ifdef DAsyncMapCli



