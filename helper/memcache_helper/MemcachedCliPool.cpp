#include "MemcachedCliPool.h"

#include <stdlib.h>
#include <time.h>

#include "NodeMGR.h"
#include "log.h"
#include "time_helper.h" 
#include "MemcachedCli.h"

namespace { /*{{{*/
const static std::string _s_ckv_node_mgr_name = "ckv";
} /*}}}*/

namespace util {
#define SCFree(cliP) {              \
    if (cliCType==kCKVCliSC) {      \
        if (cliP!=NULL) {           \
            delete cliP;            \
        }                           \
        cliP = NULL;                \
    }                               \
}

MemcachedCliPool::MemcachedCliPool()
    : atomic_inc_index(0),
      maxRetryNum(1), 
      connTimeOutMs(100), 
      sendTimeOutMs(200), 
      recvTimeOutMs(200)
{
    srandom(time(NULL));
    atomic_inc_index = (uint64_t)random();
    serverIps.clear();
    serverPorts.clear();
}

MemcachedCliPool::~MemcachedCliPool()
{
    serverIps.clear();    
    serverPorts.clear(); 
}

bool MemcachedCliPool::Init(const std::vector<std::string>& ips, 
            const std::vector<uint32_t>& ports, 
            int conn_timeout_ms/*=100*/, 
            int send_timeout_ms/*=200*/, 
            int recv_timeout_ms/*=200*/, 
            int max_retry_num/*=1*/, 
            int cli_c_type/*=kCKVCliSC*/) 
{ 
    if (ips.empty() || ports.empty()) {
        log_error("ips.empty || ports.empty");
        return false;
    }

    if (conn_timeout_ms<=0 || send_timeout_ms <=0 || recv_timeout_ms<=0) {
        log_error("conn_timeout_ms<=0 || send_timeout_ms <=0 || recv_timeout_ms<=0");
        return false;
    }

    if (max_retry_num<0 || max_retry_num>10) {
        log_error("max_retry_num<0 || max_retry_num>10");
        return false;
    }

    serverIps     = ips;
    serverPorts   = ports;
    connTimeOutMs = conn_timeout_ms;
    sendTimeOutMs = send_timeout_ms;
    recvTimeOutMs = recv_timeout_ms;
    maxRetryNum   = max_retry_num;
    cliCType      = cli_c_type; 
    log_notice("ips.size=%ld\t"
                "ports.size=%ld\t"
                "conn_time_ms=%d\t"
                "send_timeout_ms=%d\t"
                "recv_timeout_ms=%d\t"
                "max_retry_num=%d\t"
                "cli_c_type=%d", 
                serverIps.size(), 
                serverPorts.size(), 
                connTimeOutMs, 
                sendTimeOutMs,
                recvTimeOutMs, 
                maxRetryNum, 
                cliCType);   
    if (cliCType==kCKVCliSC) {
        return true;
    } 
    return InitLCPool(); 
}

//succ return 0, failed return 1
int MemcachedCliPool::MultiGet(MGetKeyType& keys, MGetResultType& result)
{ 
    nodemanager::NodeInfoPtr node_ptr;
    int ret = 1;
    for (int do_times=0; do_times<maxRetryNum; do_times++) {
        uint64_t beg_ms = TimeHelper::timems();
        
        MemcachedCli* cliP = GetMemcacheCli(node_ptr);
        if (cliP==NULL) {
            ret = 1;
            log_error("do_times=%d cliP==NULL", do_times);
            continue;
        }

        node_ptr = cliP->NodePtr();
        ret = cliP->MultiGet(keys, result);
        SCFree(cliP); 
        if (ret!=0) {
            log_error("do_times=%d MultiGet failed", do_times);
            ret = 1;
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kBad);
            continue;   
        }

        int total_cost_ms = TimeConsumeMs(beg_ms); 
        int bad_cost_ms = 50; /*ms*/
        if (total_cost_ms>bad_cost_ms) {
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kBad);
        } else { 
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kHealth);
        }
        ret = 0;  // succ
        break;
    } 
    return ret; 
}

//succ return 0, failed return 1
//int MemcachedCliPool::MultiSet(MCKVLine& lines)
//{
//    if (lines.empty()) {
//        return 0;
//    }
//
//    //获取一个可用连接
//    nodemanager::NodeInfoPtr node_ptr;    
//    MemcachedCli*            cliP=NULL;
//    int ret = 1;
//
//    for (size_t li=0; li<lines.size(); li++) { 
//        CKVLine& line = lines[li];
//        if (cliP==NULL) {
//            cliP = GetMemcacheCli(node_ptr); 
//            if (cliP==NULL) {
//                log_error("MultiSet lines.size=%ld cliP==NULL", lines.size());
//                return 1;
//            }
//        }
//
//        node_ptr = cliP->NodePtr();
//        ret = cliP->Set(line.key, line.val, line.ttl, line.flag);
//        if (ret!=0) {
//            SCFree(cliP); 
//            ret = 1;
//            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kBad);
//            log_error("MultiSet key=%s val=%s ttl=%d flag=%u failed", 
//                        line.key.c_str(), 
//                        line.val.c_str(), 
//                        line.ttl, 
//                        line.flag);
//            continue;
//        }
//        NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kHealth);
//        ret = 0;  // succ
//    }
//}

