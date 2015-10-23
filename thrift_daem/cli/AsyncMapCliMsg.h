#ifndef _ASYNCMAPCLIMSG_H_
#define _ASYNCMAPCLIMSG_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <tr1/functional>
#include <tr1/memory>

#include "thrift/protocol/TBinaryProtocol.h"

#include "map_types.h"

#ifdef DAsyncMapCli

namespace clib {
    class EventThread;
    typedef std::tr1::shared_ptr<EventThread>  EventThreadPtr;
    //typedef boost::shared_ptr<EventThread>  EventThreadPtr;
    class EventWatcher;
    typedef std::tr1::shared_ptr<EventWatcher> EventWatcherPtr;
    //typedef boost::shared_ptr<EventWatcher> EventWatcherPtr;
}

namespace apache { namespace thrift { 
    namespace transport { 
        class TSocket; 
        class TTransport; 
    } // apache::thrift::transport 
    namespace protocol { 
        class TProtocol;
        //class TBinaryProtocolFactory;
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

class AsyncMapCli;
typedef std::tr1::shared_ptr<AsyncMapCli> AsyncMapCliPtr;

enum ReqStatus {
    kBeg     = 0,
    kEnd     = 1,
};

struct ComputeRespWithErrorCode;
typedef std::tr1::function<void()> Handler;
struct ComputeReqWithTimeOut : public boost::enable_shared_from_this<ComputeReqWithTimeOut> { 
    typedef std::tr1::function<void(const ComputeRespWithErrorCode& crsp)> CallBack;
    //1
    ReqStatus                 status;
    AsyncMapCliPtr            asyncMapCliPtr;
    clib::EventWatcherPtr     eventWatcherPtr; 

    //2
    TBinaryProtocolFactoryPtr tBinaryProtocolFactoryPtr;    
    TAsyncChannelPtr          tAsyncChannelPtr;

    //3
    ComputeReq                creq;
    MapServiceCobClientPtr    cobClientPtr; 

    ComputeReqWithTimeOut(AsyncMapCliPtr _asyncMapCliPtr) {
        asyncMapCliPtr = _asyncMapCliPtr;
        status = kBeg;
    }

    ~ComputeReqWithTimeOut() {
        //fprintf(stdout, "~ComputeReqWithTimeOut\n");
        UnInit();
    }

    void Compute(CallBack cb);
private:
    bool Init(); 
    bool UnInit();
    void ComputeHandle(CallBack cb);
    bool SetTimeOutHandler(const Handler& handler);
    void Cob(MapServiceCobClient* client, CallBack cb);
};
typedef boost::shared_ptr<ComputeReqWithTimeOut> ComputeReqWithTimeOutPtr;

enum RspErrorCode {
    kOK         = 0,
    kNoConnect  = 1,
    kTimeOut    = 2,
    kConnectErr = 3,
    kEventErr   = 4,
    kUnknown    = 5, 
};
struct ComputeRespWithErrorCode {
    RspErrorCode ret;
    ComputeResp  crsp;
    ComputeRespWithErrorCode() {
        ret = kOK;
    }
};
typedef boost::shared_ptr<ComputeRespWithErrorCode> ComputeRespWithErrorCodePtr;

#endif //#ifdef DAsyncMapCli

#endif //#ifndef _ASYNCMAPMSG_H_
