#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#include <map>
#include <vector>
#include <string>

#include "MemcachedCliPool.h"
#include "log.h"

int main(int argc, char* argv[]) 
{
    signal(SIGPIPE, SIG_IGN);   

    std::vector<std::string> ips;
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));
    ips.push_back(std::string("127.0.0.1"));

    std::vector<uint32_t>    ports;
    ports.push_back(11218);
    ports.push_back(11218);
    ports.push_back(11218);
    
    int conn_timeout_ms = 100;
    int send_timeout_ms = 200;
    int recv_timeout_ms = 200;
    int max_try_times = 1;
    int cli_type      = 0; //kCKVCliSC;

    //log_init((enum LogLevel)0, "memcache_tool.log", "./log");

    util::MemcachedCliPoolPtr g_ckv_cli_pool_ptr(new util::MemcachedCliPool()); 
    if (!g_ckv_cli_pool_ptr->Init(ips, ports, conn_timeout_ms, send_timeout_ms, recv_timeout_ms, max_try_times, cli_type)) {
        log_error("g_ckv_cli_pool_ptr->Init failed");
        exit(1);
    }

    std::string k1("k1"); 
    std::string v1("v11111111111111111111");

    if (g_ckv_cli_pool_ptr->Set(k1, v1, 3000, (uint32_t)0)!=0) { 
        log_error("g_ckv_cli_pool_ptr->Set failed");
        exit(0);
    }

    std::string k2("k2"); 
    std::string v2("v2222222222222222222222222222222222222222222");
    
    if (g_ckv_cli_pool_ptr->Set(k2, v2, 3000, (uint32_t)0)!=0) { 
        log_error("g_ckv_cli_pool_ptr->Set failed");
        exit(0);
    }

    std::vector<std::string> ks;
    ks.push_back("k1");
    ks.push_back("k2");
    ks.push_back("k3");
    ks.push_back("k4");

    std::map<std::string/*key*/, std::string/*value*/> kvmap;  
    if (g_ckv_cli_pool_ptr->MultiGet(ks, kvmap)!=0) {
        log_error("g_ckv_cli_pool_ptr->MultiGet failed");
        exit(0);
    }

    std::map<std::string, std::string>::const_iterator cit = kvmap.begin();
    for(; cit!=kvmap.end(); cit++) { 
        printf("%s<--->%s\n", cit->first.c_str(), cit->second.c_str());
    }
    return 0;
}
