#ifndef DAsyncMapCli
#include "MapCli.h" 
#else 
#include "AsyncMapCliMgn.h" 
#include "AsyncMapCliMsg.h" 
#endif
#include "map_types.h"
#include "EventThreadPool.h"
#include "EventWatcher.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

//#include <boost/program_options.hpp>

#include <gflags/gflags.h>
DEFINE_string(ip, "127.0.0.1", "thrift server ip");
DEFINE_int32(port, 23456, "thrift server port");
DEFINE_int32(thread_num, 24, "cli thread num");
DEFINE_int32(concurrent, 2, "并发数");

uint64_t g_total_send  = 0;
uint64_t g_total_recv  = 0;
uint64_t g_total_error = 0;

typedef boost::shared_ptr<AsyncMapCliMgn> AsyncMapCliMgnPtr;
AsyncMapCliMgnPtr g_asyncMapCliMgnPtr;
void ProcessReq();
void ProcessRsp(ComputeRespWithErrorCode crsp)
{ 
    if (crsp.ret!=0) {
        __sync_fetch_and_add(&g_total_error, 1);
        #ifdef PRINT
        fprintf(stdout, "ProcessRsp failed, tid=%ld ret=%d\n", pthread_self(), crsp.ret);
        #endif
    } else { 
        __sync_fetch_and_add(&g_total_recv, 1);
        #ifdef PRINT
        fprintf(stdout, "ProcessRsp succ, tid=%ld crsp.data=%s\n", pthread_self(), crsp.crsp.data.c_str()); 
        #endif
    }
    ProcessReq();
}

void ProcessReq()
{
    __sync_fetch_and_add(&g_total_send, 1);
    ComputeReqWithTimeOutPtr creq_ptr(new ComputeReqWithTimeOut(g_asyncMapCliMgnPtr->GetNextAsyncMapCliPtr()));
    ComputeReq& creq = creq_ptr->creq;
    creq.type   = OrderType::COMMONORDER;
    creq.jobId  = 0;
    creq.cityId = 100;
    creq.mapsplitId = 0;
    //for (int64_t i=0; i<1000; i++) { 
    //    creq.oidList.push_back(i);
    //    creq.didList.push_back(i*2);
    //}
    creq_ptr->Compute(std::tr1::bind(&ProcessRsp, std::tr1::placeholders::_1));
}

boost::shared_ptr<clib::TimedEventWatcher> g_twatcher;
void StatisHandle(event_base* evbase);
void Statis(event_base* evbase) 
{
    if (evbase==NULL) {
        fprintf(stderr, "evbase==NULL\n");
        return;
    }
    g_twatcher.reset(new clib::TimedEventWatcher(evbase, boost::bind(&StatisHandle, evbase))); 

    struct timeval timeout;
    gettimeofday(&timeout, NULL);
    timeout.tv_sec = timeout.tv_sec+1;
    g_twatcher->Watch(&timeout);

    if (!g_twatcher->Init()) {
        fprintf(stderr, "g_twatcher->Init failed\n");
        return;
    }
}

void StatisHandle(event_base* evbase)
{
    static uint64_t last_total_send  = 0;
    static uint64_t last_total_recv  = 0;
    static uint64_t last_total_error = 0;
    uint32_t send_per_sec = (uint32_t)(g_total_send-last_total_send);
    last_total_send = g_total_send;
    uint32_t recv_per_sec = (uint32_t)(g_total_recv-last_total_recv);
    last_total_recv = g_total_recv;
    uint32_t error_per_sec = (uint32_t)(g_total_error-last_total_error);
    last_total_error = g_total_error;
    fprintf(stdout, "send_per_sec=%lu\trecv_per_sec=%lu\terror_per_sec=%lu\ttotal_error=%llu\n", 
                (unsigned long int)send_per_sec, 
                (unsigned long int)recv_per_sec, 
                (unsigned long int)error_per_sec, 
                (unsigned long long int)g_total_error);
    if (evbase!=NULL) {
        Statis(evbase);
    }
}

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::string ip      = FLAGS_ip;
    uint16_t port       = (uint16_t)FLAGS_port;
    uint16_t thread_num = (uint16_t)FLAGS_thread_num; 
    uint16_t concurrent = (uint16_t)FLAGS_concurrent; 
    fprintf(stdout, "ip=%s\tport=%d\tthread_num=%d\tconcurrent=%d\n", ip.c_str(), port, thread_num, concurrent);
    sleep(3);

    signal(SIGPIPE, SIG_IGN);  

    #ifndef DAsyncMapCli
    ComputeResp crsp;
    MapCli mapCli(ip.c_str(), port, 1000, 1000, 1000);

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
    clib::EventThreadPoolPtr eventThreadPoolPtr(new clib::EventThreadPool(thread_num));
    if (!eventThreadPoolPtr->Start()) {
        fprintf(stderr, "eventThreadPoolPtr->Start failed\n");
        return false;
    }

    //clib::EventThreadPtr eventThreadPtr = eventThreadPoolPtr->GetNextEventThread();
    //Statis(eventThreadPtr->event_base());

    //clib::EventThreadPtr eventThreadPtr(new clib::EventThread()); 
    //if (eventThreadPtr)

    std::vector<std::string> ips;
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    ips.push_back(ip);
    std::vector<uint16_t>    ports;
    ports.push_back(port);
    //ports.push_back(23457);
    //ports.push_back(23458);
    uint16_t connTimeOut=1000;
    uint16_t sendTimeOut=1000;
    uint16_t recvTimeOut=1000;

    g_asyncMapCliMgnPtr.reset(new AsyncMapCliMgn(eventThreadPoolPtr, ips, ports, connTimeOut, sendTimeOut, recvTimeOut));
    if (!g_asyncMapCliMgnPtr->Init()) {
        fprintf(stderr, "g_asyncMapCliMgnPtr.Init  failed\n");
        return false;
    }

    for (int16_t i=0; i<concurrent; i++) { 
        ProcessReq();
    }

    while (true) { 
        StatisHandle(NULL);
        sleep(1);
    }
    #endif
    return 0;
}
