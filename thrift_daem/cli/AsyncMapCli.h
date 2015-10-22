#ifndef _ASYNC_MAP_CLI_H_
#define _ASYNC_MAP_CLI_H_ 

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <tr1/functional>
#include <tr1/memory>
#include <sstream>
#include <string>
#include <vector>
using std::string;

#include <stdint.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include "map_types.h"

#ifdef DAsyncMapCli

namespace clib {
    class EventThread;
    typedef std::tr1::shared_ptr<EventThread>  EventThreadPtr;
    class EventWatcher;
    typedef std::tr1::shared_ptr<EventWatcher> EventWatcherPtr;
}

enum ReqStatus {
    kBeg     = 0,
    kEnd     = 1,
};
typedef struct ComputeReqWithTimeOut { 
    ReqStatus  status;
    ComputeReq creq;
    clib::EventWatcherPtr eventWatcherPtr; 
    ComputeReqWithTimeOut() {
        status = kBeg;
    }
} ComputeReqWithTimeOut;
typedef std::tr1::shared_ptr<ComputeReqWithTimeOut> ComputeReqWithTimeOutPtr;

enum RspErrorCode {
    kOK         = 0,
    kNoConnect  = 1,
    kTimeOut    = 2,
    kConnectErr = 3,
    kUnknown    = 4, 
};
typedef struct ComputeRespWithErrorCode {
    int ret;
    ComputeResp crsp;
    ComputeRespWithErrorCode() {
        ret = kOK;
    }
} ComputeRespWithErrorCode;
typedef std::tr1::shared_ptr<ComputeRespWithErrorCode> ComputeRespWithErrorCodePtr;

namespace apache { namespace thrift { 
    namespace transport { 
        class TSocket; 
        class TTransport; 
    } // apache::thrift::transport 
    namespace protocol { 
        class TProtocol;
    } // apache::thrift::protocol
    namespace async {
        class TEvhttpClientChannel;
        class TAsyncChannel;
    } // apache::thrift::async
}} // apache::thrift

typedef boost::shared_ptr<apache::thrift::transport::TSocket>    TSocketPtr;
typedef boost::shared_ptr<apache::thrift::transport::TTransport> TTransportPtr;
typedef boost::shared_ptr<apache::thrift::protocol::TProtocol>   TProtocolPtr;
typedef boost::shared_ptr<apache::thrift::protocol::TBinaryProtocolFactory>   TBinaryProtocolFactoryPtr;
typedef boost::shared_ptr<apache::thrift::async::TAsyncChannel>  TAsyncChannelPtr;
typedef boost::shared_ptr<apache::thrift::async::TEvhttpClientChannel>   TEvhttpClientChannelPtr;

class MapServiceCobClient; 
typedef boost::shared_ptr<MapServiceCobClient> MapServiceCobClientPtr; 

class AsyncMapCli {
public:
    typedef std::tr1::function<void(const ComputeRespWithErrorCode& crsp)> CallBack;
public:
    AsyncMapCli(clib::EventThreadPtr _eventThreadPtr, 
            char* _ip, uint16_t _port, 
            uint16_t _conn_timeout=100/*ms*/, 
            uint16_t _send_timeout=100/*ms*/, 
            uint16_t _recv_timeout=100/*ms*/); 
    ~AsyncMapCli();

    //TODO use template function merge Compute Write
    void Compute(const ComputeReqWithTimeOutPtr& creq_ptr, CallBack cb); 

public:
    std::string get_cliInfo() { return cliInfo.str(); }

private:
    void ComputeHandle(const ComputeReqWithTimeOutPtr& creq_ptr, CallBack cb); 

private:
    bool Open();
    bool Close();
    static void Cob(MapServiceCobClient* client, AsyncMapCli* asyncMapCli, ComputeReqWithTimeOutPtr& creq_ptr, CallBack cb);

private:
    int64_t TaskNum() { return wait_to_do_task_num; }
    int64_t Inc() { return __sync_fetch_and_add(&wait_to_do_task_num, 1); }
    int64_t Dec() { return __sync_fetch_and_add(&wait_to_do_task_num, -1); }

private:
    clib::EventThreadPtr      eventThreadPtr;
    
    std::string         ip;
    uint16_t            port;
    uint16_t            conn_timeout;
    uint16_t            send_timeout;
    uint16_t            recv_timeout;
    bool                init; 
    int64_t             wait_to_do_task_num;
    int64_t             max_wait_to_do_task_num;
    
    std::stringstream   cliInfo;
    
    TBinaryProtocolFactoryPtr tBinaryProtocolFactoryPtr;    
    TAsyncChannelPtr          tAsyncChannelPtr;
    MapServiceCobClientPtr    cobClientPtr; 
}; 
typedef std::tr1::shared_ptr<AsyncMapCli> AsyncMapCliPtr; 
typedef std::vector<AsyncMapCliPtr>       AsyncMapCliPtrVec;

#endif //#ifdef DAsyncMapCli

#endif 


