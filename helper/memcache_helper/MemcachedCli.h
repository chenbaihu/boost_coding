#ifndef _MEMCACHEDCLI_H_
#define _MEMCACHEDCLI_H_

#include <tr1/memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdint.h> 
#include "mutex.h"
#include "ssMemcached.h"
#include "NodeMGR.h"

namespace util {
enum CliCType {
    kCKVCliSC = 0,
    kCKVCliLC = 1, 
};

class MemcachedCli { 
public:
    MemcachedCli(const std::string& ip, 
                uint16_t port=11211, 
                int conn_time_out_ms=100, 
                int send_time_out_ms=200, 
                int recv_time_out_ms=200, 
                CliCType cli_c_type=kCKVCliSC);
    MemcachedCli(nodemanager::NodeInfoPtr& node_ptr,  
                int conn_time_out_ms=100, 
                int send_time_out_ms=200, 
                int recv_time_out_ms=200, 
                CliCType cli_c_type=kCKVCliSC);
    ~MemcachedCli();

public:
    const std::string Info()            { return serverInfo.str(); }
    nodemanager::NodeInfoPtr& NodePtr() { return nodePtr;          }

public:
    int MultiGet(MGetKeyType& keys, MGetResultType& result);

    int Set(std::string& key, 
                std::string& value, 
                int32_t exptime_time_s=3000/*s*/, 
                uint32_t flag=0);

private:
    int MultiGetImpl(MGetKeyType& keys, MGetResultType& result);

    int SetImpl(std::string& key, 
                std::string& value, 
                int32_t exptime_time_s=3000/*s*/, 
                uint32_t flag=0); 

private:
    bool Open();
    void Close();

private:
    std::string  serverIp;
    uint16_t     serverPort;
    int          connTimeOutMs;
    int          sendTimeOutMs;
    int          recvTimeOutMs;
    CliCType     cliCType;
    nodemanager::NodeInfoPtr nodePtr;  
    Mutex        mutex;
    ssMemcached* pCKV;

private:
    std::stringstream serverInfo;
};
typedef std::tr1::shared_ptr<MemcachedCli> MemcachedCliPtr; 

} // namespace util

#endif
