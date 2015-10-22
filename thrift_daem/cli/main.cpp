#ifndef DAsyncMapCli
#include "MapCli.h" 
#else 
#include "AsyncMapCliMgn.h" 
#endif
#include "map_types.h"
#include "EventThreadPool.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <signal.h>

void ShowRsp(const ComputeReqWithTimeOutPtr creq_ptr, ComputeRespWithErrorCode crsp)
{
    if (creq_ptr->status != kBeg) {
        return;
    }
    creq_ptr->status = kEnd;

    if (crsp.ret!=0) {
        fprintf(stdout, "Compute failed, tid=%ld ret=%d\n", pthread_self(), crsp.ret);
        return;
    } 
    fprintf(stdout, "Compute succ, tid=%ld crsp.data=%s\n", pthread_self(), crsp.crsp.data.c_str());
}

int main(int argc, char* argv[]) { 
    signal(SIGPIPE, SIG_IGN);  

    #ifndef DAsyncMapCli
    char ip[] = "127.0.0.1"; 
    ComputeResp crsp;
    MapCli mapCli(ip, 23456, 1000, 1000, 1000);

    ComputeReq creq;
    creq.type   = OrderType::COMMONORDER;
    creq.jobId  = 0;
    creq.cityId = 100;
    creq.mapsplitId = 0;

    for (int64_t i=0; i<1000; i++) { 
        creq.oidList.push_back(i);
        creq.didList.push_back(i*2);
    }
    if (!mapCli.Compute(creq, crsp)) {
        fprintf(stderr, "Compute failed\n");
        return 1;
    }
    fprintf(stdout, "Compute succ, crsp.data=%s\n", crsp.data.c_str());
    #else
    clib::EventThreadPoolPtr eventThreadPoolPtr(new clib::EventThreadPool(10));
    if (!eventThreadPoolPtr->Start()) {
        fprintf(stderr, "eventThreadPoolPtr->Start failed\n");
        return false;
    }

    std::vector<std::string> ips;
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    ips.push_back("127.0.0.1");
    std::vector<uint16_t>    ports;
    ports.push_back(23456);
    //ports.push_back(23457);
    //ports.push_back(23458);
    uint16_t connTimeOut=1000;
    uint16_t sendTimeOut=1000;
    uint16_t recvTimeOut=1000;

    AsyncMapCliMgn asyncMapCliMgn(eventThreadPoolPtr, ips, ports, connTimeOut, sendTimeOut, recvTimeOut);
    if (!asyncMapCliMgn.Init()) {
        fprintf(stderr, "asyncMapCliMgn.Init  failed\n");
        return false;
    }

    while (true) { 
        ComputeReqWithTimeOutPtr creq_ptr(new ComputeReqWithTimeOut);
        ComputeReq& creq = creq_ptr->creq;
        creq.type   = OrderType::COMMONORDER;
        creq.jobId  = 0;
        creq.cityId = 100;
        creq.mapsplitId = 0;
        for (int64_t i=0; i<1000; i++) { 
            creq.oidList.push_back(i);
            creq.didList.push_back(i*2);
        }
        asyncMapCliMgn.Compute(creq_ptr, std::tr1::bind(&ShowRsp, creq_ptr, std::tr1::placeholders::_1));
        usleep(10000);
    }
    #endif
    return 0;
}
