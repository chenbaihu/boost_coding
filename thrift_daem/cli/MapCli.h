#ifndef _MAP_CLI_H_
#define _MAP_CLI_H_ 

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <tr1/memory>
#include <sstream>
#include <string>
#include <vector>
using std::string;

#include <stdint.h>

#ifndef DAsyncMapCli

namespace apache { namespace thrift { 
    namespace transport { 
        class TSocket; 
        class TTransport; 
    } // apache::thrift::transport 
    namespace protocol { 
        class TProtocol;
    } // apache::thrift::protocol 
}} // apache::thrift

typedef boost::shared_ptr<apache::thrift::transport::TSocket>    TSocketPtr;
typedef boost::shared_ptr<apache::thrift::transport::TTransport> TTransportPtr;
typedef boost::shared_ptr<apache::thrift::protocol::TProtocol>   TProtocolPtr;

class MapServiceClient; 
typedef boost::shared_ptr<MapServiceClient> MapServiceClientPtr; 

class ComputeReq; 
class ComputeResp;

class MapCli {
public:
    MapCli( char* _ip, uint16_t _port, 
            uint16_t _conn_timeout=100/*ms*/, 
            uint16_t _send_timeout=100/*ms*/, 
            uint16_t _recv_timeout=100/*ms*/); 
    ~MapCli();

    //TODO use template function merge Compute Write
    bool Compute(
            const ComputeReq& creq, 
            ComputeResp& crsp); 
private:
    bool Open();
    void Close(); 

private: 
    std::string         ip;
    uint16_t            port;
    uint16_t            conn_timeout;
    uint16_t            send_timeout;
    uint16_t            recv_timeout;
    
    std::stringstream   cliInfo;

    TSocketPtr          socketPtr;
    TTransportPtr       transportPtr;
    TProtocolPtr        protocolPtr;
    MapServiceClientPtr clientPtr; 
}; 
typedef std::tr1::shared_ptr<MapCli> MapCliPtr; 
typedef std::vector<MapCliPtr>       MapCliPtrVec;

#endif //#ifndef DAsyncMapCli

#endif 


