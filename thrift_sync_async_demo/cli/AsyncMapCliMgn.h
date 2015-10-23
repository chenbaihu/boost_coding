#ifndef _ASYNC_MAP_CLI_MGN_H_
#define _ASYNC_MAP_CLI_MGN_H_ 

#include "AsyncMapCli.h"

#include <tr1/functional>
#include <tr1/memory>
#include <vector>
#include <stdint.h>

namespace clib {
    class EventThreadPool;
    typedef std::tr1::shared_ptr<EventThreadPool> EventThreadPoolPtr;
}

class AsyncMapCliMgn {
public:
    AsyncMapCliMgn(clib::EventThreadPoolPtr _eventThreadPoolPtr,
                std::vector<std::string> _ips,
                std::vector<uint16_t> _ports, 
                uint16_t _connTimeOut=1000, 
                uint16_t _sendTimeOut=1000,
                uint16_t _recvTimeOut=1000);
    ~AsyncMapCliMgn();

public:
    bool Init();
    AsyncMapCliPtr& GetNextAsyncMapCliPtr();
    //void Compute(const ComputeReqWithTimeOutPtr& creq_ptr, AsyncMapCli::CallBack cb); 

    //TODO 检车这些ip和port，哪些是ok的

private:
    uint64_t AtomicInc() {
        if (asyncMapCliPtrVec.empty()) {
            return 0;
        }
        uint64_t index = __sync_fetch_and_add(&atomic_index, 1);
        return index%asyncMapCliPtrVec.size();
    }

private:
    clib::EventThreadPoolPtr eventThreadPoolPtr;
    
    std::vector<std::string> ips;
    std::vector<uint16_t>    ports;
    uint16_t                 connTimeOut;
    uint16_t                 sendTimeOut;
    uint16_t                 recvTimeOut;
    uint64_t                 atomic_index;

    AsyncMapCliPtrVec        asyncMapCliPtrVec;
};

#endif // _ASYNC_MAP_CLI_MGN_H_

