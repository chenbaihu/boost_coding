#include "RedisConnPool.h"
#include "log.h"

namespace util {

RedisConnPool::RedisConnPool(std::string host, int port/*=6396*/, int pool_size/*=1024*/, 
            int conn_timeout_ms/*=100*/, int send_timeout_ms/*=100*/, int recv_timeout_ms/*=100*/)
{
    host_            = host;
    port_            = port;
    pool_size_       = pool_size;
    conn_timeout_ms_ = conn_timeout_ms; 
    send_timeout_ms_ = send_timeout_ms; 
    recv_timeout_ms_ = recv_timeout_ms;  
    
    log_notice("host=%s\tport=%d\tpool_size=%d\tconn_timeout_ms=%d\tsend_timeout_ms=%d\trecv_timeout_ms%d", 
                host_.c_str(),
                port_,
                pool_size_, 
                conn_timeout_ms_,
                send_timeout_ms_, 
                recv_timeout_ms_);

    busy_conn_num_   = 0; 

    if (pthread_mutex_init(&mutex_, NULL) != 0) { 
        log_error("RedisConnPool pthread_mutex_init failed");
    }

    for(int i = 0; i < pool_size_; i++) {
        redisContext* conn = Connect();
        if (conn == NULL) { 
            //i--;
            continue;
        }
        free_conns_.push_back(conn);
    }
} 

RedisConnPool::~RedisConnPool()
{
    std::list<redisContext*>::iterator freeite ;
    for(freeite = this->free_conns_.begin(); freeite != this->free_conns_.end(); freeite++) {
        redisFree(*freeite);
    }
    pthread_mutex_destroy(&mutex_);
}


redisContext* RedisConnPool::Connect()
{
    struct timeval tv;
    tv.tv_sec =  conn_timeout_ms_/1000;
    tv.tv_usec = (conn_timeout_ms_%1000)*1000;

    redisContext *c = redisConnectWithTimeout(host_.c_str(), port_, tv);
    if(c == NULL) {
        return NULL;
    }

    if(c->err) { 
        log_error("connect redis failed. ip=[%s] port=[%d],c->strerr=%s", 
                    host_.c_str(), port_, c->errstr);
        redisFree(c);
        c = NULL;
        return NULL;
    }

    struct timeval rtv;
    rtv.tv_sec =   recv_timeout_ms_/1000;
    rtv.tv_usec =  (recv_timeout_ms_%1000)*1000;
    redisSetTimeout(c, rtv);
    return c;
}

redisContext* RedisConnPool::BorrowRedisConn()
{
    redisContext *conn = NULL;

    pthread_mutex_lock(&mutex_);
    if(!free_conns_.empty()) {
        busy_conn_num_++;
        conn = free_conns_.front();
        free_conns_.pop_front();
    } else if(busy_conn_num_ < pool_size_) {
        conn = Connect();
        if(conn != NULL && !conn->err) {
            busy_conn_num_++;
        }
        if (conn->err) {
            conn = NULL;
        }
    }
    pthread_mutex_unlock(&mutex_);
    return conn;
}

void RedisConnPool::ReturnRedisConn(redisContext *conn)
{
    if(conn == NULL) {
        log_warn("msg=[returnredisconn conn is NULL] conn=NULL");
        return;
    }

    pthread_mutex_lock(&mutex_);
    if(conn->err) {
        redisFree(conn);
        conn = Connect();
        if(conn != NULL && !conn->err) {
            free_conns_.push_back(conn);
        }
    } else {
        free_conns_.push_back(conn);
    }
    busy_conn_num_--;

    pthread_mutex_unlock(&mutex_);
}

int RedisConnPool::BusyConnCount()
{
    pthread_mutex_lock(&mutex_);
    int busy_conn = busy_conn_num_;
    pthread_mutex_unlock(&mutex_);
    return busy_conn;
}

int RedisConnPool::FreeConnCount()
{
    pthread_mutex_lock(&mutex_);
    int avail_conn = free_conns_.size();
    pthread_mutex_unlock(&mutex_);
    return avail_conn;
}

} //namespace util
