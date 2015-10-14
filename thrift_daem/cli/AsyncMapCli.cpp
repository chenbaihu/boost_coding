#include "AsyncMapCli.h"

#include "MapService.h"             // /usr/local/bin/thrift -r -gen cpp idl/map.thrift  生成的文件中的一个

#ifdef DAsyncMapCli

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransportUtils.h"
#include "thrift/async/TEvhttpClientChannel.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::async;

AsyncMapCli::AsyncMapCli(event_base* _evbase, 
        char* _ip, uint16_t _port, 
        uint16_t _conn_timeout/*=100 ms*/, 
        uint16_t _send_timeout/*=100 ms*/, 
        uint16_t _recv_timeout/*=100 ms*/)
    : evbase(_evbase), 
    ip(std::string(_ip)), 
    port(_port), 
    conn_timeout(_conn_timeout), 
    send_timeout(_send_timeout), 
    recv_timeout(_recv_timeout), 
    init(false) 
{
    cliInfo 
        << "ip="              << ip           << "\t"
        << "port="            << port         << "\t" 
        << "conn_timeout_ms=" << conn_timeout << "\t" 
        << "send_timeout="    << send_timeout << "\t" 
        << "recv_timeout="    << recv_timeout;
    Open();  //没有考虑成功还是失败
}

AsyncMapCli::~AsyncMapCli() 
{
    cobClientPtr.reset();
    tAsyncChannelPtr.reset();
} 

bool AsyncMapCli::Open() 
{ /*{{{*/
    if (init) {
        return true;
    }

    if (evbase==NULL 
            || ip.empty() || port==0 || conn_timeout==0 
            || send_timeout==0 || recv_timeout==0) { 
        fprintf(stderr, "Open\t"
                "cliInfo=%s\t"
                "check failed",  
                cliInfo.str().c_str());
        return false;
    }


    tAsyncChannelPtr.reset(new TEvhttpClientChannel(ip.c_str(), "/", ip.c_str(), port, evbase));
    cobClientPtr.reset(new MapServiceCobClient(tAsyncChannelPtr, new TBinaryProtocolFactory()));
    init = true;
    return true;
} /*}}}*/

void AsyncMapCli::Cob(MapServiceCobClient* client, CallBack cb)
{
    ComputeResp crsp;
    if (client==NULL) {
        cb(crsp);
    }
    client->recv_compute(crsp);
    cb(crsp);
}

bool AsyncMapCli::Compute(const ComputeReq& creq, CallBack cb) 
{ /*{{{*/
    if (!Open()) { 
        fprintf(stderr, "Compute\t"
                "cliInfo=%s\t" 
                "sliceID=%d\t"
                "oidList.size=%ld\t"
                "didList.size=%ld\t" 
                "Open failed",
                cliInfo.str().c_str(), 
                creq.mapsplitId,
                creq.oidList.size(), 
                creq.didList.size());
        return false;
    }

    try {
        cobClientPtr->compute(boost::bind(&AsyncMapCli::Cob, _1, cb), creq);
    } catch (TException& tx) { 
        fprintf(stderr, "Compute\t"
                "cliInfo=%s\t"
                "sliceID=%d\t" 
                "oidList.size=%ld\t"
                "didList.size=%ld\t"
                "failed,tw.what=%s",
                cliInfo.str().c_str(), 
                creq.mapsplitId, 
                creq.oidList.size(), 
                creq.didList.size(), 
                tx.what());
        return false;
    }
    fprintf(stdout, "Compute\t"
            "cliInfo=%s\t"
            "sliceID=%u\t"
            "oidList.size=%ld\t"
            "didList.size=%ld\t" 
            "succ", 
            cliInfo.str().c_str(), 
            (uint16_t)creq.mapsplitId, 
            creq.oidList.size(), 
            creq.didList.size());
    return true;
} /*}}}*/

#endif //#ifdef DAsyncMapCli



