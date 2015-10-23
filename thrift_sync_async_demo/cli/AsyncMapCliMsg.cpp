#include "AsyncMapCliMsg.h"
#include "AsyncMapCli.h"
#include "MapService.h"             // /usr/local/bin/thrift -r -gen cpp idl/map.thrift  生成的文件中的一个

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

bool ComputeReqWithTimeOut::Init() 
{ 
    if (asyncMapCliPtr==NULL) {
        return false;
    }

    if (asyncMapCliPtr->get_event_base()==NULL) {
        return false;
    }

    status = kBeg;
    tBinaryProtocolFactoryPtr.reset(new TBinaryProtocolFactory());
    tAsyncChannelPtr.reset(new TEvhttpClientChannel(
                    asyncMapCliPtr->ip.c_str(), 
                    "/", 
                    asyncMapCliPtr->ip.c_str(), 
                    (int)asyncMapCliPtr->port, 
                    asyncMapCliPtr->get_event_base()));
    cobClientPtr.reset(new MapServiceCobClient(tAsyncChannelPtr, tBinaryProtocolFactoryPtr.get()));
    return true;
}

bool ComputeReqWithTimeOut::UnInit() 
{
    cobClientPtr.reset();
    tAsyncChannelPtr.reset();
    tBinaryProtocolFactoryPtr.reset();
    eventWatcherPtr.reset();
    return true;
}

bool ComputeReqWithTimeOut::SetTimeOutHandler(const Handler& handler)
{
    eventWatcherPtr.reset(new clib::TimedEventWatcher(asyncMapCliPtr->get_event_base(), handler));
    if (!eventWatcherPtr->Init()) {
        return false;
    }
    struct timeval cur;
    gettimeofday(&cur, NULL);
    cur.tv_sec  = cur.tv_sec  + asyncMapCliPtr->recv_timeout/1000;
    cur.tv_usec = cur.tv_usec + (asyncMapCliPtr->recv_timeout%1000)*1000;
    eventWatcherPtr->Watch(&cur);
    return true;
}

void ComputeReqWithTimeOut::Compute(CallBack cb)
{ 
    if (!Init()) {
        ComputeRespWithErrorCode crsp;
        crsp.ret = kEventErr;
        cb(crsp);
    }

    clib::EventThread::TaskPtr task(new clib::EventThread::Task(boost::bind(&ComputeReqWithTimeOut::ComputeHandle, shared_from_this(), cb)));
    asyncMapCliPtr->AddTask(task);
}

void ComputeReqWithTimeOut::ComputeHandle(CallBack cb) 
{
    try {
        cobClientPtr->compute(boost::bind(&ComputeReqWithTimeOut::Cob, shared_from_this(), _1, cb), creq);
        //SetTimeOutHandler(std::tr1::bind(&ComputeReqWithTimeOut::Cob, this, NULL, cb));
    } catch (TException& tx) { 
        fprintf(stderr, "Compute\t"
                    "sliceID=%d\t" 
                    "oidList.size=%ld\t"
                    "didList.size=%ld\t"
                    "failed,tw.what=%s\n",
                    creq.mapsplitId, 
                    creq.oidList.size(), 
                    creq.didList.size(), 
                    tx.what());
        return;
    }
    //fprintf(stdout, "Compute\t"
    //        "sliceID=%u\t"
    //        "oidList.size=%ld\t"
    //        "didList.size=%ld\t" 
    //        "succ\n", 
    //        (uint16_t)creq.mapsplitId, 
    //        creq.oidList.size(), 
    //        creq.didList.size());
    //return;
    return;
}

void ComputeReqWithTimeOut::Cob(MapServiceCobClient* client, CallBack cb)
{
    if (status==kEnd) {
        return;
    }
    status = kEnd;

    ComputeRespWithErrorCode crsp;
    if (client==NULL) {
        crsp.ret = kTimeOut;
        cb(crsp);
        UnInit();
    }

    try {
        client->recv_compute(crsp.crsp);
        crsp.ret = kOK;
    } catch (TException& tx) {
        fprintf(stderr, "Cob\t"
                    "pid=%ld\t"
                    "failed,tw.what=%s\n",
                    pthread_self(),
                    tx.what());
        crsp.ret = kUnknown;
    }
    cb(crsp);
    UnInit();
}

#endif //#ifdef DAsyncMapCli

