#include "MemcachedCli.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>

namespace util {
MemcachedCli::MemcachedCli(const std::string& ip, 
            uint16_t port/*=11211*/, 
            int conn_time_out_ms/*=100*/, 
            int send_time_out_ms/*=200*/, 
            int recv_time_out_ms/*=200*/, 
            CliCType cli_c_type/*=kCKVCliSC*/)
    :serverIp(ip),
     serverPort(port),
     connTimeOutMs(conn_time_out_ms), 
     sendTimeOutMs(send_time_out_ms), 
     recvTimeOutMs(recv_time_out_ms), 
     cliCType(cli_c_type), 
     pCKV(NULL)
{ 
    if (serverIp.empty()) {
        serverIp = std::string("127.0.0.1");
    }
    serverInfo << "[" 
               << serverIp      << ":"
               << serverPort    << ":"
               << connTimeOutMs << ":"
               << sendTimeOutMs << ":"
               << recvTimeOutMs << ":"
               << (int)cliCType 
               << "]";
}

MemcachedCli::MemcachedCli(nodemanager::NodeInfoPtr& node_ptr, 
            int conn_time_out_ms/*=100*/, 
            int send_time_out_ms/*=100*/, 
            int recv_time_out_ms/*=100*/, 
            CliCType cli_c_type/*=kCKVCliSC*/)
    :serverIp(node_ptr->GetIp()),
     serverPort((uint16_t)node_ptr->GetPort()),
     connTimeOutMs(conn_time_out_ms), 
     sendTimeOutMs(send_time_out_ms), 
     recvTimeOutMs(recv_time_out_ms), 
     cliCType(cli_c_type),
     nodePtr(node_ptr), 
     pCKV(NULL)
{ 
    if (serverIp.empty()) {
        serverIp = std::string("127.0.0.1");
    }
    serverInfo << "[" 
               << serverIp      << ":"
               << serverPort    << ":"
               << connTimeOutMs << ":"
               << sendTimeOutMs << ":"
               << recvTimeOutMs << ":"
               << (int)cliCType 
               << "]";
}

MemcachedCli::~MemcachedCli() 
{ 
    Close();
} 

//succ return 0, failed return 1 
int MemcachedCli::MultiGet(MGetKeyType& keys, MGetResultType& result)
{
    //log_notice("keys.size=%d", (int)keys.size());
    if (keys.empty()) { 
        log_error("keys empty...");
        result.clear();
        return 0;
    }

    if (MAX_KEY_NUM<keys.size()) {
        log_error("MAX_KEY_NUM<keys.size");
        result.clear();
        return 1;
    }

    //短连接
    if (cliCType==kCKVCliSC) {
        return MultiGetImpl(keys, result);
    }

    //长连接
    mutex.lock();
    int ret = MultiGetImpl(keys, result);
    mutex.unlock();
    return ret;
}

int MemcachedCli::MultiGetImpl(MGetKeyType& keys, MGetResultType& result) 
{
    if (!Open()) {
        Close();
        return 1;
    }
    assert(pCKV!=NULL);

    int ret = pCKV->multiget(keys, result);                                  
    if (ret==0) {
        return 0;
    }
    log_error("serverInfo=%s multiget failed", serverInfo.str().c_str());
    Close();
    return 1;
} 

int MemcachedCli::Set(std::string& key, 
            std::string& value, 
            int32_t exptime_time_s/*=3000s*/, 
            uint32_t flag/*=0*/)
{
    if (key.empty()) {
        log_error("key empty...");
        return 0;
    }

    //短连接
    if (cliCType==kCKVCliSC) {
        return SetImpl(key, value, exptime_time_s, flag);
    }

    //长链接
    mutex.lock();
    int ret = SetImpl(key, value, exptime_time_s, flag);
    mutex.unlock();
    return ret;
}

int MemcachedCli::SetImpl(std::string& key, 
            std::string& value, 
            int32_t exptime_time_s/*=3000s*/, 
            uint32_t flag/*=0*/)
{ 
    if (!Open()) {
        log_error("key=%s value.size=%ld serverInfo=%s Open failed", 
                    key.c_str(), value.size(), serverInfo.str().c_str());
        Close();
        return 1;
    }
    assert(pCKV!=NULL);
    
    int ret = pCKV->set((char*)&key[0], 
                key.size(), 
                &value[0], 
                (int)value.size(), 
                exptime_time_s, 
                flag); 
    if (ret==0) { 
        return 0;
    }
    log_error("key=%s value.size=%ld serverInfo=%s set failed", 
                key.c_str(), value.size(), serverInfo.str().c_str());
    Close();
    return 1;   
}

bool MemcachedCli::Open()
{
    if (cliCType==kCKVCliSC) { 
        Close();
    }
    if (pCKV!=NULL) {   //已经open
        return true;
    } 
    pCKV = new ssMemcached;
    if (pCKV->initServer((char*)serverIp.c_str(), 
                    serverPort, 
                    connTimeOutMs, 
                    sendTimeOutMs,
                    recvTimeOutMs)!=0) {
        Close();
        log_error("serverInfo=%s initServer failed", serverInfo.str().c_str());
        return false;
    }
    return true;
} 

void MemcachedCli::Close() 
{ 
    if (pCKV!=NULL) { 
        pCKV->destory();
        delete pCKV;
    }
    pCKV = NULL;
}  

} // namespace util
