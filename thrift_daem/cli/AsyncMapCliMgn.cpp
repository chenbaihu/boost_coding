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
                return false;
            }
            AsyncMapCliPtr asyncMapCliPtr(new AsyncMapCli(eventThreadPtr, &(ips[ipi][0]), ports[porti], connTimeOut, sendTimeOut, recvTimeOut));
            asyncMapCliPtrVec[index] = asyncMapCliPtr;
            index++;
        }
    }
    return true;
}

void AsyncMapCliMgn::Compute(const ComputeReq& creq, AsyncMapCli::CallBack cb)
{
    if (asyncMapCliPtrVec.empty()) {
        ComputeResp crsp;
        cb(-3, crsp);
        return;
    }

    int i = random()%asyncMapCliPtrVec.size();
    AsyncMapCliPtr asyncMapCliPtr = asyncMapCliPtrVec[i];
    asyncMapCliPtr->Compute(creq, cb);
    return;
}


