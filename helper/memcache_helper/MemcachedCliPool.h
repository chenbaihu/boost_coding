#ifndef _MEMCACHEDCLIPOOL_H_
#define _MEMCACHEDCLIPOOL_H_

#include <tr1/memory>
#include <map>
#include <vector>
#include <stdint.h>

namespace nodemanager {
    struct  NodeInfo;
    typedef std::tr1::shared_ptr<NodeInfo> NodeInfoPtr;
} // namespace nodemanager

namespace util {

typedef std::vector<std::string/*key*/>                    MGetKeyType;
typedef std::map<std::string/*key*/, std::string/*value*/> MGetResultType;

class   MemcachedCli;
typedef std::tr1::shared_ptr<MemcachedCli> MemcachedCliPtr;

class MemcachedCliPool {
public:
    typedef std::vector<MemcachedCliPtr> MemcachedCliPtrVec; 

public:
    MemcachedCliPool();
    ~MemcachedCliPool();

public:
    bool Init(const std::vector<std::string>& ips, 
                const std::vector<uint32_t>& ports, 
                int conn_time_ms=100, 
                int send_time_ms=200, 
                int recv_time_ms=200, 
                int max_retry_num=1, 
                int cli_c_type=0/*kCKVCliSC*/);

public:
    int MultiGet(MGetKeyType& keys, MGetResultType& result);
    
    //int MultiSet(MCKVLine& lines); 
    
    //int Set(CKVLine& line); 

    int Set(std::string& key, 
                std::string& value, 
                int32_t exptime_time_s=3000/*s*/, 
                uint32_t flag=0);

private:
    bool InitLCPool();

private:
    MemcachedCli* GetMemcacheCli(nodemanager::NodeInfoPtr& last_node_ptr); 
    MemcachedCli* GetMemcacheCliSC(nodemanager::NodeInfoPtr& last_node_ptr);
    MemcachedCli* GetMemcacheCliLC(nodemanager::NodeInfoPtr& last_node_ptr);
    MemcachedCli* GetGoodMemcacheCliSC(nodemanager::NodeInfoPtr& last_node_ptr);  
    
private:
    uint64_t                 atomic_inc_index;    //atomic inc index  
    uint8_t                  maxRetryNum;
    int                      connTimeOutMs;
    int                      sendTimeOutMs;
    int                      recvTimeOutMs;
    std::vector<std::string> serverIps;
    std::vector<uint32_t>    serverPorts;
    //CliCType                 cliCType;
    int                      cliCType;
    MemcachedCliPtrVec       memcachedCliPtrVec;
};
typedef std::tr1::shared_ptr<MemcachedCliPool> MemcachedCliPoolPtr;

} // namespace util

#endif
