#ifndef _UTIL_MULTIHASH_REDISCLIPOOL_H_ 
#define _UTIL_MULTIHASH_REDISCLIPOOL_H_ 

#include <tr1/functional>
#include <tr1/memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

namespace util {

typedef std::map<std::string/*key*/, std::set<std::string/*field*/> >                       MultiHashKeys;
typedef std::map<std::string/*key*/, std::map<std::string/*field*/, std::string/*value*/> > MultiHashKeyValues;
typedef std::map<std::string/*field*/, std::string/*value*/>                                FieldValueMap;
typedef std::set<std::string/*field*/> FieldSet;
typedef std::set<std::string/*key*/> FailedHashKeySet;

//////MSet
struct MultiHashMSetReq {
    MultiHashKeyValues mh_mset_key_values;
};
struct MultiHashMSetRsp { 
    FailedHashKeySet   mh_mset_failed_keys;
};

//////MDel
struct MultiHashMDelReq { 
    MultiHashKeys      mh_mdel_keys;
};
struct MultiHashMDelRsp {
    FailedHashKeySet   mh_mdel_failed_keys;
};

//////MGet
struct MultiHashMGetReq { 
    MultiHashKeys      mh_mget_keys;
};
struct MultiHashMGetRsp { 
    MultiHashKeyValues mh_mget_key_values;
    FailedHashKeySet   mh_mget_failed_keys;
};

class   RedisCliPool;
typedef std::tr1::shared_ptr<RedisCliPool> RedisCliPoolPtr; 

class MultiHashRedisCliPool { 
public:
    MultiHashRedisCliPool(const RedisCliPoolPtr& redis_cli_pool_ptr);
    ~MultiHashRedisCliPool(); 

public:
    bool MultiHashMSet(const MultiHashMSetReq& mh_mset_req, MultiHashMSetRsp& mh_mset_rsp, int max_try_times=2);

    bool MultiHashMDel(const MultiHashMDelReq& mh_mdel_req, MultiHashMDelRsp& mh_mdel_rsp, int max_try_times=2);

    bool MultiHashMGet(const MultiHashMGetReq& mh_mget_req, MultiHashMGetRsp& mh_mget_rsp, int max_try_times=2);

private:
    RedisCliPoolPtr redis_cli_pool_ptr_;
};

} //namespace util

#endif //#ifndef _UTIL_MULTIHASH_REDISCLIPOOL_H_
