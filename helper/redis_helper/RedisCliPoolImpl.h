#ifndef _UTIL_REDIS_CLI_POOL_IMPL_H_
#define _UTIL_REDIS_CLI_POOL_IMPL_H_

#include <tr1/functional>
#include <tr1/memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

struct  redisContext;
struct  redisReply;

namespace util {

typedef std::map<std::string/*field_name*/, std::string/*field_value*/> FieldValueMap;
typedef std::set<std::string/*field_name*/> FieldSet;
typedef std::set<std::string/*field_name*/> ValueSet; 

std::string DumpFieldValueStr(const FieldValueMap& field_value_map);
std::string DumpSetStr(const std::set<std::string/*field*/> field_set); 

class   RedisConnPool;
typedef std::tr1::shared_ptr<RedisConnPool> RedisConnPoolPtr; 

struct RedisConnect {
public:
    redisContext*    redis_ctx;
    RedisConnPoolPtr redis_conn_pool_ptr;

public:
    RedisConnect() {
        redis_ctx= NULL;
    }
    ~RedisConnect() {
        redis_ctx= NULL;
    }

public:
    RedisConnect(const RedisConnect& redis_conn_ref) { 
        redis_ctx           = redis_conn_ref.redis_ctx;
        redis_conn_pool_ptr = redis_conn_ref.redis_conn_pool_ptr;
    }
    RedisConnect& operator=(const RedisConnect& redis_conn_ref) {
        redis_ctx           = redis_conn_ref.redis_ctx;
        redis_conn_pool_ptr = redis_conn_ref.redis_conn_pool_ptr;
        return *this;
    }
};

class RedisCliPoolImpl {
public:
    RedisCliPoolImpl();
    ~RedisCliPoolImpl();

public:
    bool Init(std::vector<std::string> ips, std::vector<uint32_t> ports, int pool_size=1024, 
                int conn_timeout_ms=100, int send_timeout_ms=100, int recv_timeout_ms=100);

public:
    int  HSetImpl(const std::string& key, 
            const std::string& field_name, 
            const std::string& field_value, 
            bool nx=false);

    bool HMSetImpl(const std::string& key, const FieldValueMap& values); 

public:
    int  HMDelImpl(const std::string& str_cmd);

public:
    bool HGetImpl(const std::string& str_cmd, std::string& value);
    bool HMGetImpl(const std::string& str_cmd, const FieldSet& fields, FieldValueMap& values); 

public:
    bool HKeysImpl(const std::string& str_cmd, FieldSet& fields);
    bool HValsImpl(const std::string& str_cmd, ValueSet& values); 

private:
    void GetReplyValues(redisReply *reply, const FieldSet& fields, FieldValueMap& values);

private:
    RedisConnect BorrowRedisConn();
    void ReturnRedisConn(RedisConnect redis_conn);

private:
    std::vector<RedisConnPoolPtr> redis_conn_pool_ptr_vec_;
};
typedef std::tr1::shared_ptr<RedisCliPoolImpl> RedisCliPoolImplPtr;

} // namespace util

#endif // #ifndef _UTIL_REDIS_CLI_POOL_IMPL_H_


