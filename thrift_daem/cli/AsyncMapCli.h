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

#ifdef DAsyncMapCli

namespace clib {
    class EventThread;
    typedef boost::shared_ptr<EventThread> EventThreadPtr;
}

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
typedef boost::shared_ptr<apache::thrift::async::TAsyncChannel>  TAsyncChannelPtr;
typedef boost::shared_ptr<apache::thrift::async::TEvhttpClientChannel>   TEvhttpClientChannelPtr;

class MapServiceCobClient; 
typedef boost::shared_ptr<MapServiceCobClient> MapServiceCobClientPtr; 
class ComputeReq; 
class ComputeResp;

class AsyncMapCli {
public:
    typedef std::tr1::function<void(int ret, const ComputeResp& crsp)> CallBack;
public:
    AsyncMapCli(clib::EventThreadPtr _eventThreadPtr, 
            char* _ip, uint16_t _port, 
            uint16_t _conn_timeout=100/*ms*/, 
            uint16_t _send_timeout=100/*ms*/, 
            uint16_t _recv_timeout=100/*ms*/); 
    ~AsyncMapCli();

    //TODO use template function merge Compute Write
    void Compute(const ComputeReq& creq, CallBack cb); 

private:
    void ComputeHandle(const ComputeReq& creq, CallBack cb); 

private:
    bool Open();
    static void Cob(MapServiceCobClient* client, CallBack cb);

private:
    clib::EventThreadPtr      eventThreadPtr;
    
    std::string         ip;
    uint16_t            port;
    uint16_t            conn_timeout;
    uint16_t            send_timeout;
    uint16_t            recv_timeout;
    bool                init; 
    
    std::stringstream   cliInfo;

    TAsyncChannelPtr       tAsyncChannelPtr;
    MapServiceCobClientPtr cobClientPtr; 
}; 
typedef std::tr1::shared_ptr<AsyncMapCli> AsyncMapCliPtr; 
typedef std::vector<AsyncMapCliPtr>       AsyncMapCliPtrVec;

#endif //#ifdef DAsyncMapCli

#endif 


