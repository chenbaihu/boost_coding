#ifndef _UTIL_REDIS_CLI_POOL_H_
#define _UTIL_REDIS_CLI_POOL_H_

#include <tr1/functional>
#include <tr1/memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

namespace util {

typedef std::map<std::string/*field_name*/, std::string/*field_value*/> FieldValueMap;
typedef std::set<std::string/*field_name*/> FieldSet;
typedef std::set<std::string/*field_name*/> ValueSet;

class   RedisCliPoolImpl;
typedef std::tr1::shared_ptr<RedisCliPoolImpl> RedisCliPoolImplPtr;

class RedisCliPool {
public:
    RedisCliPool();
    ~RedisCliPool();

public:
    bool Init(std::vector<std::string> ips, std::vector<uint32_t> ports, int pool_size=1024, 
                int conn_timeout_ms=100, int send_timeout_ms=100, int recv_timeout_ms=100);

public:
    int  HSet(const std::string& key, const std::string& field_name, const std::string& field_value, 
                bool nx=false, int max_try_times=2);
    bool HMSet(const std::string& key, const FieldValueMap& values, int max_try_times=2); 

public:
    int  HMDel(const std::string& key, const FieldSet& fields, int max_try_times=2);

public:
    bool HGet(const std::string& key, const std::string& field, std::string& value, 
                int max_try_times=2); 
    bool HGetAll(const std::string& key, FieldValueMap& values, 
                int max_try_times=2); 
    bool HMGet(const std::string& key, const FieldSet& fields, FieldValueMap& values, 
                int max_try_times=2); 

public:
    bool HKeys(const std::string& key, FieldSet& fields, int max_try_times=2);
    bool KVals(const std::string& key, ValueSet& values, int max_try_times=2);

private:
    RedisCliPoolImplPtr redis_cli_pool_impl_ptr_;
}; 
typedef std::tr1::shared_ptr<RedisCliPool> RedisCliPoolPtr;

} // namespace util

#endif // #ifndef _UTIL_REDIS_CLI_POOL_H_


