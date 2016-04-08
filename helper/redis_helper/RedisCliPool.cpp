#include "RedisCliPool.h" 

#include <sstream>
#include <stdlib.h>
#include <time.h>

#include "RedisCliPoolImpl.h" 
#include "log.h"
#include "base64.h"

namespace util {

RedisCliPool::RedisCliPool()
{
}

RedisCliPool::~RedisCliPool() 
{
    redis_cli_pool_impl_ptr_.reset();
}

bool RedisCliPool::Init(std::vector<std::string> ips, 
            std::vector<uint32_t> ports, 
            int pool_size/*=1024*/, 
            int conn_timeout_ms/*=100*/, 
            int send_timeout_ms/*=100*/, 
            int recv_timeout_ms/*=100*/)
{
    redis_cli_pool_impl_ptr_.reset(new RedisCliPoolImpl()); 
    if (redis_cli_pool_impl_ptr_==NULL) {
        return false;
    }
    return redis_cli_pool_impl_ptr_->Init(ips, ports, pool_size, conn_timeout_ms, send_timeout_ms, recv_timeout_ms);
}

int RedisCliPool::HSet(const std::string& key, 
        const std::string& field_name, 
        const std::string& field_value, 
        bool nx/*=false*/, 
        int max_try_times/*=2*/)
{
    if (key.empty() || 
            field_name.empty() || 
            (field_value.length()==0) || 
            max_try_times<=0 || 
            max_try_times>10) {
        log_error("req params error");
        return -2;
    }

    int i = 0;
    for (i=0; i<max_try_times; i++) {
        int ret = redis_cli_pool_impl_ptr_->HSetImpl(key, field_name, field_value, nx); 
        if (ret>=0) { // 操作成功
            log_notice("HSet key=%s value_b64=%s succ",
                        key.c_str(), 
                        util::Base64::encode(field_value).c_str());
            return ret;
        }
        log_error("key=%s value_b64=%s failed i=%d", 
                key.c_str(), 
                util::Base64::encode(field_value).c_str(),
                i);
    } 
    return -3;
}

bool RedisCliPool::HMSet(const std::string& key, 
        const FieldValueMap& values, 
        int max_try_times/*=2*/)
{
    if (key.empty() || values.empty()) {
        log_warn("req params error");
        return true;
    }
   
    int i = 0;
    for (i=0; i<max_try_times; i++) {
        if (redis_cli_pool_impl_ptr_->HMSetImpl(key, values)) {
            log_notice("key=%s values.size=%ld succ",
                        SafeStr(key.c_str()), 
                        values.size());
            return true;
        }
        log_error("key=%s faied i=%d", key.c_str(), i);
    }
    return false;
}

int RedisCliPool::HMDel(const std::string& key, const FieldSet& fields, int max_try_times/*=2*/)
{
    if (key.empty() || fields.empty()) { 
        return 0;
    } 

    std::stringstream str_cmd; 
    str_cmd << "HDEL " << key; 

    FieldSet::const_iterator cit = fields.begin();
    for (; cit!=fields.end(); cit++) {
        if ((*cit).empty()) { 
            continue;
        }
        str_cmd << " " << *cit; //TODO *cit Trim 
    } 

    int i = 0;
    for (i=0; i<max_try_times; i++) { 
        int ret = redis_cli_pool_impl_ptr_->HMDelImpl(str_cmd.str()); 
        if (ret>=0) { // 操作成功
            log_notice("str_cmd=%s succ", SafeStr(str_cmd.str().c_str()));
            return ret;
        }
        log_error("str_cmd=%s failed i=%d", str_cmd.str().c_str(), i);
    }
    return -3;
}

bool RedisCliPool::HGet(const std::string& key, const std::string& field, std::string& value, 
            int max_try_times/*=2*/)
{
    value.clear();
    if (key.empty() || field.empty() || max_try_times<=0 || max_try_times>10) {
        log_error("req params error");
        return false;
    } 

    std::stringstream str_cmd;
    str_cmd << "HGET " << key << " " << field;

    int i = 0;
    for (i=0; i<max_try_times; i++) {
        if (redis_cli_pool_impl_ptr_->HGetImpl(str_cmd.str(), value)) {
            #ifdef _DEBUG
            log_notice("str_cmd=%s succ", SafeStr(str_cmd.str().c_str()));
            #endif
            return true;
        }
        log_error("str_cmd=%s failed i=%d", str_cmd.str().c_str(), i);
    }
    return false; 
}

bool RedisCliPool::HMGet(const std::string& key, const FieldSet& fields, FieldValueMap& values, 
            int max_try_times/*=2*/)
{
    values.clear(); 
    if (key.empty() || fields.empty() || max_try_times<=0 || max_try_times>10) {
        log_error("req params error");
        return false;
    } 

    std::stringstream str_cmd;
    str_cmd << "HMGET " << key;

    FieldSet::const_iterator cit = fields.begin();
    for (; cit!=fields.end(); cit++) {
        if ((*cit).empty()) { 
            continue;
        }
        str_cmd << " " << *cit; //TODO *cit Trim 
    } 

    for (int i=0; i<max_try_times; i++) {
        if (redis_cli_pool_impl_ptr_->HMGetImpl(str_cmd.str(), fields, values)) {
            #ifdef _DEBUG
            log_notice("str_cmd=%s fields.size=%ld values.size=%ld", 
                        SafeStr(str_cmd.str().c_str()),
                        fields.size(), 
                        values.size());
            #endif
            return true;
        }
        log_error("str_cmd=%s failed i=%d", str_cmd.str().c_str(), i);
    }  
    return false;
} 

bool RedisCliPool::HGetAll(const std::string& key, FieldValueMap& values, 
            int max_try_times/*=2*/)
{
    values.clear(); 
    if (key.empty() || max_try_times<=0 || max_try_times>10) {
        log_error("req params error");
        return false;
    } 

    std::stringstream str_cmd;
    str_cmd << "HGETALL " << key; 

    FieldSet fields;

    int i = 0;
    for (i=0; i<max_try_times; i++) {
        if (redis_cli_pool_impl_ptr_->HMGetImpl(str_cmd.str(), fields, values)) {
            log_notice("str_cmd=%s values.size=%ld", 
                        SafeStr(str_cmd.str().c_str()),
                        values.size());
            return true;
        }
        log_error("str_cmd=%s failed i=%d", str_cmd.str().c_str(), i);
    }
    return false;
}

bool RedisCliPool::HKeys(const std::string& key, FieldSet& fields, int max_try_times/*=2*/)
{
    fields.clear();
    //TODO...
    return true;
}

bool RedisCliPool:: KVals(const std::string& key, ValueSet& values, int max_try_times/*=2*/)
{
    values.clear();
    //TODO...
    return true;
}

} // namespace util
