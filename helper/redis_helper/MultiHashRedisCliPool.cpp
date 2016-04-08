#include "MultiHashRedisCliPool.h"
#include "RedisCliPool.h"

#include "log.h"

#include <assert.h>

namespace util {

MultiHashRedisCliPool::MultiHashRedisCliPool(const RedisCliPoolPtr& redis_cli_pool_ptr)
    : redis_cli_pool_ptr_(redis_cli_pool_ptr) 
{
}

MultiHashRedisCliPool::~MultiHashRedisCliPool() 
{
}

bool MultiHashRedisCliPool::MultiHashMSet(const MultiHashMSetReq& mh_mset_req, MultiHashMSetRsp& mh_mset_rsp, int max_try_times)
{
    if (mh_mset_req.mh_mset_key_values.empty()) {
        return true;
    }
 
    assert(redis_cli_pool_ptr_!=NULL); 

    bool ret = true;
    MultiHashKeyValues::const_iterator cit = mh_mset_req.mh_mset_key_values.begin();
    for (; cit!=mh_mset_req.mh_mset_key_values.end(); cit++) {
        if (!redis_cli_pool_ptr_->HMSet(cit->first, cit->second, max_try_times)) {
            log_error("HMSet %s failed", SafeStr(cit->first.c_str()));
            mh_mset_rsp.mh_mset_failed_keys.insert(cit->first);
            ret = false;
        }
    }
    return ret;
}

bool MultiHashRedisCliPool::MultiHashMDel(const MultiHashMDelReq& mh_mdel_req, MultiHashMDelRsp& mh_mdel_rsp, int max_try_times)
{
    if (mh_mdel_req.mh_mdel_keys.empty()) {
        return true;
    }
    
    assert(redis_cli_pool_ptr_!=NULL); 

    bool ret = true;
    MultiHashKeys::const_iterator cit = mh_mdel_req.mh_mdel_keys.begin();
    for (; cit!=mh_mdel_req.mh_mdel_keys.end(); cit++) {
        if (redis_cli_pool_ptr_->HMDel(cit->first, cit->second, max_try_times)<0) {
            mh_mdel_rsp.mh_mdel_failed_keys.insert(cit->first);
            ret = false;
        }
    }
    return ret;
}

bool MultiHashRedisCliPool::MultiHashMGet(const MultiHashMGetReq& mh_mget_req, MultiHashMGetRsp& mh_mget_rsp, int max_try_times)
{
    if (mh_mget_req.mh_mget_keys.empty()) {
        return true;
    }

    assert(redis_cli_pool_ptr_!=NULL); 

    bool ret = true; 
    MultiHashKeys::const_iterator cit = mh_mget_req.mh_mget_keys.begin(); 
    for (; cit!=mh_mget_req.mh_mget_keys.end(); cit++) { 
        if (!redis_cli_pool_ptr_->HMGet(cit->first, cit->second, mh_mget_rsp.mh_mget_key_values[cit->first], max_try_times)) {
            mh_mget_rsp.mh_mget_failed_keys.insert(cit->first);
            ret = false;
        }
    }
    return ret;
}

} //namespace util 
