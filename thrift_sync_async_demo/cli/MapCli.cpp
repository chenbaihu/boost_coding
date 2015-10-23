#include "MapCli.h"

#include "MapService.h"             // /usr/local/bin/thrift -r -gen cpp idl/map.thrift  生成的文件中的一个

#ifndef DAsyncMapCli

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransportUtils.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

MapCli::MapCli( char* _ip, uint16_t _port, 
        uint16_t _conn_timeout/*=100 ms*/, 
        uint16_t _send_timeout/*=100 ms*/, 
        uint16_t _recv_timeout/*=100 ms*/)
    : ip(std::string(_ip)), 
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
    Open();  //没有考虑成功还是失败
}

MapCli::~MapCli() 
{
    Close();
    clientPtr.reset();
    protocolPtr.reset();
    transportPtr.reset();
    socketPtr.reset();
} 

bool MapCli::Open() 
{ /*{{{*/  
    if (ip.empty() || port==0 || conn_timeout==0 
            || send_timeout==0 || recv_timeout==0) { 
        fprintf(stderr, "Open\t"
                "cliInfo=%s\t"
                "check failed",  
                cliInfo.str().c_str());
        return false;
    }

    Close();

    socketPtr.reset(new TSocket(ip.c_str(), port)); 
    socketPtr->setConnTimeout(conn_timeout);
    socketPtr->setSendTimeout(send_timeout);
    socketPtr->setRecvTimeout(recv_timeout); 

    transportPtr.reset(new TFramedTransport(socketPtr));

    protocolPtr.reset(new TBinaryProtocol(transportPtr)); 
    clientPtr.reset(new MapServiceClient(protocolPtr)); 

    try {  
        transportPtr->open();  
    } catch (TException& tx) {
        fprintf(stderr, "Open\t"
                "cliInfo=%s\t"
                "failed,tw.what=%s", 
                cliInfo.str().c_str(), 
                tx.what());
        return false;
    }
    return true;
} /*}}}*/

void MapCli::Close() 
{ /*{{{*/  
    try { 
        if (transportPtr!=NULL) {
            transportPtr->close();  // to short connet
        }
    } catch (TException& tx) {
        fprintf(stderr, "Close\t"
                "cliInfo%s\t"
                "failed,tw.what=%s", 
                cliInfo.str().c_str(), 
                tx.what());
    }
    return;
} /*}}}*/ 

bool MapCli::Compute(
        const ComputeReq& creq, 
        ComputeResp& crsp) 
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
        clientPtr->compute(crsp, creq);
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

#endif //#ifndef DAsyncMapCli


