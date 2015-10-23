#include "AsyncMapCliMgn.h"
#include "AsyncMapCli.h"
#include "EventThreadPool.h"
#include "MapService.h"

#include <sys/types.h>
#include <time.h>

AsyncMapCliMgn::AsyncMapCliMgn(
            clib::EventThreadPoolPtr _eventThreadPoolPtr,
            std::vector<std::string> _ips,
            std::vector<uint16_t> _ports, 
            uint16_t _connTimeOut/*=1000*/, 
            uint16_t _sendTimeOut/*=1000*/,
            uint16_t _recvTimeOut/*=1000*/)
    : eventThreadPoolPtr(_eventThreadPoolPtr), 
      ips(_ips),
      ports(_ports), 
      connTimeOut(_connTimeOut), 
      sendTimeOut(_sendTimeOut), 
      recvTimeOut(_recvTimeOut) 
{
    srandom(time(NULL));
}

AsyncMapCliMgn::~AsyncMapCliMgn() 
{
}

bool AsyncMapCliMgn::Init() 
{
    if (ips.empty() || ports.empty() || 
                connTimeOut==0 || sendTimeOut==0 || recvTimeOut==0) {
        return false;
    }

    if (eventThreadPoolPtr==NULL) {
        //OR create default
        return false;
    }

    int index = 0;

    asyncMapCliPtrVec.resize(ports.size()*ips.size());
    for (size_t porti=0; porti<ports.size(); porti++) { 
        for (size_t ipi=0; ipi<ips.size(); ipi++) { 
            clib::EventThreadPtr eventThreadPtr = eventThreadPoolPtr->GetNextEventThread();
            if (eventThreadPtr==NULL) {
                //TODO write log
                fprintf(stderr, "Init eventThreadPtr==NULL failed\n");
                return false;
            }
            AsyncMapCliPtr asyncMapCliPtr(new AsyncMapCli(eventThreadPtr, &(ips[ipi][0]), ports[porti], connTimeOut, sendTimeOut, recvTimeOut));
            if (!asyncMapCliPtr->Init()) {
                //TODO write log
                fprintf(stderr, "Init asyncMapCliPtr->Init failed\n");
                return false;
            }
            asyncMapCliPtrVec[index] = asyncMapCliPtr;
            index++;
        }
    }
    return true;
}

AsyncMapCliPtr& AsyncMapCliMgn::GetNextAsyncMapCliPtr() 
{
    uint64_t cli_index = AtomicInc();
    return asyncMapCliPtrVec[cli_index];
}

//void AsyncMapCliMgn::Compute(const ComputeReqWithTimeOutPtr& creq_ptr, AsyncMapCli::CallBack cb)
//{
//    if (asyncMapCliPtrVec.empty()) {
//        ComputeRespWithErrorCode crsp;
//        crsp.ret = kEventErr;
//        cb(crsp);
//        return;
//    }
//
//    int i = random()%asyncMapCliPtrVec.size();
//    AsyncMapCliPtr asyncMapCliPtr = asyncMapCliPtrVec[i];
//    asyncMapCliPtr->Compute(creq_ptr, cb);
//    return;
//}


