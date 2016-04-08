#ifndef _UTIL_REDIS_CONNPOOL_H_
#define _UTIL_REDIS_CONNPOOL_H_

#include <tr1/functional>
#include <tr1/memory>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <hiredis.h>
#include <pthread.h>
#include <list>

namespace util { 

class RedisConnPool {
public:
    RedisConnPool(std::string host, int port=6396, int pool_size=1024, 
                int conn_timeout_ms=100, int send_timeout_ms=100, int recv_timeout_ms=100);
    ~RedisConnPool();

public:
    redisContext * BorrowRedisConn();
    void ReturnRedisConn(redisContext *conn);
    int FreeConnCount();
    int BusyConnCount();

private:
    std::string host_;
    int         port_;
    int         pool_size_;
    int         conn_timeout_ms_;
    int         send_timeout_ms_;
    int         recv_timeout_ms_;
    int         busy_conn_num_;

private:
    pthread_mutex_t          mutex_;
    std::list<redisContext*> free_conns_;

private:
    redisContext* Connect();
}; 
typedef std::tr1::shared_ptr<RedisConnPool> RedisConnPoolPtr;

} // namespace util

#endif //#ifndef _UTIL_REDIS_CONNPOOL_H_