//succ return 0, failed return 1 
//int MemcachedCliPool::Set(CKVLine& line)
//{
//    return Set(line.key, line.val, line.ttl, line.flag);
//}

//succ return 0, failed return 1
int MemcachedCliPool::Set(std::string& key, 
            std::string& value,
            int32_t exptime_time_s/*=3000 s*/, 
            uint32_t flag/*=0*/) 
{
    nodemanager::NodeInfoPtr node_ptr;
    int ret = 1;
    for (int do_times=0; do_times<maxRetryNum; do_times++) { 
        uint64_t beg_ms = TimeHelper::timems();

        MemcachedCli* cliP = GetMemcacheCli(node_ptr);
        if (cliP==NULL) {
            ret = 1;
            log_error("do_times=%d\t"
                        "key=%s\t"
                        "value=%ld\t"
                        "exptime_time_s=%d\t"
                        "cliP==NULL", 
                        do_times, 
                        key.c_str(), 
                        value.size(), 
                        exptime_time_s);
            continue;
        } 
        node_ptr = cliP->NodePtr();
        ret = cliP->Set(key, value, exptime_time_s, flag);
        SCFree(cliP); 
        if (ret!=0) {
            ret = 1;
            log_error("do_times=%d key=%s value.size=%ld exptime_time_s=%d Set failed", 
                        do_times, key.c_str(), value.size(), exptime_time_s);
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kBad);
            continue;
        }

        int total_cost_ms = TimeConsumeMs(beg_ms); 
        int bad_cost_ms = 50; /*ms*/
        if (total_cost_ms>bad_cost_ms) {
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kBad);
        } else { 
            NodeMGR().Vote(&_s_ckv_node_mgr_name, node_ptr, nodemanager::kHealth);
        }
        ret = 0;  // succ
        break;
    }  
    return ret; 
} 

bool MemcachedCliPool::InitLCPool() 
{
    for (size_t mi=0; mi<20; mi++) { //跟一台机器建立很多个链接
        for (size_t porti=0; porti<serverPorts.size(); porti++) { 
            for (size_t ipi=0; ipi<serverIps.size(); ipi++) { 
                MemcachedCliPtr mCliPtr(new MemcachedCli(
                                serverIps[ipi], 
                                (uint16_t)serverPorts[porti], 
                                connTimeOutMs,
                                sendTimeOutMs, 
                                recvTimeOutMs, 
                                (util::CliCType)cliCType)); 
                memcachedCliPtrVec.push_back(mCliPtr);
            }
        }
    }
    log_notice("memcachedCliPtrVec.size=%ld", memcachedCliPtrVec.size());
    return true;
}

MemcachedCli* MemcachedCliPool::GetMemcacheCli(nodemanager::NodeInfoPtr& last_node_ptr) 
{
    if (cliCType==kCKVCliSC) {
        return GetMemcacheCliSC(last_node_ptr); 
    }
    if (cliCType==kCKVCliLC) {
        return GetMemcacheCliLC(last_node_ptr); 
    }
    log_error("cliCType=%d", cliCType);
    return NULL; //为空有问题 
}

MemcachedCli* MemcachedCliPool::GetMemcacheCliSC(nodemanager::NodeInfoPtr& last_node_ptr) 
{
    MemcachedCli* cliP = GetGoodMemcacheCliSC(last_node_ptr);
    if (cliP!=NULL) {
        return cliP;
    }

    if (serverIps.empty() || serverPorts.empty()) {
        return NULL; //为空有问题 
    }   
    uint64_t index   = __sync_fetch_and_add(&atomic_inc_index, 1); 
    size_t ipIndex   = index%(uint64_t)serverIps.size();
    size_t portIndex = (index/(uint64_t)serverIps.size())%serverPorts.size(); 
    cliP = new MemcachedCli(serverIps[ipIndex], 
                serverPorts[portIndex], 
                connTimeOutMs, 
                sendTimeOutMs,
                recvTimeOutMs);
    return cliP;
} 

MemcachedCli* MemcachedCliPool::GetGoodMemcacheCliSC(nodemanager::NodeInfoPtr& last_node_ptr) 
{
    nodemanager::NodeInfoPtr node_ptr;
    if (last_node_ptr==NULL) {
        node_ptr =  NodeMGR().GetNode(_s_ckv_node_mgr_name);
    } else {
        node_ptr =  NodeMGR().GetNode(_s_ckv_node_mgr_name, last_node_ptr);   
    }

    if (node_ptr==NULL) {
        return NULL; 
    }
    MemcachedCli* cliP = new MemcachedCli(node_ptr, 
                connTimeOutMs, 
                sendTimeOutMs, 
                recvTimeOutMs);
    return cliP;
}

MemcachedCli* MemcachedCliPool::GetMemcacheCliLC(nodemanager::NodeInfoPtr& last_node_ptr) 
{ 
    if (memcachedCliPtrVec.empty()) {
        return NULL; //为空有问题 
    }
    uint64_t index  = __sync_fetch_and_add(&atomic_inc_index, 1);
    return memcachedCliPtrVec[index%memcachedCliPtrVec.size()].get();
}

} // namespace util 
